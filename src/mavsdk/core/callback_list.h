#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace mavsdk {

template<typename... Args> class CallbackList;

template<typename... Args> class Handle {
public:
    explicit Handle(uint64_t id) : _id(id) {}

private:
    uint64_t _id;

    friend CallbackList<Args...>;
};

template<typename... Args> class CallbackList {
public:
    Handle<Args...> subscribe(const std::function<void(Args...)>&& callback)
    {
        check_removals();

        std::lock_guard<std::mutex> lock(_mutex);
        auto handle = Handle<Args...>(_last_id++);
        _list.emplace_back(handle, callback);
        return handle;
    }

    void unsubscribe(Handle<Args...> handle)
    {
        // We want to allow unsubscribing while in a callback. This would
        // normally lead to a deadlock. Therefore, we just try to grab the
        // lock here, and if it's already taken, we schedule the removal
        // for later.
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (lock.owns_lock()) {
            _list.erase(
                std::remove_if(
                    _list.begin(),
                    _list.end(),
                    [&](auto& pair) { return pair.first._id == handle._id; }),
                _list.end());
        } else {
            std::lock_guard<std::mutex> remove_later_lock(_remove_later_mutex);
            _remove_later.push_back(handle._id);
        }
    }

    void operator()(Args... args)
    {
        check_removals();

        std::lock_guard<std::mutex> lock(_mutex);
        for (const auto& pair : _list) {
            pair.second(args...);
        }
    }

private:
    void check_removals()
    {
        std::lock_guard<std::mutex> remove_later_lock(_remove_later_mutex);

        // We could probably just grab the lock here but it's safer not to
        // acquire both locks to avoid deadlocks.
        if (_mutex.try_lock()) {
            for (auto& id : _remove_later) {
                _list.erase(
                    std::remove_if(
                        _list.begin(),
                        _list.end(),
                        [&](auto& pair) { return pair.first._id == id; }),
                    _list.end());
            }
            _mutex.unlock();
        }
    }

    mutable std::mutex _mutex{};
    uint64_t _last_id{0};
    std::vector<std::pair<Handle<Args...>, std::function<void(Args...)>>> _list{};

    mutable std::mutex _remove_later_mutex{};
    std::vector<uint64_t> _remove_later{};
};

} // namespace mavsdk
