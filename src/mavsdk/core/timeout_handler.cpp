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

    _iterator_invalidated = true;

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
        _iterator_invalidated = true;
    }
}

void TimeoutHandler::run_once()
{
    std::unique_lock<std::mutex> lock(_timeouts_mutex);

    auto now = _time.steady_time();

    for (auto it = _timeouts.begin(); it != _timeouts.end(); /* no ++it */) {
        // If time is passed, call timeout callback.
        if (it->time < now) {
            // Get a copy for the callback because we will remove it.
            auto callback = it->callback;

            // Self-destruct before calling to avoid locking issues.
            it = _timeouts.erase(it);

            if (callback) {
                // Unlock while we call back because it might in turn want to add timeouts.
                lock.unlock();
                callback();
                lock.lock();

                // We start over if anyone has messed with this while we called the callback.
                if (_iterator_invalidated) {
                    _iterator_invalidated = false;
                    it = _timeouts.begin();
                }
            }
        } else {
            ++it;
        }
    }
}

} // namespace mavsdk
