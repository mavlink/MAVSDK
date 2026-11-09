#pragma once

#include "mavlink_include.hpp"
#include "rtt_estimator.hpp"
#include <atomic>

namespace mavsdk {

class SystemImpl;

class Ping {
public:
    explicit Ping(SystemImpl& system_impl);
    ~Ping();

    void run_once();
    [[nodiscard]] double last_ping_time_s() const
    {
        return static_cast<double>(_last_ping_time_us) * 1e-6;
    }

    // Timeout derived from the measured round-trip time, 0 until a ping was answered.
    [[nodiscard]] double rtt_timeout_s() const { return _rtt_estimator.timeout_s(); }

    // Forget the measured RTT, e.g. because the link went down and may come back as another.
    void reset() { _rtt_estimator.reset(); }

private:
    void process_ping(const mavlink_message_t& message);

    SystemImpl& _system_impl;
    uint32_t _ping_sequence{0};
    std::atomic<uint64_t> _last_ping_time_us{0};
    RttEstimator _rtt_estimator;
};

} // namespace mavsdk
