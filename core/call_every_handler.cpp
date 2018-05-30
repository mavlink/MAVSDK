#include "call_every_handler.h"

namespace dronecore {

CallEveryHandler::CallEveryHandler(Time &time) : _time(time) {}

CallEveryHandler::~CallEveryHandler() {}

void CallEveryHandler::add(std::function<void()> callback, float interval_s, void **cookie)
{
    auto new_entry = std::make_shared<Entry>();
    new_entry->callback = callback;
    new_entry->last_time = _time.steady_time();
    new_entry->interval_s = interval_s;

    void *new_cookie = static_cast<void *>(new_entry.get());

    {
        std::lock_guard<std::mutex> lock(_entries_mutex);
        _entries.insert(std::pair<void *, std::shared_ptr<Entry>>(new_cookie, new_entry));
    }

    if (cookie != nullptr) {
        *cookie = new_cookie;
    }
}

void CallEveryHandler::change(float interval_s, const void *cookie)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto it = _entries.find(const_cast<void *>(cookie));
    if (it != _entries.end()) {
        it->second->interval_s = interval_s;
    }
}

void CallEveryHandler::reset(const void *cookie)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto it = _entries.find(const_cast<void *>(cookie));
    if (it != _entries.end()) {
        it->second->last_time = _time.steady_time();
    }
}

void CallEveryHandler::remove(const void *cookie)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto it = _entries.find(const_cast<void *>(cookie));
    if (it != _entries.end()) {
        _entries.erase(const_cast<void *>(cookie));
        _iterator_invalidated = true;
    }
}

void CallEveryHandler::run_once()
{
    _entries_mutex.lock();

    for (auto it = _entries.begin(); it != _entries.end(); ++it) {
        if (_time.elapsed_since_s(it->second->last_time) > double(it->second->interval_s)) {
            _time.shift_steady_time_by(it->second->last_time, double(it->second->interval_s));

            if (it->second->callback) {
                // Get a copy for the callback because we unlock.
                std::function<void()> callback = it->second->callback;

                // Unlock while we callback because it might in turn want to add timeouts.
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

} // namespace dronecore
