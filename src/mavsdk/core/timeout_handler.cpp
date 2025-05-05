#include "timeout_handler.h"
#include <algorithm>

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

    for (auto it = _timeouts.begin(); it != _timeouts.end();) {
        // If time is passed, execute the callback and remove the timeout
        if (it->time < now) {
            // Copy callback and cookie before executing the callback
            // in case the callback calls remove() on itself
            auto callback = it->callback;
            auto cookie = it->cookie;

            // Execute the callback
            if (callback) {
                callback();
            }

            // Check if the timeout still exists (it might have been removed by the callback)
            auto it_after_callback =
                std::find_if(_timeouts.begin(), _timeouts.end(), [&](auto& timeout) {
                    return timeout.cookie == cookie;
                });

            if (it_after_callback != _timeouts.end()) {
                // If it still exists, remove it
                it = _timeouts.erase(it_after_callback);
            } else {
                // If it was already removed by the callback, just continue
                it = it_after_callback;
            }
        } else {
            ++it;
        }
    }
}

} // namespace mavsdk
