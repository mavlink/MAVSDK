#pragma once

#include "param_value.hpp"
#include <algorithm>
#include <functional>
#include <string>
#include <variant>
#include <vector>
#include <asio/io_context.hpp>
#include <asio/post.hpp>

namespace mavsdk {

// Util for exposing the functionality of subscribing to parameter changes
// Note: maybe it would be cleaner to also have the parameter set in here, and make this class
// A "MavlinkParameterSet" so to say where set_xxx properly calls the subscriptions.
// But for now, I don't want to go down that route yet since I don't know if that isn't too much of
// inheritance. NOTE: r.n the inherited class still needs to remember to call
// find_and_call_subscriptions_value_changed() When a value is changed.
//
// Threading: the subscription list is only ever touched on the io_context thread.
// find_and_call_subscriptions_value_changed() runs there (it is driven by received
// param messages), and subscribe/unsubscribe post their mutation onto the same
// io_context. That keeps everything single-threaded without locks, and post() also
// makes it safe to (un)subscribe from inside a subscription callback (the mutation
// runs after the current dispatch returns).
class MavlinkParameterSubscription {
public:
    explicit MavlinkParameterSubscription(asio::io_context& io_context) :
        _io_context(io_context)
    {}

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
            asio::post(_io_context, [this, subscription = std::move(subscription)]() mutable {
                _param_changed_subscriptions.push_back(std::move(subscription));
            });
        } else {
            asio::post(_io_context, [this, name, cookie]() {
                _param_changed_subscriptions.erase(
                    std::remove_if(
                        _param_changed_subscriptions.begin(),
                        _param_changed_subscriptions.end(),
                        [&](const auto& subscription) {
                            return subscription.param_name == name && subscription.cookie == cookie;
                        }),
                    _param_changed_subscriptions.end());
            });
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

    // Remove all subscriptions for the given cookie. Must be called on the io_context
    // thread (or while it is stopped), e.g. from within a handler already posted there.
    void unsubscribe_all_params_changed(const void* cookie);

    // Same, but synchronous and callable from any thread: the removal is run on the
    // io_context thread and this returns only once it has completed, so no callback for
    // this cookie can fire afterwards. Safe to call from the io_context thread too (the
    // removal then runs inline rather than waiting on itself).
    void unsubscribe_all_params_changed_blocking(const void* cookie);

protected:
    /**
     * Find all the subscriptions for the given @param param_name,
     * check their type and call them when matching. This does not check if the given param actually
     * was changed, but it is safe to call with mismatching types.
     *
     * Must be called on the io_context thread.
     */
    void find_and_call_subscriptions_value_changed(
        const std::string& param_name, const ParamValue& new_param_value);

private:
    asio::io_context& _io_context;
    std::vector<ParamChangedSubscription> _param_changed_subscriptions{};
};

} // namespace mavsdk
