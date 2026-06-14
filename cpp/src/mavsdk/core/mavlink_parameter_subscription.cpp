#include "mavlink_parameter_subscription.hpp"

#include <algorithm>
#include <future>
#include <asio/dispatch.hpp>

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
    // Runs on the io_context thread; _param_changed_subscriptions is only touched there,
    // so no locking is needed.
    for (const auto& subscription : _param_changed_subscriptions) {
        if (subscription.param_name != param_name) {
            continue;
        }
        LogDebug("Param {} changed to {}", param_name, value.get_string());
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
            LogErr("Type and callback mismatch");
        }
    }
}

void MavlinkParameterSubscription::unsubscribe_all_params_changed(const void* cookie)
{
    // Must be called on the io_context thread (or while it is stopped); the list is only
    // touched there.
    _param_changed_subscriptions.erase(
        std::remove_if(
            _param_changed_subscriptions.begin(),
            _param_changed_subscriptions.end(),
            [&](const auto& subscription) { return subscription.cookie == cookie; }),
        _param_changed_subscriptions.end());
}

void MavlinkParameterSubscription::unsubscribe_all_params_changed_blocking(const void* cookie)
{
    if (_io_context.stopped()) {
        // The io_context thread is dead — safe to access directly.
        unsubscribe_all_params_changed(cookie);
        return;
    }

    // dispatch() runs the handler inline if we are already on the io_context thread
    // (so we never wait on ourselves and deadlock), and posts it otherwise.
    std::promise<void> done;
    asio::dispatch(_io_context, [this, cookie, &done]() {
        unsubscribe_all_params_changed(cookie);
        done.set_value();
    });
    done.get_future().wait();
}

} // namespace mavsdk
