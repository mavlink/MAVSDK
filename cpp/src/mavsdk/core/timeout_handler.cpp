#include "timeout_handler.hpp"
#include <algorithm>

namespace mavsdk {

TimeoutHandler::TimeoutHandler(Time& time) : _time(time) {}

TimeoutHandler::Cookie TimeoutHandler::add(std::function<void()> callback, double duration_s)
{
    Cookie cookie;
    bool moved_earlier;

    {
        std::lock_guard<std::mutex> lock(_timeouts_mutex);

        const auto previous_earliest = earliest_with_lock();

        auto new_timeout = Timeout{};
        new_timeout.callback = std::move(callback);
        new_timeout.time = _time.steady_time_in_future(duration_s);
        new_timeout.duration_s = duration_s;
        new_timeout.cookie = _next_cookie++;
        _timeouts.push_back(new_timeout);

        cookie = new_timeout.cookie;
        moved_earlier = !previous_earliest || new_timeout.time < *previous_earliest;
    }

    if (moved_earlier && _wakeup_callback) {
        _wakeup_callback();
    }

    return cookie;
}

void TimeoutHandler::refresh(Cookie cookie)
{
    bool moved_earlier = false;

    {
        std::lock_guard<std::mutex> lock(_timeouts_mutex);

        auto it = std::find_if(_timeouts.begin(), _timeouts.end(), [&](const Timeout& timeout) {
            return timeout.cookie == cookie;
        });
        if (it == _timeouts.end()) {
            return;
        }

        const auto previous_earliest = earliest_with_lock();
        it->time = _time.steady_time_in_future(it->duration_s);
        moved_earlier = !previous_earliest || it->time < *previous_earliest;
    }

    if (moved_earlier && _wakeup_callback) {
        _wakeup_callback();
    }
}

std::optional<SteadyTimePoint> TimeoutHandler::next_deadline()
{
    std::lock_guard<std::mutex> lock(_timeouts_mutex);
    return earliest_with_lock();
}

std::optional<SteadyTimePoint> TimeoutHandler::earliest_with_lock() const
{
    std::optional<SteadyTimePoint> earliest;
    for (const auto& timeout : _timeouts) {
        if (!earliest || timeout.time < *earliest) {
            earliest = timeout.time;
        }
    }
    return earliest;
}

void TimeoutHandler::set_wakeup_callback(std::function<void()> callback)
{
    _wakeup_callback = std::move(callback);
}

void TimeoutHandler::remove(Cookie cookie)
{
    remove_impl(cookie, false);
}

void TimeoutHandler::remove_blocking(Cookie cookie)
{
    remove_impl(cookie, true);
}

void TimeoutHandler::remove_impl(Cookie cookie, bool blocking)
{
    // Cookie 0 is never handed out by add(), so it means "nothing registered" --
    // callers keep a default-initialised cookie around for a timer they may never
    // have started. Bail out before the wait below, which uses 0 for "not executing"
    // and would otherwise never be satisfied.
    if (cookie == 0) {
        return;
    }

    std::unique_lock<std::mutex> lock(_timeouts_mutex);

    const auto matches = [&](const Timeout& timeout) { return timeout.cookie == cookie; };

    _timeouts.remove_if(matches);
    // Also drop it if run_once() has already taken it out of _timeouts as due but has not
    // gotten around to invoking it yet -- otherwise removing a timeout would not actually
    // stop it from firing.
    _due.remove_if(matches);

    if (!blocking) {
        return;
    }

    // The callback is running right now: wait for it to return so that the caller can
    // destroy whatever it captured. Unless we are being called from that very callback, in
    // which case waiting would deadlock on ourselves.
    _callback_done.wait(lock, [&]() {
        return _executing_cookie != cookie || _executing_thread == std::this_thread::get_id();
    });
}

void TimeoutHandler::run_once()
{
    std::unique_lock<std::mutex> lock(_timeouts_mutex);

    const auto now = _time.steady_time();

    // Move the due timeouts over to _due in one go. They are one-shot, so they leave
    // _timeouts here either way, and parking them in _due (instead of a local vector)
    // keeps them cancellable by remove() until the moment they are invoked.
    for (auto it = _timeouts.begin(); it != _timeouts.end();) {
        if (it->time < now) {
            auto due_it = it++;
            _due.splice(_due.end(), _timeouts, due_it);
        } else {
            ++it;
        }
    }

    while (!_due.empty()) {
        auto timeout = std::move(_due.front());
        _due.pop_front();

        if (!timeout.callback) {
            continue;
        }

        _executing_cookie = timeout.cookie;
        _executing_thread = std::this_thread::get_id();

        lock.unlock();
        timeout.callback();
        // Drop whatever the callback captured while we are not holding the lock, in case
        // its destructor calls back into us.
        timeout.callback = nullptr;
        lock.lock();

        _executing_cookie = 0;
        _executing_thread = {};
        _callback_done.notify_all();
    }
}

} // namespace mavsdk
