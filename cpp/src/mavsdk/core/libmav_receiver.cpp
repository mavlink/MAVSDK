#include "libmav_receiver.hpp"
#include "mavsdk_impl.hpp"
#include <mav/MessageSet.h>
#include <mav/BufferParser.h>
#include <nlohmann/json.hpp>
#include <variant>
#include <cstring>
#include <array>
#include <string>
#include <string_view>
#include <vector>
#include "log.hpp"

namespace mavsdk {

namespace {

// A handful of MAVLink messages declare a field as char[] but actually use it
// to carry raw binary data. The most common case is the extended-parameter
// protocol, which packs a typed parameter value as little-endian bytes into
// PARAM_EXT_{VALUE,SET,ACK}.param_value (char[128]).
//
// Such fields must be represented in JSON as a byte array, not a string:
//  - libmav's getString() truncates char[] at the first NUL (strnlen), which
//    corrupts any value containing a zero byte, and
//  - the bytes are usually not valid UTF-8.
//
// This is an explicit, deliberately small allow-list. It intentionally does NOT
// match PARAM_VALUE.param_value, which is a normal float. New entries should be
// added only for fields genuinely carrying binary in a char[].
bool is_binary_char_field(const std::string& message_name, const std::string& field_name)
{
    struct BinaryField {
        std::string_view message;
        std::string_view field;
    };
    // Kept as string-view literals in a constexpr array (no static object with a
    // runtime destructor and no per-lookup allocation) so it is safe to read
    // from the message-delivery thread, including during process teardown.
    static constexpr std::array<BinaryField, 3> binary_fields{{
        {"PARAM_EXT_VALUE", "param_value"},
        {"PARAM_EXT_SET", "param_value"},
        {"PARAM_EXT_ACK", "param_value"},
    }};
    for (const auto& binary_field : binary_fields) {
        if (field_name == binary_field.field && message_name == binary_field.message) {
            return true;
        }
    }
    return false;
}

} // namespace

LibmavReceiver::LibmavReceiver(MavsdkImpl& mavsdk_impl) : _mavsdk_impl(mavsdk_impl)
{
    // No need for individual BufferParser - we'll use MavsdkImpl's thread-safe parsing

    if (const char* env_p = std::getenv("MAVSDK_MAVLINK_DIRECT_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            _debugging = true;
        }
    }
}

LibmavReceiver::~LibmavReceiver() = default;

void LibmavReceiver::set_new_datagram(char* datagram, unsigned datagram_len)
{
    // Append new data to accumulation buffer (for serial where messages can span multiple reads)
    _accumulation_buffer.insert(
        _accumulation_buffer.end(),
        reinterpret_cast<uint8_t*>(datagram),
        reinterpret_cast<uint8_t*>(datagram) + datagram_len);

    // Prevent unbounded growth
    if (_accumulation_buffer.size() > ACCUMULATION_BUFFER_SIZE) {
        // Drop oldest bytes to make room
        size_t bytes_to_drop = _accumulation_buffer.size() - ACCUMULATION_BUFFER_SIZE;
        _accumulation_buffer.erase(
            _accumulation_buffer.begin(),
            _accumulation_buffer.begin() + static_cast<ptrdiff_t>(bytes_to_drop));
    }
}

bool LibmavReceiver::parse_message()
{
    if (_accumulation_buffer.empty()) {
        return false;
    }

    // Use libmav to parse messages from the accumulation buffer
    return parse_libmav_message_from_buffer();
}

bool LibmavReceiver::parse_libmav_message_from_buffer()
{
    size_t bytes_consumed = 0;

    // Use thread-safe parsing from MavsdkImpl (handles MessageSet synchronization internally)
    auto message_opt = _mavsdk_impl.parse_message_safe(
        _accumulation_buffer.data(), _accumulation_buffer.size(), bytes_consumed);

    if (!message_opt) {
        // No complete message found - consume bytes before the magic byte (if any)
        // to avoid reprocessing garbage bytes on the next attempt
        if (bytes_consumed > 0) {
            _accumulation_buffer.erase(
                _accumulation_buffer.begin(),
                _accumulation_buffer.begin() + static_cast<ptrdiff_t>(bytes_consumed));
        }
        return false;
    }

    auto message = message_opt.value();

    if (_debugging) {
        LogDebug("Parsed message: {} (ID: {})", message.name(), message.id());
    }

    // Extract system and component IDs from header
    auto header = message.header();

    // Generate complete JSON with all field values
    std::string json = libmav_message_to_json(message);

    // Fill our message structures
    _last_libmav_message = message;
    _last_message.message_name = message.name();
    _last_message.system_id = header.systemId();
    _last_message.component_id = header.componentId();

    if (_last_libmav_message) {
        const uint32_t wire_len = _last_libmav_message->finalizedSize();
        if (wire_len > 0) {
            const uint8_t* wire_ptr = _last_libmav_message->data();
            _last_message.raw_bytes.assign(wire_ptr, wire_ptr + wire_len);
        } else {
            _last_message.raw_bytes.clear();
        }
    } else {
        _last_message.raw_bytes.clear();
    }

    // Extract target_system and target_component if present in message fields
    uint8_t target_system_id = 0;
    uint8_t target_component_id = 0;
    if (message.get("target_system", target_system_id) == mav::MessageResult::Success) {
        _last_message.target_system_id = target_system_id;
    } else {
        _last_message.target_system_id = 0;
    }
    if (message.get("target_component", target_component_id) == mav::MessageResult::Success) {
        _last_message.target_component_id = target_component_id;
    } else {
        _last_message.target_component_id = 0;
    }

    _last_message.fields_json = json;

    // Remove the consumed bytes from the accumulation buffer
    _accumulation_buffer.erase(
        _accumulation_buffer.begin(),
        _accumulation_buffer.begin() + static_cast<ptrdiff_t>(bytes_consumed));

    return true;
}

std::string LibmavReceiver::libmav_message_to_json(const mav::Message& msg) const
{
    // Use ordered_json so fields keep their MAVLink XML-definition order
    // (plain nlohmann::json would sort keys alphabetically). nlohmann handles
    // string escaping and renders NaN/Inf as null on dump().
    nlohmann::ordered_json json;
    json["message_id"] = msg.id();
    json["message_name"] = msg.name();

    // Get message definition to iterate through all fields (using thread-safe method)
    auto message_def_opt = _mavsdk_impl.get_message_definition_safe(static_cast<int>(msg.id()));

    if (message_def_opt) {
        auto& message_def = message_def_opt.get();

        // Get field names and iterate through them
        auto field_names = message_def.fieldNames();
        for (const auto& field_name : field_names) {
            // Binary char[] fields (see allow-list) are emitted as a byte array
            // of the full field width, read directly as raw bytes so the value
            // is not NUL-truncated. The field stays declared as char, so the
            // message CRC_EXTRA is unaffected.
            if (is_binary_char_field(msg.name(), field_name)) {
                std::vector<uint8_t> raw;
                if (msg.get(field_name, raw) == ::mav::MessageResult::Success) {
                    json[field_name] = raw;
                    continue;
                }
            }

            // Extract field value based on type and convert to JSON
            auto variant_opt = msg.getAsNativeTypeInVariant(field_name);
            if (variant_opt) {
                std::visit(
                    [&json, &field_name](const auto& value) {
                        using T = std::decay_t<decltype(value)>;

                        if constexpr (std::is_same_v<T, char>) {
                            // Represent a scalar char as its numeric value.
                            json[field_name] = static_cast<int>(value);
                        } else {
                            // Scalars and vectors (incl. uint8/int8, which
                            // nlohmann serializes as number arrays) assign
                            // directly; NaN/Inf become null on dump().
                            json[field_name] = value;
                        }
                    },
                    variant_opt.value());
            } else {
                // Field not present or failed to extract
                json[field_name] = nullptr;
            }
        }
    }

    // MAVLink char[] fields can carry raw binary (non-UTF-8) bytes. Use the
    // replace error handler so dump() emits U+FFFD instead of throwing (which,
    // under -fno-exceptions, would abort).
    return json.dump(-1, ' ', false, nlohmann::ordered_json::error_handler_t::replace);
}

std::optional<std::string> LibmavReceiver::message_id_to_name(uint32_t id) const
{
    return _mavsdk_impl.message_id_to_name_safe(id);
}

std::optional<int> LibmavReceiver::message_name_to_id(const std::string& name) const
{
    return _mavsdk_impl.message_name_to_id_safe(name);
}

std::optional<mav::Message> LibmavReceiver::create_message(const std::string& message_name) const
{
    return _mavsdk_impl.create_message_safe(message_name);
}

bool LibmavReceiver::load_custom_xml(const std::string& xml_content)
{
    // Note: This method should not be called directly on LibmavReceiver instances.
    // Use MavlinkDirect::load_custom_xml() instead which goes through proper channels.

    // Use thread-safe method from MavsdkImpl
    return _mavsdk_impl.load_custom_xml_to_message_set(xml_content);
}

} // namespace mavsdk
