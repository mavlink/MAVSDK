#include "call_every_handler.h"

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
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    // Process entries and collect callbacks that need to be executed
    for (auto& entry : _entries) {
        if (_time.elapsed_since_s(entry.last_time) > double(entry.interval_s)) {
            _time.shift_steady_time_by(entry.last_time, double(entry.interval_s));

            if (entry.callback) {
                entry.callback();
            }
        }
    }
}

} // namespace mavsdk
