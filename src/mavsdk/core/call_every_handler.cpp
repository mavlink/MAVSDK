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

    // Try to acquire the lock and add directly to _entries
    if (_entries_mutex.try_lock()) {
        _entries.push_back(new_entry);
        _entries_mutex.unlock();
    } else {
        // If we couldn't acquire the lock, add to _add_list
        std::lock_guard<std::mutex> lock(_add_mutex);
        _add_list.push_back(new_entry);
    }

    return new_entry.cookie;
}

void CallEveryHandler::change(double interval_s, Cookie cookie)
{
    // Try to acquire the lock and change directly in _entries
    if (_entries_mutex.try_lock()) {
        auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
            return entry.cookie == cookie;
        });
        if (it != _entries.end()) {
            it->interval_s = interval_s;
        }
        _entries_mutex.unlock();
    } else {
        // If we couldn't acquire the lock, we'll have to try again later
        // For simplicity, we don't queue changes
        std::lock_guard<std::mutex> lock(_change_mutex);
        auto entry = Entry{};
        entry.interval_s = interval_s;
        entry.cookie = cookie;
        _change_list.push_back(entry);
    }
}

void CallEveryHandler::reset(Cookie cookie)
{
    // Try to acquire the lock and reset directly in _entries
    if (_entries_mutex.try_lock()) {
        auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
            return entry.cookie == cookie;
        });
        if (it != _entries.end()) {
            it->last_time = _time.steady_time();
        }
        _entries_mutex.unlock();
    } else {
        // If we couldn't acquire the lock, we presumably just ran it this instance anyway.
    }
}

void CallEveryHandler::remove(Cookie cookie)
{
    // Try to acquire the lock and remove directly from _entries
    if (_entries_mutex.try_lock()) {
        auto it = std::find_if(_entries.begin(), _entries.end(), [&](auto& timeout) {
            return timeout.cookie == cookie;
        });

        if (it != _entries.end()) {
            _entries.erase(it);
        }
        _entries_mutex.unlock();
    } else {
        // If we couldn't acquire the lock, add to _remove_list
        std::lock_guard<std::mutex> lock(_remove_mutex);
        _remove_list.push_back(cookie);
    }
}

void CallEveryHandler::run_once()
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    {
        // Process add list
        std::lock_guard<std::mutex> add_lock(_add_mutex);
        for (auto& entry : _add_list) {
            _entries.push_back(entry);
        }
        _add_list.clear();
    }
    {
        // Process remove list
        std::lock_guard<std::mutex> remove_lock(_remove_mutex);
        for (const auto& cookie : _remove_list) {
            auto it = std::find_if(_entries.begin(), _entries.end(), [&](auto& timeout) {
                return timeout.cookie == cookie;
            });

            if (it != _entries.end()) {
                _entries.erase(it);
            }
        }
        _remove_list.clear();
    }
    {
        // Process change list
        std::lock_guard<std::mutex> change_lock(_change_mutex);
        for (const auto& change : _change_list) {
            auto it = std::find_if(_entries.begin(), _entries.end(), [&](const Entry& entry) {
                return entry.cookie == change.cookie;
            });
            if (it != _entries.end()) {
                it->interval_s = change.interval_s;
            }
        }
        _change_list.clear();
    }

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
