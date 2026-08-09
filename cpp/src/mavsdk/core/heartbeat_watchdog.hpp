#pragma once

#include <mutex>
#include <optional>

#include "mavsdk_export.h"
#include "mavsdk_time.hpp"

namespace mavsdk {

/*
 * Deadman timer for the periodic heartbeats that MAVSDK sends.
 *
 * When enabled, heartbeats are only allowed while feed() keeps being called at
 * least once per timeout period. A feed is valid only until its deadline, which
 * includes across system disconnect/reconnect and across periods where
 * heartbeats are not being sent for other reasons.
 *
 *   Disabled  --(timeout > 0)-->  NeedsFeed
 *   NeedsFeed --(feed)--------->  Armed      (start deadline)
 *   Armed     --(feed)--------->  Armed      (extend deadline)
 *   Armed     --(expired)------>  NeedsFeed
 *   any       --(timeout = 0)-->  Disabled
 *
 * Enabling the watchdog, or changing its timeout, always requires a fresh feed
 * before heartbeats are allowed again: it never grants a free timeout period.
 *
 * All methods are safe to call from any thread. The internal mutex is a leaf:
 * nothing is acquired while holding it, so it may be taken under another lock.
 */
class MAVSDK_TEST_EXPORT HeartbeatWatchdog {
public:
    // Minimum timeout when enabled, in seconds.
    //
    // Two heartbeat periods rather than one: heartbeats are sent at 1 Hz and
    // the deadline is only checked when one is due, so with a 1 s timeout a
    // feed and the tick can land almost in phase and a single slightly late
    // feed already drops a heartbeat. 1 s is therefore not usable as a
    // deadman timeout, however well documented.
    static constexpr double min_timeout_s = 2.0;

    // A timeout is valid if it is 0 (disabled), or finite and at least
    // min_timeout_s.
    static bool is_valid_timeout_s(double timeout_s);

    explicit HeartbeatWatchdog(Time& time);

    // Returns false and keeps the previous timeout if timeout_s is invalid.
    // Setting the same timeout again is a no-op and does not re-arm.
    bool set_timeout_s(double timeout_s);
    double timeout_s() const;

    // Resets the deadline. Has no effect while the watchdog is disabled.
    void feed();

    // Called when a heartbeat is due. Reports whether the watchdog allows it,
    // expiring an armed deadline that has passed. Note this is not a pure
    // query: it is what drives the Armed -> NeedsFeed transition.
    bool allows_sending();

private:
    enum class State {
        Disabled,
        NeedsFeed,
        Armed,
    };

    Time& _time;
    mutable std::mutex _mutex{};
    State _state{State::Disabled};
    double _timeout_s{0.0};
    // Set if and only if _state is Armed.
    std::optional<SteadyTimePoint> _deadline{};
};

} // namespace mavsdk
