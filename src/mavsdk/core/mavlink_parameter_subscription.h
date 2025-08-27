#pragma once

#include "param_value.h"
#include <algorithm>
#include <functional>
#include <mutex>
#include <string>
#include <variant>
#include <vector>

namespace mavsdk {

// Util for exposing the functionality of subscribing to parameter changes
// Note: maybe it would be cleaner to also have the parameter set in here, and make this class
// A "MavlinkParameterSet" so to say where set_xxx properly calls the subscriptions.
// But for now, I don't want to go down that route yet since I don't know if that isn't too much of
// inheritance. NOTE: r.n the inherited class still needs to remember to call
// find_and_call_subscriptions_value_changed() When a value is changed.
class MavlinkParameterSubscription {
public:
    template<class T> using ParamChangedCallback = std::function<void(T value)>;

    using ParamChangedCallbacks = std::variant<
        ParamChangedCallback<uint8_t>,
        ParamChangedCallback<uint16_t>,
        ParamChangedCallback<uint32_t>,
        ParamChangedCallback<uint64_t>,
        ParamChangedCallback<int8_t>,
        ParamChangedCallback<int16_t>,
        ParamChangedCallback<int32_t>,
        ParamChangedCallback<int64_t>,
        ParamChangedCallback<float>,
        ParamChangedCallback<double>,
        ParamChangedCallback<std::string>>;

    struct ParamChangedSubscription {
        std::string param_name;
        ParamChangedCallbacks callback;
        const void* cookie;
        ParamChangedSubscription(
            std::string param_name1, ParamChangedCallbacks callback1, const void* cookie1) :
            param_name(std::move(param_name1)),
            callback(std::move(callback1)),
            cookie(cookie1) {};
    };

    /**
     * Subscribe to changes on the parameter referenced by @param name.
     * If the value for this parameter changes, the given callback is called provided that the
     * expected type matches the actual type of the parameter.
     */
    template<class T>
    void subscribe_param_changed(
        const std::string& name, const ParamChangedCallback<T>& callback, const void* cookie)
    {
        if (callback != nullptr) {
            ParamChangedSubscription subscription{name, callback, cookie};

            // Try to lock the mutex, if it fails we're likely in a callback
            if (_param_changed_subscriptions_mutex.try_lock()) {
                // We've acquired the lock without blocking, so we can modify the list
                _param_changed_subscriptions.push_back(subscription);
                _param_changed_subscriptions_mutex.unlock();
            } else {
                // We couldn't acquire the lock because we're likely in a callback
                // Defer the subscription
                std::lock_guard<std::mutex> lock(_deferred_subscriptions_mutex);
                _deferred_subscriptions.push_back(subscription);
            }
        } else {
            // For unsubscribing, we also need to handle the case where we're in a callback
            if (_param_changed_subscriptions_mutex.try_lock()) {
                // We've acquired the lock without blocking, so we can modify the list
                for (auto it = _param_changed_subscriptions.begin();
                     it != _param_changed_subscriptions.end();
                     /* ++it */) {
                    if (it->param_name == name && it->cookie == cookie) {
                        it = _param_changed_subscriptions.erase(it);
                    } else {
                        ++it;
                    }
                }
                _param_changed_subscriptions_mutex.unlock();
            } else {
                // We couldn't acquire the lock because we're likely in a callback
                // Defer the unsubscription
                std::lock_guard<std::mutex> lock(_deferred_unsubscriptions_mutex);
                _deferred_unsubscriptions.push_back({name, cookie});
            }
        }
    }

    using ParamFloatChangedCallback = ParamChangedCallback<float>;
    using ParamIntChangedCallback = ParamChangedCallback<int>;
    using ParamCustomChangedCallback = ParamChangedCallback<std::string>;

    void subscribe_param_float_changed(
        const std::string& name, const ParamFloatChangedCallback& callback, const void* cookie);
    void subscribe_param_int_changed(
        const std::string& name, const ParamIntChangedCallback& callback, const void* cookie);
    void subscribe_param_custom_changed(
        const std::string& name, const ParamCustomChangedCallback& callback, const void* cookie);

    void unsubscribe_all_params_changed(const void* cookie);

protected:
    /**
     * Find all the subscriptions for the given @param param_name,
     * check their type and call them when matching. This does not check if the given param actually
     * was changed, but it is safe to call with mismatching types.
     */
    void find_and_call_subscriptions_value_changed(
        const std::string& param_name, const ParamValue& new_param_value);

private:
    // Process any deferred subscriptions or unsubscriptions
    void process_deferred_operations()
    {
        // Process deferred subscriptions
        {
            std::lock_guard<std::mutex> lock(_deferred_subscriptions_mutex);
            if (!_deferred_subscriptions.empty() && _param_changed_subscriptions_mutex.try_lock()) {
                for (const auto& subscription : _deferred_subscriptions) {
                    _param_changed_subscriptions.push_back(subscription);
                }
                _deferred_subscriptions.clear();
                _param_changed_subscriptions_mutex.unlock();
            }
        }

        // Process deferred unsubscriptions
        {
            std::lock_guard<std::mutex> lock(_deferred_unsubscriptions_mutex);
            if (!_deferred_unsubscriptions.empty() &&
                _param_changed_subscriptions_mutex.try_lock()) {
                for (const auto& unsub : _deferred_unsubscriptions) {
                    if (unsub.param_name.empty()) {
                        // This is a special marker for unsubscribe_all_params_changed
                        _param_changed_subscriptions.erase(
                            std::remove_if(
                                _param_changed_subscriptions.begin(),
                                _param_changed_subscriptions.end(),
                                [&](const auto& subscription) {
                                    return subscription.cookie == unsub.cookie;
                                }),
                            _param_changed_subscriptions.end());
                    } else {
                        // This is a normal unsubscription for a specific parameter
                        for (auto it = _param_changed_subscriptions.begin();
                             it != _param_changed_subscriptions.end();
                             /* ++it */) {
                            if (it->param_name == unsub.param_name && it->cookie == unsub.cookie) {
                                it = _param_changed_subscriptions.erase(it);
                            } else {
                                ++it;
                            }
                        }
                    }
                }
                _deferred_unsubscriptions.clear();
                _param_changed_subscriptions_mutex.unlock();
            }
        }
    }

    struct DeferredUnsubscription {
        std::string param_name;
        const void* cookie;
    };

    std::mutex _param_changed_subscriptions_mutex{};
    std::vector<ParamChangedSubscription> _param_changed_subscriptions{};

    std::mutex _deferred_subscriptions_mutex{};
    std::vector<ParamChangedSubscription> _deferred_subscriptions{};

    std::mutex _deferred_unsubscriptions_mutex{};
    std::vector<DeferredUnsubscription> _deferred_unsubscriptions{};
};

} // namespace mavsdk
