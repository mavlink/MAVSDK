#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <atomic>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include "log.h"
#include "callback_list.h"
#include "handle_factory.h"

namespace mavsdk {

template<typename... Args> class CallbackListImpl {
public:
    Handle<Args...> subscribe(const std::function<void(Args...)>& callback)
    {
        check_removals();
        process_subscriptions();

        // We need to return a handle, even if the callback is nullptr to
        // unsubscribe. That's fine, the handle just won't remove anything
        // when/if used later.
        auto handle = _handle_factory.create();

        if (callback != nullptr) {
            if (_mutex.try_lock()) {
                // We've acquired the lock without blocking, so we can modify the list.
                _list.emplace_back(handle, callback);
                _mutex.unlock();
            } else {
                // We couldn't acquire the lock because we're likely in a callback.
                // Defer the subscription.
                std::lock_guard<std::mutex> lock(_subscribe_later_mutex);
                _subscribe_later.emplace_back(handle, callback);
            }
        } else {
            LogErr() << "Use new unsubscribe methods instead of subscribe(nullptr)\n"
                     << "See: https://mavsdk.mavlink.io/main/en/cpp/api_changes.html#unsubscribe";
            try_clear();
        }

        return handle;
    }

    void unsubscribe(Handle<Args...> handle)
    {
        // Ignore null handle.
        if (!handle.valid()) {
            LogErr() << "Invalid null handle";
            return;
        }

        // We want to allow unsubscribing while in a callback. This would
        // normally lead to a deadlock. Therefore, we just try to grab the
        // lock here, and if it's already taken, we schedule the removal
        // for later.
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (lock.owns_lock()) {
            _list.erase(
                std::remove_if(
                    _list.begin(), _list.end(), [&](auto& pair) { return pair.first == handle; }),
                _list.end());
        } else {
            std::lock_guard<std::mutex> remove_later_lock(_remove_later_mutex);
            _remove_later.push_back(handle);
        }

        // check and remove from deferred lock list if present
        std::lock_guard<std::mutex> later_lock(_subscribe_later_mutex);
        _subscribe_later.erase(
            std::remove_if(
                _subscribe_later.begin(),
                _subscribe_later.end(),
                [&](const auto& pair) { return pair.first == handle; }),
            _subscribe_later.end());
    }

    /**
     * @brief Subscribe a new conditional callback to the list. Conditional callbacks
     * automatically unsubscribe if the callback evaluates to true, so the user does not
     * have to manage a handle for 'one-shot' callbacks.
     *
     * @param callback The callback function to subscribe.
     * @return void Since removal is handled internally, we dont need to expose the Handle.
     */
    void subscribe_conditional(const std::function<bool(Args...)>& callback)
    {
        check_removals();
        process_subscriptions();

        if (callback != nullptr) {
            if (_mutex.try_lock()) {
                _cond_cb_list.emplace_back(callback);
                _mutex.unlock();
            } else {
                // We couldn't acquire the lock because we're likely in a callback.
                // Defer the subscription.
                std::lock_guard<std::mutex> lock(_subscribe_later_mutex);
                _subscribe_later_cond.emplace_back(callback);
            }
        } else {
            try_clear();
        }
    }

    void exec(Args... args)
    {
        check_removals();
        process_subscriptions();

        std::lock_guard<std::mutex> lock(_mutex);
        for (const auto& pair : _list) {
            pair.second(args...);
        }

        for (auto it = _cond_cb_list.begin(); it != _cond_cb_list.end();) {
            if ((*it)(args...)) {
                // If the callback returns true, remove it based on the iterator
                it = _cond_cb_list.erase(it);
            } else {
                // Otherwise, move to the next element
                ++it;
            }
        }
    }

    void queue(Args... args, const std::function<void(const std::function<void()>&)>& queue_func)
    {
        check_removals();
        process_subscriptions();

        std::lock_guard<std::mutex> lock(_mutex);

        for (const auto& pair : _list) {
            queue_func([callback = pair.second, args...]() { callback(args...); });
        }
    }

    bool empty()
    {
        check_removals();
        process_subscriptions();

        std::lock_guard<std::mutex> lock(_mutex);
        return _list.empty() && _cond_cb_list.empty();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _list.clear();
        _cond_cb_list.clear();
    }

private:
    void check_removals()
    {
        std::lock_guard<std::mutex> remove_later_lock(_remove_later_mutex);

        // We could probably just grab the lock here, but it's safer not to
        // acquire both locks to avoid deadlocks.
        if (_mutex.try_lock()) {
            if (_remove_all_later) {
                _remove_all_later = false;
                _list.clear();
                _cond_cb_list.clear();
                _remove_later.clear();
            }

            for (const auto& handle : _remove_later) {
                _list.erase(
                    std::remove_if(
                        _list.begin(),
                        _list.end(),
                        [&](auto& pair) { return pair.first == handle; }),
                    _list.end());
            }
            _mutex.unlock();
        }
    }

    void process_subscriptions()
    {
        std::lock_guard<std::mutex> subscribe_later_lock(_subscribe_later_mutex);

        if (_mutex.try_lock()) {
            for (const auto& sub : _subscribe_later) {
                _list.emplace_back(sub);
            }
            _subscribe_later.clear();

            // add conditional callbacks
            for (const auto& sub : _subscribe_later_cond) {
                _cond_cb_list.emplace_back(sub);
            }
            _subscribe_later_cond.clear();
            _mutex.unlock();
        }
    }

    void try_clear()
    {
        std::unique_lock<std::mutex> lock(_mutex, std::try_to_lock);
        if (lock.owns_lock()) {
            _list.clear();
        } else {
            std::lock_guard<std::mutex> remove_later_lock(_remove_later_mutex);
            _remove_all_later = true;
        }
    }

    mutable std::mutex _mutex{};
    HandleFactory<Args...> _handle_factory;
    std::vector<std::pair<Handle<Args...>, std::function<void(Args...)>>> _list{};
    std::vector<std::function<bool(Args...)>> _cond_cb_list{};

    mutable std::mutex _remove_later_mutex{};
    std::vector<Handle<Args...>> _remove_later{};

    std::mutex _subscribe_later_mutex;
    std::vector<std::pair<Handle<Args...>, std::function<void(Args...)>>> _subscribe_later;
    std::vector<std::function<bool(Args...)>> _subscribe_later_cond;

    bool _remove_all_later{false};
};

} // namespace mavsdk
