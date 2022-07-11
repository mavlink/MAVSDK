#include "timeout_handler.h"

namespace mavsdk {

TimeoutHandler::TimeoutHandler(Time& time) : _time(time) {}

void TimeoutHandler::add(std::function<void()> callback, double duration_s, void** cookie)
{
    auto new_timeout = std::make_shared<Timeout>();
    new_timeout->callback = callback;
    new_timeout->time = _time.steady_time_in_future(duration_s);
    new_timeout->duration_s = duration_s;

    void* new_cookie = static_cast<void*>(new_timeout.get());

    {
        std::lock_guard<std::mutex> lock(_timeouts_mutex);
        _timeouts.insert(std::pair<void*, std::shared_ptr<Timeout>>(new_cookie, new_timeout));
    }

    if (cookie != nullptr) {
        *cookie = new_cookie;
    }
}

void TimeoutHandler::refresh(const void* cookie)
{
    if (cookie == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(_timeouts_mutex);

    auto it = _timeouts.find(const_cast<void*>(cookie));
    if (it != _timeouts.end()) {
        auto future_time = _time.steady_time_in_future(it->second->duration_s);
        it->second->time = future_time;
    }
}

void TimeoutHandler::remove(const void* cookie)
{
    if (cookie == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(_timeouts_mutex);

    auto it = _timeouts.find(const_cast<void*>(cookie));
    if (it != _timeouts.end()) {
        _timeouts.erase(const_cast<void*>(cookie));
        _iterator_invalidated = true;
    }
}

void TimeoutHandler::run_once()
{
    _timeouts_mutex.lock();

    auto now = _time.steady_time();

    for (auto it = _timeouts.begin(); it != _timeouts.end(); /* no ++it */) {
        // If time is passed, call timeout callback.
        if (it->second->time < now) {
            if (it->second->callback) {
                // Get a copy for the callback because we will remove it.
                std::function<void()> callback = it->second->callback;

                // Self-destruct before calling to avoid locking issues.
                _timeouts.erase(it++);

                // Unlock while we callback because it might in turn want to add timeouts.
                _timeouts_mutex.unlock();
                callback();
                _timeouts_mutex.lock();
            }

        } else {
            ++it;
        }

        // We start over if anyone has messed with this while we called the callback.
        if (_iterator_invalidated) {
            _iterator_invalidated = false;
            it = _timeouts.begin();
        }
    }
    _timeouts_mutex.unlock();
}

} // namespace mavsdk
