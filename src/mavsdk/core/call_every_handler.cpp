#include "call_every_handler.h"

#include <utility>
#include <algorithm>

namespace mavsdk {

CallEveryHandler::CallEveryHandler(Time& time) : _time(time) {}

CallEveryHandler::Cookie CallEveryHandler::add(std::function<void()> callback, double interval_s)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto new_entry = Entry{};
    new_entry.callback = std::move(callback);
    auto before = _time.steady_time();
    // Make sure it gets run straightaway. The epsilon seemed not enough, so
    // we use the arbitrary value of 1 ms.
    _time.shift_steady_time_by(before, -interval_s - 0.001);
    new_entry.last_time = before;
    new_entry.interval_s = interval_s;
    new_entry.cookie = _next_cookie++;
    _entries.push_back(new_entry);

    _iterator_invalidated = true;

    return new_entry.cookie;
}

void CallEveryHandler::change(double interval_s, Cookie cookie)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
        return entry.cookie == cookie;
    });
    if (it != _entries.end()) {
        it->interval_s = interval_s;
    }
}

void CallEveryHandler::reset(Cookie cookie)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
        return entry.cookie == cookie;
    });
    if (it != _entries.end()) {
        it->last_time = _time.steady_time();
    }
}

void CallEveryHandler::remove(Cookie cookie)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto it = std::find_if(
        _entries.begin(), _entries.end(), [&](auto& timeout) { return timeout.cookie == cookie; });

    if (it != _entries.end()) {
        _entries.erase(it);
        _iterator_invalidated = true;
    }
}

void CallEveryHandler::run_once()
{
    std::unique_lock<std::mutex> lock(_entries_mutex);

    for (auto& entry : _entries) {
        if (_time.elapsed_since_s(entry.last_time) > double(entry.interval_s)) {
            _time.shift_steady_time_by(entry.last_time, double(entry.interval_s));

            if (entry.callback) {
                // Make a copy and unlock while we call back because it might
                // in turn want to remove or change it within.
                std::function<void()> callback = entry.callback;
                lock.unlock();
                callback();
                lock.lock();
            }
        }

        // We leave the loop.
        // FIXME: there should be a nicer way to do this.
        if (_iterator_invalidated) {
            _iterator_invalidated = false;
            break;
        }
    }
}

} // namespace mavsdk
