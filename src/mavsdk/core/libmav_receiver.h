#pragma once

#include <mav/Message.h>
#include <string>
#include <cstdint>
#include <optional>
#include <memory>
#include <ostream>
#include <vector>
#include "mavlink_include.h"
#include "mavsdk.h"

// Forward declarations to avoid including MessageSet.h in header
namespace mav {
class MessageSet;
class BufferParser;
} // namespace mav

namespace Json {
class Value;
}

namespace mavsdk {

// Forward declaration for thread-safe MessageSet operations
class MavsdkImpl;

// Original implementation using std::ostream << for float/double conversion.
template<typename T> void value_to_json_stream(std::ostream& json_stream, const T& value);

// Optimized implementation using std::to_chars for float/double conversion.
template<typename T> void value_to_json_stream_fast(std::ostream& json_stream, const T& value);

// Explicit template declarations - original
extern template void value_to_json_stream<float>(std::ostream&, const float&);
extern template void value_to_json_stream<double>(std::ostream&, const double&);
extern template void value_to_json_stream<int32_t>(std::ostream&, const int32_t&);
extern template void value_to_json_stream<uint32_t>(std::ostream&, const uint32_t&);
extern template void
value_to_json_stream<std::vector<float>>(std::ostream&, const std::vector<float>&);
extern template void
value_to_json_stream<std::vector<double>>(std::ostream&, const std::vector<double>&);

// Explicit template declarations - optimized
extern template void value_to_json_stream_fast<float>(std::ostream&, const float&);
extern template void value_to_json_stream_fast<double>(std::ostream&, const double&);
extern template void value_to_json_stream_fast<int32_t>(std::ostream&, const int32_t&);
extern template void value_to_json_stream_fast<uint32_t>(std::ostream&, const uint32_t&);
extern template void
value_to_json_stream_fast<std::vector<float>>(std::ostream&, const std::vector<float>&);
extern template void
value_to_json_stream_fast<std::vector<double>>(std::ostream&, const std::vector<double>&);

class LibmavReceiver {
public:
    explicit LibmavReceiver(MavsdkImpl& mavsdk_impl);
    ~LibmavReceiver(); // Need explicit destructor for unique_ptr with incomplete type

    const Mavsdk::MavlinkMessage& get_last_message() const { return _last_message; }
    const std::optional<mav::Message>& get_last_libmav_message() const
    {
        return _last_libmav_message;
    }

    void set_new_datagram(char* datagram, unsigned datagram_len);

    bool parse_message();

    // Message creation for sending
    std::optional<mav::Message> create_message(const std::string& message_name) const;

    // Helper methods for message ID/name conversion
    std::optional<std::string> message_id_to_name(uint32_t id) const;
    std::optional<int> message_name_to_id(const std::string& name) const;

    // Load custom XML message definitions
    bool load_custom_xml(const std::string& xml_content);

    // JSON conversion (made public for use in message interception)
    std::string libmav_message_to_json(const mav::Message& msg) const;

private:
    MavsdkImpl& _mavsdk_impl; // For thread-safe MessageSet access
    std::unique_ptr<mav::BufferParser> _buffer_parser;
    Mavsdk::MavlinkMessage _last_message;
    std::optional<mav::Message> _last_libmav_message; // Separate libmav message for integration

    char* _datagram = nullptr;
    unsigned _datagram_len = 0;

    bool _debugging = false;

    // Helper methods
    bool parse_libmav_message_from_buffer(const uint8_t* buffer, size_t buffer_len);
};

} // namespace mavsdk
