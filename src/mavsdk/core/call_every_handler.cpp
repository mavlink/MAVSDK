#include "call_every_handler.h"

#include <utility>
#include <algorithm>
#include <vector>

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
    new_entry.cookie = _next_cookie++;

    std::lock_guard<std::recursive_mutex> lock(_mutex);
    _entries.push_back(new_entry);

    return new_entry.cookie;
}

void CallEveryHandler::change(double interval_s, Cookie cookie)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
        return entry.cookie == cookie;
    });
    if (it != _entries.end()) {
        it->interval_s = interval_s;
    }
}

void CallEveryHandler::reset(Cookie cookie)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
        return entry.cookie == cookie;
    });
    if (it != _entries.end()) {
        it->last_time = _time.steady_time();
    }
}

void CallEveryHandler::remove(Cookie cookie)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    auto it = std::find_if(
        _entries.begin(), _entries.end(), [&](auto& timeout) { return timeout.cookie == cookie; });

    if (it != _entries.end()) {
        _entries.erase(it);
    }
}

void CallEveryHandler::run_once()
{
    // First, identify all entries that need to be executed and update timestamps
    // while holding the lock
    std::vector<std::function<void()>> callbacks_to_execute;

    {
        std::lock_guard<std::recursive_mutex> lock(_mutex);

        for (auto& entry : _entries) {
            if (_time.elapsed_since_s(entry.last_time) > double(entry.interval_s)) {
                // Update the timestamp before potentially executing
                _time.shift_steady_time_by(entry.last_time, double(entry.interval_s));

                // Store the callback for later execution
                if (entry.callback) {
                    callbacks_to_execute.push_back(entry.callback);
                }
            }
        }
    }

    // Now execute all callbacks outside the lock to prevent lock-order inversions
    for (const auto& callback : callbacks_to_execute) {
        callback();
    }
}

} // namespace mavsdk
