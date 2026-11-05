#pragma once

#include "mavlink_include.hpp"
#include "mavlink_signing.hpp"
#include "mavsdk_time.hpp"
#include <cstdint>

namespace mavsdk {

class MavlinkReceiver {
public:
    enum class ParseResult {
        MessageParsed, // Valid message ready for processing and forwarding
        UnknownMessage, // Not in our dialect, so no CRC check - forward and pass on to libmav
        BadCrc, // Corrupted message - forward raw bytes only, don't process
        BadSignature, // Signing is on and the message isn't accepted - drop
        NoneAvailable // No more messages in datagram
    };

    explicit MavlinkReceiver(MavlinkSigning& signing);

    mavlink_message_t& get_last_message() { return _last_message; }

    // Why the last message was rejected, if it was BadSignature.
    MavlinkSigning::Result get_last_signing_result() const { return _signing_result; }

    mavlink_status_t& get_status() { return _status; }

    void set_new_datagram(char* datagram, unsigned datagram_len);

    ParseResult parse_message();

    void debug_drop_rate();
    void print_line(
        const char* index,
        uint64_t count,
        uint64_t count_total,
        uint64_t overall_bytes,
        uint64_t overall_bytes_total);

private:
    MavlinkSigning& _signing;
    MavlinkSigning::Result _signing_result{MavlinkSigning::Result::Accepted};

    mavlink_message_t _last_message{};
    mavlink_status_t _status{};

    mavlink_message_t _mavlink_message_buffer{};
    mavlink_status_t _mavlink_status{};
    char* _datagram = nullptr;
    unsigned _datagram_len = 0;

    Time _time{};

    bool _drop_debugging_on{false};

    struct {
        uint64_t bytes_received{0};
        uint64_t bytes_sent_overall{0};
        uint64_t bytes_at_camera_overall{0};
        uint64_t bytes_at_sdk_overall{0};
        bool first{true};
        SteadyTimePoint last_time{};
        double time_elapsed{0.0};
    } _drop_stats{};
};

} // namespace mavsdk
