#pragma once

#include "global_include.h"
#include "mavlink_include.h"

namespace mavsdk {

class SystemImpl;

class Timesync {
public:
    Timesync(SystemImpl& parent);
    ~Timesync();

    void do_work();

    Timesync(const Timesync&) = delete;
    Timesync& operator=(const Timesync&) = delete;

private:
    SystemImpl& _parent;

    void process_timesync(const mavlink_message_t& message);
    void send_timesync(uint64_t tc1, uint64_t ts1);
    void set_timesync_offset(int64_t offset_ns, uint64_t start_transfer_local_time_ns);

    static constexpr double _TIMESYNC_SEND_INTERVAL_S = 5.0;
    dl_time_t _last_time{};

    static constexpr uint64_t _MAX_CONS_HIGH_RTT = 5;
    static constexpr uint64_t _MAX_RTT_SAMPLE_MS = 10;
    uint64_t _high_rtt_count{};
    bool _autopilot_timesync_acquired = false;
};
} // namespace mavsdk
