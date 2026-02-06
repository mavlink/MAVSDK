#pragma once

#include "mavsdk_time.h"
#include "mavlink_include.h"

namespace mavsdk {

class SystemImpl;

class Timesync {
public:
    explicit Timesync(SystemImpl& parent);
    ~Timesync();

    void enable();
    void do_work();

    Timesync(const Timesync&) = delete;
    Timesync& operator=(const Timesync&) = delete;

private:
    SystemImpl& _system_impl;

    void process_timesync(const mavlink_message_t& message);
    void send_timesync(uint64_t tc1, uint64_t ts1);
    void set_timesync_offset(int64_t offset_ns, uint64_t start_transfer_local_time_ns);

    static constexpr double TIMESYNC_SEND_INTERVAL_S = 5.0;
    SteadyTimePoint _last_time{};

    static constexpr uint64_t MAX_CONS_HIGH_RTT = 5;
    static constexpr uint64_t MAX_RTT_SAMPLE_MS = 10;
    uint64_t _high_rtt_count{};
    bool _autopilot_timesync_acquired{false};
    bool _is_enabled{false};
};
} // namespace mavsdk
