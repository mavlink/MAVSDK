#include "libmav_receiver.h"
#include "embedded_mavlink_xml.h"
#include <mav/MessageSet.h>
#include <json/json.h>
#include <variant>
#include <cstring>
#include <sstream>
#include "log.h"

namespace mavsdk {

LibmavReceiver::LibmavReceiver()
{
    // Initialize MessageSet with embedded common.xml content
    _message_set = std::make_unique<mav::MessageSet>();
    _message_set->addFromXMLString(mav_embedded::COMMON_XML);

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
    // For now, use the same manual parsing approach but enhanced for all message types
    // TODO: Implement proper libmav parsing once we understand the correct API
    for (size_t i = 0; i < buffer_len; ++i) {
        uint8_t byte = buffer[i];

        // Look for MAVLink v2 start byte
        if (byte == 0xFD) {
            // Check if we have enough bytes for a minimal header (10 bytes)
            if (i + 10 > buffer_len) {
                break; // Not enough data for complete message
            }

            // Extract payload length from header
            uint8_t payload_len = buffer[i + 1];

            // Calculate total message length: header(10) + payload + checksum(2)
            size_t total_msg_len = 10 + payload_len + 2;

            if (i + total_msg_len > buffer_len) {
                break; // Not enough data for complete message
            }

            // Extract message ID from header (24-bit field at bytes 7-9)
            uint32_t message_id = (static_cast<uint32_t>(buffer[i + 7]) << 0) |
                                  (static_cast<uint32_t>(buffer[i + 8]) << 8) |
                                  (static_cast<uint32_t>(buffer[i + 9]) << 16);

            if (_debugging) {
                LogDebug() << "Parsing message with ID: " << message_id;
            }

            // Get message name from ID
            auto message_name_opt = message_id_to_name(message_id);
            if (!message_name_opt) {
                LogWarn() << "Unknown message ID: " << message_id;
                continue; // Unknown message type
            }

            if (_debugging) {
                LogDebug() << "Parsed message: " << message_name_opt.value()
                           << " (ID: " << message_id << ")";
            }

            // For now, generate basic JSON with message info
            // TODO: Implement proper field extraction using libmav
            std::ostringstream json_stream;
            json_stream << "{";
            json_stream << "\"message_id\":" << message_id;
            json_stream << ",\"message_name\":\"" << message_name_opt.value() << "\"";
            json_stream << "}";

            // Fill our LibmavMessage structure
            _last_message.message = std::nullopt; // No libmav message for now
            _last_message.message_name = message_name_opt.value();
            _last_message.system_id = buffer[i + 5];
            _last_message.component_id = buffer[i + 6];
            _last_message.target_system = 0; // TODO: Extract from payload if present
            _last_message.target_component = 0; // TODO: Extract from payload if present
            _last_message.fields_json = json_stream.str();

            // Clear the original datagram since we processed it
            _datagram = nullptr;
            _datagram_len = 0;

            return true;
        }
    }

    return false;
}

std::string LibmavReceiver::libmav_message_to_json(const mav::Message& msg) const
{
    // This is a placeholder - for now we don't use this method
    // TODO: Implement proper libmav field extraction once API is understood
    std::ostringstream json_stream;
    json_stream << "{";
    json_stream << "\"message_id\":" << msg.id();
    json_stream << ",\"message_name\":\"" << msg.name() << "\"";
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

bool LibmavReceiver::libmav_to_mavlink_message(
    const mav::Message& libmav_msg, mavlink_message_t& mavlink_msg) const
{
    // Get the raw message data from the finalized libmav message
    auto message_data = libmav_msg.data();

    // Parse the raw MAVLink data into mavlink_message_t structure
    mavlink_status_t status;
    bool message_parsed = false;

    // Parse the message header and payload from the raw data
    // libmav data includes full MAVLink packet: header + payload + checksum + signature
    // Parse byte by byte until we get a complete message or reach max size
    for (size_t i = 0; i < 280; ++i) { // 280 is roughly max MAVLink message size
        if (mavlink_parse_char(MAVLINK_COMM_0, message_data[i], &mavlink_msg, &status)) {
            message_parsed = true;
            break;
        }
    }

    return message_parsed;
}

} // namespace mavsdk