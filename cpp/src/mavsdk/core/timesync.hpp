#pragma once

#include "call_every_handler.hpp"
#include "callback_list.hpp"
#include "mavsdk_time.hpp"
#include "mavlink_include.hpp"
#include "system.hpp"

#include "rtt_estimator.hpp"

#include <array>
#include <atomic>
#include <chrono>
#include <mutex>

namespace mavsdk {

class SystemImpl;

class Timesync {
public:
    explicit Timesync(SystemImpl& parent);
    ~Timesync();

    // Only decides whether the measured offset is applied. Requests go out and the RTT is
    // measured regardless.
    void enable();

    // Called by SystemImpl at a fixed interval: sends a request while connected.
    void send_tick();

    // Timeout derived from the measured round-trip time, 0 until a request was answered.
    [[nodiscard]] double rtt_timeout_s() const { return _rtt_estimator.timeout_s(); }

    // Forget the measured RTT, e.g. because the link went down and may come back as another.
    void reset_rtt();

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

    void remember_request(int64_t ts1);
    void add_rtt_sample(int64_t ts1);

    static constexpr uint64_t MAX_CONS_HIGH_RTT = 5;
    static constexpr uint64_t MAX_RTT_SAMPLE_MS = 10;
    uint64_t _high_rtt_count{};
    bool _autopilot_timesync_acquired{false};
    std::atomic<bool> _is_enabled{false};

    // When our requests went out, on the steady clock. The answer echoes ts1, which is in
    // autopilot time and shifts whenever an offset is applied, so the RTT is measured here
    // instead. A few slots, so a slow answer is still matched after the next request.
    struct Request {
        int64_t ts1{0};
        std::chrono::steady_clock::time_point sent{};
    };
    static constexpr std::size_t NUM_REQUESTS = 4;
    std::mutex _requests_mutex{};
    std::array<Request, NUM_REQUESTS> _requests{};
    std::size_t _next_request{0};

    RttEstimator _rtt_estimator;

    mutable std::mutex _status_mutex{};
    System::TimesyncStatus _status{};
    CallbackList<System::TimesyncStatus> _status_callbacks;
};
} // namespace mavsdk
