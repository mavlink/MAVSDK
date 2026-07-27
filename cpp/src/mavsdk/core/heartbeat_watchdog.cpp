#include "heartbeat_watchdog.hpp"

#include <cmath>

#include "log.hpp"

namespace mavsdk {

bool HeartbeatWatchdog::is_valid_timeout_s(double timeout_s)
{
    return timeout_s == 0.0 || (std::isfinite(timeout_s) && timeout_s >= min_timeout_s);
}

HeartbeatWatchdog::HeartbeatWatchdog(Time& time) : _time(time) {}

bool HeartbeatWatchdog::set_timeout_s(double timeout_s)
{
    if (!is_valid_timeout_s(timeout_s)) {
        LogWarn(
            "Invalid heartbeat watchdog timeout: {} s (must be 0 or >= {} s)",
            timeout_s,
            min_timeout_s);
        return false;
    }

    std::lock_guard<std::mutex> lock(_mutex);

    if (timeout_s == _timeout_s) {
        return true;
    }

    _timeout_s = timeout_s;
    // Enabling the watchdog or changing the timeout: do not allow heartbeats
    // until fed. Disabling it: heartbeats follow the usual policy again.
    _state = (timeout_s > 0.0) ? State::NeedsFeed : State::Disabled;
    _deadline.reset();

    return true;
}

double HeartbeatWatchdog::timeout_s() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _timeout_s;
}

void HeartbeatWatchdog::feed()
{
    std::lock_guard<std::mutex> lock(_mutex);

    switch (_state) {
        case State::Disabled:
            return;
        case State::NeedsFeed:
        case State::Armed:
            // A feed is valid only until this deadline.
            _state = State::Armed;
            _deadline = _time.steady_time_in_future(_timeout_s);
            return;
    }
}

bool HeartbeatWatchdog::allows_sending()
{
    std::lock_guard<std::mutex> lock(_mutex);

    switch (_state) {
        case State::Disabled:
            return true;
        case State::NeedsFeed:
            return false;
        case State::Armed:
            if (_time.steady_time() >= *_deadline) {
                LogWarn("Heartbeat watchdog expired, stopping heartbeats");
                _state = State::NeedsFeed;
                _deadline.reset();
                return false;
            }
            return true;
    }

    return false;
}

} // namespace mavsdk
