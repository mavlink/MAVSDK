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
    new_timeout->start_time = steady_time();
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
        it->second->start_time = steady_time();
    }
}

void TimeoutHandler::update(double updated_duration_s, const void *cookie)
{
    std::lock_guard<std::mutex> lock(_timeouts_mutex);

    auto it = _timeouts.find((void *)(cookie));
    if (it != _timeouts.end()) {
        it->second->duration_s = updated_duration_s;
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


    for (auto it = _timeouts.begin(); it != _timeouts.end(); /* no ++it */) {

        // If time is passed, call timeout callback.
        if (elapsed_since_s(it->second->start_time) > it->second->duration_s) {

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
    }
    _timeouts_mutex.unlock();
}

} // namespace dronecore
