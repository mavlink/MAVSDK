#include "mavlink_parameter_subscription.h"

#include <algorithm>

namespace mavsdk {

void MavlinkParameterSubscription::subscribe_param_float_changed(
    const std::string& name, const ParamFloatChangedCallback& callback, const void* cookie)
{
    subscribe_param_changed<float>(name, callback, cookie);
}

void MavlinkParameterSubscription::subscribe_param_int_changed(
    const std::string& name, const ParamIntChangedCallback& callback, const void* cookie)
{
    subscribe_param_changed<int>(name, callback, cookie);
}

void MavlinkParameterSubscription::subscribe_param_custom_changed(
    const std::string& name, const ParamCustomChangedCallback& callback, const void* cookie)
{
    subscribe_param_changed<std::string>(name, callback, cookie);
}

void MavlinkParameterSubscription::find_and_call_subscriptions_value_changed(
    const std::string& param_name, const ParamValue& value)
{
    // Process any deferred operations before calling subscriptions
    process_deferred_operations();

    // Now lock the mutex and call the subscriptions
    std::lock_guard<std::mutex> lock(_param_changed_subscriptions_mutex);
    for (const auto& subscription : _param_changed_subscriptions) {
        if (subscription.param_name != param_name) {
            continue;
        }
        LogDebug() << "Param " << param_name << " changed to " << value;
        // We have a subscription on this param name, now check if the subscription is for the right
        // type and call the callback when matching
        if (std::get_if<ParamFloatChangedCallback>(&subscription.callback) && value.get_float()) {
            std::get<ParamFloatChangedCallback>(subscription.callback)(value.get_float().value());
        } else if (
            std::get_if<ParamIntChangedCallback>(&subscription.callback) && value.get_int()) {
            std::get<ParamIntChangedCallback>(subscription.callback)(value.get_int().value());
        } else if (
            std::get_if<ParamCustomChangedCallback>(&subscription.callback) && value.get_custom()) {
            std::get<ParamCustomChangedCallback>(subscription.callback)(value.get_custom().value());
        } else {
            // The callback we have set is not for this type.
            LogErr() << "Type and callback mismatch";
        }
    }
}

void MavlinkParameterSubscription::unsubscribe_all_params_changed(const void* cookie)
{
    // Process any deferred operations first
    process_deferred_operations();

    // Try to lock the mutex, if it fails we're likely in a callback
    if (_param_changed_subscriptions_mutex.try_lock()) {
        // We've acquired the lock without blocking, so we can modify the list
        _param_changed_subscriptions.erase(
            std::remove_if(
                _param_changed_subscriptions.begin(),
                _param_changed_subscriptions.end(),
                [&](const auto& subscription) { return subscription.cookie == cookie; }),
            _param_changed_subscriptions.end());
        _param_changed_subscriptions_mutex.unlock();
    } else {
        // We couldn't acquire the lock because we're likely in a callback
        // Defer the unsubscription of all params for this cookie
        std::lock_guard<std::mutex> lock(_deferred_unsubscriptions_mutex);
        // Add a special marker for unsubscribe_all (empty param_name)
        _deferred_unsubscriptions.push_back({"", cookie});
    }
}

} // namespace mavsdk
