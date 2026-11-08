#pragma once

#include "call_every_handler.hpp"
#include "callback_list.hpp"
#include "mavsdk_time.hpp"
#include "mavlink_include.hpp"
#include "system.hpp"

#include <mutex>

namespace mavsdk {

class SystemImpl;

class Timesync {
public:
    explicit Timesync(SystemImpl& parent);
    ~Timesync();

    void enable();

    System::TimesyncStatus status() const;
    System::TimesyncStatusHandle subscribe_status(const System::TimesyncStatusCallback& callback);
    void unsubscribe_status(System::TimesyncStatusHandle handle);

    Timesync(const Timesync&) = delete;
    Timesync& operator=(const Timesync&) = delete;

private:
    SystemImpl& _system_impl;

    void process_timesync(const mavlink_message_t& message);
    void send_timesync(
        uint64_t tc1, uint64_t ts1, uint32_t target_system_id = 0, uint8_t target_component_id = 0);
    void set_timesync_offset(int64_t offset_ns, uint64_t start_transfer_local_time_ns);
    void update_status(bool synced, int64_t rtt_ns);

    void send_timesync_tick();

    static constexpr double TIMESYNC_SEND_INTERVAL_S = 5.0;
    CallEveryHandler::Cookie _send_cookie{};

    static constexpr uint64_t MAX_CONS_HIGH_RTT = 5;
    static constexpr uint64_t MAX_RTT_SAMPLE_MS = 10;
    uint64_t _high_rtt_count{};
    bool _autopilot_timesync_acquired{false};
    bool _is_enabled{false};

    mutable std::mutex _status_mutex{};
    System::TimesyncStatus _status{};
    CallbackList<System::TimesyncStatus> _status_callbacks;
};
} // namespace mavsdk
