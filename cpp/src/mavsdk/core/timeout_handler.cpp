#include "timeout_handler.h"
#include <algorithm>
#include <vector>

namespace mavsdk {

TimeoutHandler::TimeoutHandler(Time& time) : _time(time) {}

TimeoutHandler::Cookie TimeoutHandler::add(std::function<void()> callback, double duration_s)
{
    std::lock_guard<std::recursive_mutex> lock(_timeouts_mutex);
    auto new_timeout = Timeout{};
    new_timeout.callback = std::move(callback);
    new_timeout.time = _time.steady_time_in_future(duration_s);
    new_timeout.duration_s = duration_s;
    new_timeout.cookie = _next_cookie++;
    _timeouts.push_back(new_timeout);

    return new_timeout.cookie;
}

void TimeoutHandler::refresh(Cookie cookie)
{
    std::lock_guard<std::recursive_mutex> lock(_timeouts_mutex);

    auto it = std::find_if(_timeouts.begin(), _timeouts.end(), [&](const Timeout& timeout) {
        return timeout.cookie == cookie;
    });
    if (it != _timeouts.end()) {
        auto future_time = _time.steady_time_in_future(it->duration_s);
        it->time = future_time;
    }
}

void TimeoutHandler::remove(Cookie cookie)
{
    std::lock_guard<std::recursive_mutex> lock(_timeouts_mutex);

    auto it = std::find_if(_timeouts.begin(), _timeouts.end(), [&](auto& timeout) {
        return timeout.cookie == cookie;
    });

    if (it != _timeouts.end()) {
        _timeouts.erase(it);
    }
}

void TimeoutHandler::run_once()
{
    // First, identify all timeouts that need to be executed and remove them
    // while holding the lock
    std::vector<std::function<void()>> callbacks_to_execute;

    {
        std::lock_guard<std::recursive_mutex> lock(_timeouts_mutex);
        auto now = _time.steady_time();

        // Identify timeouts that need to be executed
        std::vector<Cookie> cookies_to_remove;

        for (const auto& timeout : _timeouts) {
            if (timeout.time < now) {
                if (timeout.callback) {
                    callbacks_to_execute.push_back(timeout.callback);
                }
                cookies_to_remove.push_back(timeout.cookie);
            }
        }

        // Remove all timeouts that need to be executed
        for (const auto& cookie : cookies_to_remove) {
            auto it = std::find_if(_timeouts.begin(), _timeouts.end(), [&](auto& timeout) {
                return timeout.cookie == cookie;
            });

            if (it != _timeouts.end()) {
                _timeouts.erase(it);
            }
        }
    }

    // Now execute all callbacks outside the lock to prevent lock-order inversions
    for (const auto& callback : callbacks_to_execute) {
        callback();
    }
}

} // namespace mavsdk
