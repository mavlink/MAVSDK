#include "call_every_handler.hpp"

#include <utility>
#include <algorithm>

namespace mavsdk {

CallEveryHandler::CallEveryHandler(Time& time) : _time(time) {}

CallEveryHandler::Cookie CallEveryHandler::add(std::function<void()> callback, double interval_s)
{
    auto new_entry = Entry{};
    new_entry.callback = std::move(callback);
    auto before = _time.steady_time();
    // Make sure it gets run straightaway. The epsilon seemed not enough, so
    // we use the arbitrary value of 1 ms.
    _time.shift_steady_time_by(before, -interval_s - 0.001);
    new_entry.last_time = before;
    new_entry.interval_s = interval_s;

    Cookie cookie;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        new_entry.cookie = _next_cookie++;
        _entries.push_back(new_entry);
        cookie = new_entry.cookie;
    }

    // last_time was deliberately shifted into the past above, so this entry is due right
    // away and the owner's timer needs to fire now rather than at its current deadline.
    if (_wakeup_callback) {
        _wakeup_callback();
    }

    return cookie;
}

void CallEveryHandler::change(double interval_s, Cookie cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
        return entry.cookie == cookie;
    });
    if (it != _entries.end()) {
        it->interval_s = interval_s;
    }
}

void CallEveryHandler::reset(Cookie cookie)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
        return entry.cookie == cookie;
    });
    if (it != _entries.end()) {
        it->last_time = _time.steady_time();
    }
}

void CallEveryHandler::call_soon(Cookie cookie)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
            return entry.cookie == cookie;
        });
        if (it == _entries.end()) {
            return;
        }
        // Same shift as add() uses to make a new entry run straightaway.
        it->last_time = _time.steady_time();
        _time.shift_steady_time_by(it->last_time, -it->interval_s - 0.001);
    }

    if (_wakeup_callback) {
        _wakeup_callback();
    }
}

std::optional<SteadyTimePoint> CallEveryHandler::next_deadline()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return earliest_with_lock();
}

std::optional<SteadyTimePoint> CallEveryHandler::earliest_with_lock() const
{
    std::optional<SteadyTimePoint> earliest;
    for (const auto& entry : _entries) {
        auto due = entry.last_time;
        _time.shift_steady_time_by(due, entry.interval_s);
        if (!earliest || due < *earliest) {
            earliest = due;
        }
    }
    return earliest;
}

void CallEveryHandler::set_wakeup_callback(std::function<void()> callback)
{
    _wakeup_callback = std::move(callback);
}

void CallEveryHandler::remove(Cookie cookie)
{
    remove_impl(cookie, false);
}

void CallEveryHandler::remove_blocking(Cookie cookie)
{
    remove_impl(cookie, true);
}

void CallEveryHandler::remove_impl(Cookie cookie, bool blocking)
{
    // Cookie 0 is never handed out by add(), so it means "nothing registered" --
    // callers keep a default-initialised cookie around for a timer they may never
    // have started. Bail out before the wait below, which uses 0 for "not executing"
    // and would otherwise never be satisfied.
    if (cookie == 0) {
        return;
    }

    std::unique_lock<std::mutex> lock(_mutex);

    _entries.remove_if([&](const Entry& entry) { return entry.cookie == cookie; });
    // Also drop it if run_once() has already found it due but has not gotten around to
    // invoking it yet -- otherwise removing an entry would not actually stop it from firing.
    _due.remove(cookie);

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

void CallEveryHandler::run_once()
{
    std::unique_lock<std::mutex> lock(_mutex);

    for (auto& entry : _entries) {
        if (_time.elapsed_since_s(entry.last_time) > double(entry.interval_s)) {
            // Update the timestamp before potentially executing
            _time.shift_steady_time_by(entry.last_time, double(entry.interval_s));

            if (entry.callback) {
                _due.push_back(entry.cookie);
            }
        }
    }

    // Note that the entries are looked up again by cookie right before they are invoked, so
    // that a callback removing another due entry actually prevents it from running.
    while (!_due.empty()) {
        const auto cookie = _due.front();
        _due.pop_front();

        auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
            return entry.cookie == cookie;
        });
        if (it == _entries.end() || !it->callback) {
            continue;
        }

        auto callback = it->callback;

        _executing_cookie = cookie;
        _executing_thread = std::this_thread::get_id();

        lock.unlock();
        callback();
        // Drop whatever the callback captured while we are not holding the lock, in case
        // its destructor calls back into us.
        callback = nullptr;
        lock.lock();

        _executing_cookie = 0;
        _executing_thread = {};
        _callback_done.notify_all();
    }
}

} // namespace mavsdk
