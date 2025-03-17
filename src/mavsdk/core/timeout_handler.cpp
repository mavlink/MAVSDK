#include "timeout_handler.h"
#include <algorithm>

namespace mavsdk {

TimeoutHandler::TimeoutHandler(Time& time) : _time(time) {}

TimeoutHandler::Cookie TimeoutHandler::add(std::function<void()> callback, double duration_s)
{
    std::lock_guard<std::mutex> lock(_timeouts_mutex);
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
    std::lock_guard<std::mutex> lock(_timeouts_mutex);

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
    std::lock_guard<std::mutex> lock(_timeouts_mutex);

    auto it = std::find_if(_timeouts.begin(), _timeouts.end(), [&](auto& timeout) {
        return timeout.cookie == cookie;
    });

    if (it != _timeouts.end()) {
        _timeouts.erase(it);
    }
}

void TimeoutHandler::run_once()
{
    // First, collect expired timeouts under the lock
    std::vector<std::function<void()>> callbacks_to_run;

    {
        std::lock_guard<std::mutex> lock(_timeouts_mutex);
        auto now = _time.steady_time();

        for (auto it = _timeouts.begin(); it != _timeouts.end();) {
            // If time is passed, collect the callback
            if (it->time < now) {
                if (it->callback) {
                    callbacks_to_run.push_back(it->callback);
                }
                it = _timeouts.erase(it);
            } else {
                ++it;
            }
        }
    }

    // Then, execute callbacks outside the lock
    for (const auto& callback : callbacks_to_run) {
        callback();
    }
}

} // namespace mavsdk
