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
    std::lock_guard<std::recursive_mutex> lock(_timeouts_mutex);
    auto now = _time.steady_time();

    // First, identify all timeouts that need to be executed
    std::vector<std::pair<Cookie, std::function<void()>>> callbacks_with_cookies;

    for (const auto& timeout : _timeouts) {
        if (timeout.time < now) {
            callbacks_with_cookies.push_back({timeout.cookie, timeout.callback});
        }
    }

    // Remove all timeouts that need to be executed
    for (const auto& [cookie, _] : callbacks_with_cookies) {
        remove(cookie);
    }

    // Now execute all callbacks
    for (const auto& [_, callback] : callbacks_with_cookies) {
        if (callback) {
            callback();
        }
    }
}

} // namespace mavsdk
