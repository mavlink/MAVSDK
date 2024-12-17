#pragma once

#include "param_value.h"
#include <mutex>
#include <string>
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
            cookie(cookie1){};
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
        std::lock_guard<std::mutex> lock(_param_changed_subscriptions_mutex);
        if (callback != nullptr) {
            ParamChangedSubscription subscription{name, callback, cookie};
            // This is just to let the upper level know of what probably is a bug, but we check
            // again when actually calling the callback We also cannot assume here that the user
            // called provide_param before subscribe_param_changed, so the only thing that makes
            // sense is to log a warning, but then continue anyways.
            /*std::lock_guard<std::mutex> lock2(_all_params_mutex);
            if (_all_params.find(name) != _all_params.end()) {
                const auto curr_value = _all_params.at(name);
                if (!curr_value.template is_same_type_templated<T>()) {
                    LogDebug()
                        << "You just registered a param changed callback where the type does not
            match the type already stored";
                }
            }*/
            _param_changed_subscriptions.push_back(subscription);
        } else {
            for (auto it = _param_changed_subscriptions.begin();
                 it != _param_changed_subscriptions.end();
                 /* ++it */) {
                if (it->param_name == name && it->cookie == cookie) {
                    it = _param_changed_subscriptions.erase(it);
                } else {
                    ++it;
                }
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
    std::mutex _param_changed_subscriptions_mutex{};
    std::vector<ParamChangedSubscription> _param_changed_subscriptions{};
};

} // namespace mavsdk
