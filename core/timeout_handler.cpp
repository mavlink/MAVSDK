#include "timeout_handler.h"

namespace dronecore {

TimeoutHandler::TimeoutHandler()
{
}

TimeoutHandler::~TimeoutHandler()
{
}

void TimeoutHandler::add(std::function<void()> callback, double duration_s, void **cookie)
{
    auto new_timeout = std::make_shared<Timeout>();
    new_timeout->callback = callback;
    new_timeout->time = steady_time_in_future(duration_s);
    new_timeout->duration_s = duration_s;

    void *new_cookie = static_cast<void *>(new_timeout.get());

    {
        std::lock_guard<std::mutex> lock(_timeouts_mutex);
        _timeouts.insert(std::pair<void *, std::shared_ptr<Timeout>>(new_cookie, new_timeout));
    }

    if (cookie != nullptr) {
        *cookie = new_cookie;
    }
}

void TimeoutHandler::refresh(const void *cookie)
{
    std::lock_guard<std::mutex> lock(_timeouts_mutex);

    auto it = _timeouts.find((void *)(cookie));
    if (it != _timeouts.end()) {
        dl_time_t future_time = steady_time_in_future(it->second->duration_s);
        it->second->time = future_time;
    }
}

void TimeoutHandler::remove(const void *cookie)
{
    std::lock_guard<std::mutex> lock(_timeouts_mutex);

    auto it = _timeouts.find((void *)cookie);
    if (it != _timeouts.end()) {
        _timeouts.erase((void *)cookie);
    }
}

void TimeoutHandler::run_once()
{
    _timeouts_mutex.lock();

    dl_time_t now = steady_time();

    for (auto it = _timeouts.begin(); it != _timeouts.end(); /* no ++it */) {

        // If time is passed, call timeout callback.
        if (it->second->time < now) {

            if (it->second->callback) {

                // Unlock while we callback because it might in turn want to add timeouts.
                std::function<void()> callback = it->second->callback;
                _timeouts_mutex.unlock();
                callback();
                _timeouts_mutex.lock();
            }

            // Self-destruct before calling to avoid locking issues.
            _timeouts.erase(it++);

        } else {
            ++it;
        }
    }
    _timeouts_mutex.unlock();
}

} // namespace dronecore
