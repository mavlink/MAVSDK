#include "libmav_receiver.h"
#include "mavsdk_impl.h"
#include <mav/MessageSet.h>
#include <mav/BufferParser.h>
#include <json/json.h>
#include <variant>
#include <cstring>
#include <sstream>
#include <cmath>
#include "log.h"

namespace mavsdk {

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
        LogDebug() << "Parsed message: " << message.name() << " (ID: " << message.id() << ")";
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
    std::ostringstream json_stream;
    json_stream << "{";
    json_stream << "\"message_id\":" << msg.id();
    json_stream << ",\"message_name\":\"" << msg.name() << "\"";

    // Get message definition to iterate through all fields (using thread-safe method)
    auto message_def_opt = _mavsdk_impl.get_message_definition_safe(static_cast<int>(msg.id()));

    if (message_def_opt) {
        auto& message_def = message_def_opt.get();

        // Get field names and iterate through them
        auto field_names = message_def.fieldNames();
        for (const auto& field_name : field_names) {
            json_stream << ",\"" << field_name << "\":";

            // Extract field value based on type and convert to JSON
            auto variant_opt = msg.getAsNativeTypeInVariant(field_name);
            if (variant_opt) {
                const auto& variant = variant_opt.value();

                // Convert variant to JSON string based on the field type
                std::visit(
                    [&json_stream](const auto& value) {
                        using T = std::decay_t<decltype(value)>;

                        if constexpr (
                            std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
                            std::is_same_v<T, uint32_t> || std::is_same_v<T, uint64_t> ||
                            std::is_same_v<T, int8_t> || std::is_same_v<T, int16_t> ||
                            std::is_same_v<T, int32_t> || std::is_same_v<T, int64_t>) {
                            json_stream << static_cast<int64_t>(value);
                        } else if constexpr (std::is_same_v<T, char>) {
                            json_stream << static_cast<int>(value);
                        } else if constexpr (
                            std::is_same_v<T, float> || std::is_same_v<T, double>) {
                            if (!std::isfinite(value)) {
                                json_stream << "null";
                            } else {
                                json_stream << value;
                            }
                        } else if constexpr (std::is_same_v<T, std::string>) {
                            json_stream << "\"" << value << "\"";
                        } else if constexpr (
                            std::is_same_v<T, std::vector<uint8_t>> ||
                            std::is_same_v<T, std::vector<int8_t>>) {
                            // Handle uint8_t/int8_t vectors specially to avoid character output
                            json_stream << "[";
                            bool first = true;
                            for (const auto& elem : value) {
                                if (!first)
                                    json_stream << ",";
                                first = false;
                                json_stream << static_cast<int>(elem);
                            }
                            json_stream << "]";
                        } else if constexpr (
                            std::is_same_v<T, std::vector<uint16_t>> ||
                            std::is_same_v<T, std::vector<uint32_t>> ||
                            std::is_same_v<T, std::vector<uint64_t>> ||
                            std::is_same_v<T, std::vector<int16_t>> ||
                            std::is_same_v<T, std::vector<int32_t>> ||
                            std::is_same_v<T, std::vector<int64_t>>) {
                            // Handle integer vector types
                            json_stream << "[";
                            bool first = true;
                            for (const auto& elem : value) {
                                if (!first)
                                    json_stream << ",";
                                first = false;
                                json_stream << elem;
                            }
                            json_stream << "]";
                        } else if constexpr (
                            std::is_same_v<T, std::vector<float>> ||
                            std::is_same_v<T, std::vector<double>>) {
                            // Handle float/double vector types with NaN check
                            json_stream << "[";
                            bool first = true;
                            for (const auto& elem : value) {
                                if (!first)
                                    json_stream << ",";
                                first = false;
                                if (!std::isfinite(elem)) {
                                    json_stream << "null";
                                } else {
                                    json_stream << elem;
                                }
                            }
                            json_stream << "]";
                        } else {
                            // Fallback for unknown types
                            json_stream << "null";
                        }
                    },
                    variant);
            } else {
                // Field not present or failed to extract
                json_stream << "null";
            }
        }
    }

    json_stream << "}";
    return json_stream.str();
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
