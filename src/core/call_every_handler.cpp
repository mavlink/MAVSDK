#include "call_every_handler.h"

#include <utility>

namespace mavsdk {

CallEveryHandler::CallEveryHandler(Time& time) : _time(time) {}

void CallEveryHandler::add(std::function<void()> callback, double interval_s, void** cookie)
{
    auto new_entry = std::make_shared<Entry>();
    new_entry->callback = std::move(callback);
    auto before = _time.steady_time();
    // Make sure it gets run straightaway. The epsilon seemed not enough, so
    // we use the arbitrary value of 1 ms.
    _time.shift_steady_time_by(before, -interval_s - 0.001);
    new_entry->last_time = before;
    new_entry->interval_s = interval_s;

    void* new_cookie = static_cast<void*>(new_entry.get());

    {
        std::lock_guard<std::mutex> lock(_entries_mutex);
        _entries.insert(std::pair<void*, std::shared_ptr<Entry>>(new_cookie, new_entry));
    }

    if (cookie != nullptr) {
        *cookie = new_cookie;
    }
}

void CallEveryHandler::change(double interval_s, const void* cookie)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto it = _entries.find(const_cast<void*>(cookie));
    if (it != _entries.end()) {
        it->second->interval_s = interval_s;
    }
}

void CallEveryHandler::reset(const void* cookie)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto it = _entries.find(const_cast<void*>(cookie));
    if (it != _entries.end()) {
        it->second->last_time = _time.steady_time();
    }
}

void CallEveryHandler::remove(const void* cookie)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto it = _entries.find(const_cast<void*>(cookie));
    if (it != _entries.end()) {
        _entries.erase(const_cast<void*>(cookie));
        _iterator_invalidated = true;
    }
}

void CallEveryHandler::run_once()
{
    _entries_mutex.lock();

    for (auto& entry : _entries) {
        if (_time.elapsed_since_s(entry.second->last_time) > double(entry.second->interval_s)) {
            _time.shift_steady_time_by(entry.second->last_time, double(entry.second->interval_s));

            if (entry.second->callback) {
                // Get a copy for the callback because we unlock.
                std::function<void()> callback = entry.second->callback;

                // Unlock while we call back because it might in turn want to add timeouts.
                _entries_mutex.unlock();
                callback();
                _entries_mutex.lock();
            }
        }

        // We leave the loop.
        // FIXME: there should be a nicer way to do this.
        if (_iterator_invalidated) {
            _iterator_invalidated = false;
            break;
        }
    }
    _entries_mutex.unlock();
}

} // namespace mavsdk
