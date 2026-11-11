#pragma once

#include "mavlink_include.hpp"

namespace mavsdk {

class SystemImpl;

// Answers PING requests. MAVSDK itself measures the round-trip time with TIMESYNC, which
// supersedes PING, so it does not send any.
class Ping {
public:
    explicit Ping(SystemImpl& system_impl);
    ~Ping();

private:
    void process_ping(const mavlink_message_t& message);

    SystemImpl& _system_impl;
};

} // namespace mavsdk
