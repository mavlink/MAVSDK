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
#include <optional>

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
    void set_timesync_offset(int64_t offset_ns, int64_t rtt_ns);
    void update_status(bool synced, int64_t rtt_ns);

    void send_request();
    void remember_request(int64_t ts1, uint32_t clock_shifts);

    // When our requests went out, on the steady clock. The answer echoes ts1, which is in
    // autopilot time and shifts whenever an offset is applied, so the RTT is measured here
    // instead. A few slots, so a slow answer is still matched after the next request.
    struct Request {
        int64_t ts1{0};
        std::chrono::steady_clock::time_point sent{};
        // Number of offsets applied before ts1 was taken. An answer to a request from before
        // the last shift has ts1 on the old clock and can't give an offset.
        uint32_t clock_shifts{0};
    };
    // Finds the request this answer is for. Only consumed if `consume`, so a duplicate or a
    // second answer doesn't match it again.
    std::optional<Request> find_request(int64_t ts1, bool consume);

    static constexpr uint64_t MAX_CONS_HIGH_RTT = 5;
    static constexpr uint64_t MAX_RTT_SAMPLE_MS = 10;
    uint64_t _high_rtt_count{};
    bool _autopilot_timesync_acquired{false};
    std::atomic<bool> _is_enabled{false};
    std::atomic<uint32_t> _clock_shifts{0};

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
