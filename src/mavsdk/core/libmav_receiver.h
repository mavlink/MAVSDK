#pragma once

#include <mav/Message.h>
#include <string>
#include <cstdint>
#include <optional>
#include <memory>
#include "mavlink_include.h"

// Forward declarations to avoid including MessageSet.h in header
namespace mav {
class MessageSet;
class BufferParser;
} // namespace mav

namespace Json {
class Value;
}

namespace mavsdk {

struct LibmavMessage {
    std::optional<mav::Message>
        message; // The libmav message object (optional since no default constructor)
    std::string message_name; // Message name (e.g., "HEARTBEAT")
    uint32_t system_id; // Source system ID
    uint32_t component_id; // Source component ID
    uint32_t target_system; // Target system ID (if applicable)
    uint32_t target_component; // Target component ID (if applicable)
    std::string fields_json; // Pre-converted JSON fields
};

// Forward declaration for thread-safe MessageSet operations
class MavsdkImpl;

class LibmavReceiver {
public:
    explicit LibmavReceiver(MavsdkImpl& mavsdk_impl);
    ~LibmavReceiver(); // Need explicit destructor for unique_ptr with incomplete type

    const LibmavMessage& get_last_message() const { return _last_message; }

    void set_new_datagram(char* datagram, unsigned datagram_len);

    bool parse_message();

    // Message creation for sending
    std::optional<mav::Message> create_message(const std::string& message_name) const;

    // Helper methods for message ID/name conversion
    std::optional<std::string> message_id_to_name(uint32_t id) const;
    std::optional<int> message_name_to_id(const std::string& name) const;

    // Load custom XML message definitions
    bool load_custom_xml(const std::string& xml_content);

private:
    MavsdkImpl& _mavsdk_impl; // For thread-safe MessageSet access
    std::unique_ptr<mav::BufferParser> _buffer_parser;
    LibmavMessage _last_message;

    char* _datagram = nullptr;
    unsigned _datagram_len = 0;

    bool _debugging = false;

    // Helper methods
    std::string libmav_message_to_json(const mav::Message& msg) const;
    bool parse_libmav_message_from_buffer(const uint8_t* buffer, size_t buffer_len);
};

} // namespace mavsdk