#include "libmav_receiver.h"
#include "embedded_mavlink_xml.h"
#include <mav/MessageSet.h>
#include <mav/BufferParser.h>
#include <json/json.h>
#include <variant>
#include <cstring>
#include <sstream>
#include "log.h"

namespace mavsdk {

LibmavReceiver::LibmavReceiver()
{
    // Initialize MessageSet with embedded XML content in dependency order
    _message_set = std::make_unique<mav::MessageSet>();
    _message_set->addFromXMLString(mav_embedded::MINIMAL_XML);
    _message_set->addFromXMLString(mav_embedded::STANDARD_XML);
    _message_set->addFromXMLString(mav_embedded::COMMON_XML);

    // Initialize BufferParser with our MessageSet
    _buffer_parser = std::make_unique<mav::BufferParser>(*_message_set);

    if (const char* env_p = std::getenv("MAVSDK_MAVLINK_DIRECT_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            _debugging = true;
        }
    }
}

LibmavReceiver::~LibmavReceiver() = default;

void LibmavReceiver::set_new_datagram(char* datagram, unsigned datagram_len)
{
    _datagram = datagram;
    _datagram_len = datagram_len;
}

bool LibmavReceiver::parse_message()
{
    if (!_datagram || _datagram_len == 0) {
        return false;
    }

    // Use libmav to parse messages directly from the buffer
    return parse_libmav_message_from_buffer(
        reinterpret_cast<const uint8_t*>(_datagram), _datagram_len);
}

bool LibmavReceiver::parse_libmav_message_from_buffer(const uint8_t* buffer, size_t buffer_len)
{
    size_t bytes_consumed = 0;
    auto message_opt = _buffer_parser->parseMessage(buffer, buffer_len, bytes_consumed);

    if (!message_opt) {
        return false; // No complete message found
    }

    auto message = message_opt.value();

    if (_debugging) {
        LogDebug() << "Parsed message: " << message.name() << " (ID: " << message.id() << ")";
    }

    // Extract system and component IDs from header
    auto header = message.header();

    // Generate complete JSON with all field values
    std::string json = libmav_message_to_json(message);

    // Fill our LibmavMessage structure
    _last_message.message = message;
    _last_message.message_name = message.name();
    _last_message.system_id = header.systemId();
    _last_message.component_id = header.componentId();

    // Extract target_system and target_component if present in message fields
    uint8_t target_system = 0;
    uint8_t target_component = 0;
    if (message.get("target_system", target_system) == mav::MessageResult::Success) {
        _last_message.target_system = target_system;
    } else {
        _last_message.target_system = 0;
    }
    if (message.get("target_component", target_component) == mav::MessageResult::Success) {
        _last_message.target_component = target_component;
    } else {
        _last_message.target_component = 0;
    }

    _last_message.fields_json = json;

    // Clear the original datagram since we processed it
    _datagram = nullptr;
    _datagram_len = 0;

    return true;
}

std::string LibmavReceiver::libmav_message_to_json(const mav::Message& msg) const
{
    std::ostringstream json_stream;
    json_stream << "{";
    json_stream << "\"message_id\":" << msg.id();
    json_stream << ",\"message_name\":\"" << msg.name() << "\"";

    // Get message definition to iterate through all fields
    auto message_def_opt = _message_set->getMessageDefinition(static_cast<int>(msg.id()));
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
                            json_stream << value;
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
                            std::is_same_v<T, std::vector<int64_t>> ||
                            std::is_same_v<T, std::vector<float>> ||
                            std::is_same_v<T, std::vector<double>>) {
                            // Handle other vector types
                            json_stream << "[";
                            bool first = true;
                            for (const auto& elem : value) {
                                if (!first)
                                    json_stream << ",";
                                first = false;
                                json_stream << elem;
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
    auto message_def = _message_set->getMessageDefinition(static_cast<int>(id));
    if (message_def) {
        return message_def.get().name();
    }
    return std::nullopt;
}

std::optional<int> LibmavReceiver::message_name_to_id(const std::string& name) const
{
    return _message_set->idForMessage(name);
}

std::optional<mav::Message> LibmavReceiver::create_message(const std::string& message_name) const
{
    return _message_set->create(message_name);
}

bool LibmavReceiver::load_custom_xml(const std::string& xml_content)
{
    if (!_message_set) {
        return false;
    }

    // Use libmav's addFromXMLString method to load custom XML
    auto result = _message_set->addFromXMLString(xml_content, false /* recursive_open_includes */);

    if (_debugging) {
        if (result == mav::MessageSetResult::Success) {
            LogDebug() << "Successfully loaded custom XML definitions";
        } else {
            LogDebug() << "Failed to load custom XML definitions";
        }
    }

    return result == mav::MessageSetResult::Success;
}

} // namespace mavsdk
