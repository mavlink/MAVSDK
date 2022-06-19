#pragma once

#include "log.h"
#include "mavlink_include.h"
#include "mavlink_param_value.h"
#include "timeout_s_callback.h"
#include "locked_queue.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <functional>
#include <cassert>
#include <vector>
#include <map>
#include <optional>
#include <variant>

namespace mavsdk {

class Sender;
class MavlinkMessageHandler;
class TimeoutHandler;

// std::to_string doesn't work for std::string, so we need this workaround.
template<typename T> std::string to_string(T&& value)
{
    return std::to_string(std::forward<T>(value));
}

inline std::string& to_string(std::string& value)
{
    return value;
}

class MAVLinkParameters {
public:
    MAVLinkParameters() = delete;
    explicit MAVLinkParameters(
        Sender& parent,
        MavlinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler,
        TimeoutSCallback timeout_s_callback,
        bool is_server);
    ~MAVLinkParameters();

    enum class Result {
        Success,
        Timeout,
        ConnectionError,
        WrongType,
        ParamNameTooLong,
        NotFound,
        ValueUnsupported,
        Failed,
        ParamValueTooLong,
        UnknownError
    };

    Result set_param(
        const std::string& name,
        parameters::ParamValue value,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false);

    using SetParamCallback = std::function<void(Result result)>;

    /**
     * Send a message to the server to change the parameter indexed by name to the specified value.
     * Once the result of this operation is known, the user-specified callback is called with the result of this (set value) operation.
     * Note that to change a server parameter, one needs to know not only the value to set, but also the exact type this value has.
     */
    void set_param_async(
        const std::string& name,
	parameters::ParamValue value,
        const SetParamCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false);

    Result set_param_int(
        const std::string& name,
        int32_t value,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false,
        // Needs to be false by default, I don't know where people using the library assume the internal type hack is applied
        bool adhere_to_mavlink_specs= false);

    void set_param_int_async(
        const std::string& name,
        int32_t value,
        const SetParamCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false,
        // Needs to be false by default, I don't know where people using the library assume the internal type hack is applied
        bool adhere_to_mavlink_specs= false);

    Result set_param_float(
        const std::string& name,
        float value,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false);

    void set_param_float_async(
        const std::string& name,
        float value,
        const SetParamCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false);

    Result set_param_custom(const std::string& name, const std::string& value);

    void set_param_custom_async(
        const std::string& name,
        const std::string& value,
        const SetParamCallback& callback,
        const void* cookie = nullptr);

    /**
     * Add a new parameter to the parameter set (only for the server/providing side).
     * It is recommended to not change the parameter set after first communicating with any client.
     * (https://mavlink.io/en/services/parameter_ext.html#parameters_invariant)
     * @param name the unique id for this parameter
     * @param param_value the value for this parameter
     * @return Result::ParamNameTooLong if the parameter name is too long,
     * Result::Success otherwise.
     */
    Result provide_server_param(const std::string& name,parameters::ParamValue param_value);
    // convenient implementations for the 3 most commonly used types
    Result provide_server_param_float(const std::string& name, float value);
    Result provide_server_param_int(const std::string& name, int value);
    Result provide_server_param_custom(const std::string& name, const std::string& value);

    /**
     * Retrieve the current value for a parameter from the server parameter set.
     * @tparam T the type of the parameter to retrieve, if the parameter from the parameter set does not match this type,
     * the method will return MAVLinkParameters::Result::WrongType  and the value is default constructed.
     * @param name the name of the parameter to retrieve, if the parameter set does not contain this name key
     * MAVLinkParameters::Result::NotFound is returned and the value is default constructed
     * @return MAVLinkParameters::Result::Success if the name is a valid key for the parameter set, AND the type matches the value in the set.
     * Otherwise,one of the error codes above.
     */
    template<class T>
    std::pair<Result,T> retrieve_server_param(const std::string& name);
    // Convenient methods for the 3 commonly used types.
    std::pair<Result, float> retrieve_server_param_float(const std::string& name);
    std::pair<Result, int> retrieve_server_param_int(const std::string& name);
    std::pair<Result, std::string> retrieve_server_param_custom(const std::string& name);
    /**
     * @return a copy of the current parameter set of the server.
     */
    std::map<std::string, parameters::ParamValue> retrieve_all_server_params();

    using GetParamAnyCallback = std::function<void(Result, parameters::ParamValue)>;
    /**
     * This is the only type of communication from client to server that is possible in regard to type safety -
     * Ask the server for a parameter (when asking, the parameter type is still unknown) and then, once we got
     * a successful response from the server we know the parameter type and its value.
     * @param name name of the parameter to get.
     * @param callback callback that is called with the response from the server (type and value for this key are now known from the response)
     */
    void get_param_async(
        const std::string& name,
        GetParamAnyCallback callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false);
    // Blocking wrapper around get_param_async()
    std::pair<Result, parameters::ParamValue>
    get_param(const std::string& name, bool extended = false);

    /**
     * This is legacy code, the original implementation takes a parameters::ParamValue to check and infer the type.
     * If the type returned by get_param_async( typeless) matches the type provided by @param value_type, the callback is called with
     * Result::Success, one of the error codes otherwise.
     * TODO: In my opinion, this is not the most verbose implementation, since a parameters::ParamValue is constructed just to infer the type.
     */
    void get_param_async(
        const std::string& name,
        parameters::ParamValue value_type,
        const GetParamAnyCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false);

    /**
     * This could replace the code above.
     * We use get_param_async to get the current type and value for a parameter, then check the type
     * of the obtained parameter and return the result with the appropriate error codes via the callback.
     */
    template<class T>
    using GetParamTypesafeCallback = std::function<void(Result,T value)>;
    template<class T>
    void get_param_async_typesafe(
        const std::string& name,
        GetParamTypesafeCallback<T> callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false);

    std::pair<Result, float> get_param_float(
        const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended);

    using GetParamFloatCallback = std::function<void(Result, float)>;

    void get_param_float_async(
        const std::string& name,
        const GetParamFloatCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended);

    std::pair<Result, int32_t> get_param_int(
        const std::string& name, std::optional<uint8_t> maybe_component_id, bool extended);

    using GetParamIntCallback = std::function<void(Result, int32_t)>;

    void get_param_int_async(
        const std::string& name,
        const GetParamIntCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended);

    std::pair<Result, std::string> get_param_custom(const std::string& name);

    using GetParamCustomCallback = std::function<void(Result, const std::string& value)>;

    void get_param_custom_async(
        const std::string& name, const GetParamCustomCallback& callback, const void* cookie,std::optional<uint8_t> maybe_component_id=std::nullopt);

    // Note: When use_extended == false, this won't return any parameters that use a string as param value,
    // since the non-extended protocol is incapable of doing so.
    std::map<std::string, parameters::ParamValue> get_all_params(bool use_extended=false);
    using GetAllParamsCallback = std::function<void(std::map<std::string, parameters::ParamValue>)>;
    void get_all_params_async(const GetAllParamsCallback& callback,bool use_extended=false);

    using ParamFloatChangedCallback = std::function<void(float value)>;
    void subscribe_param_float_changed(
        const std::string& name, const ParamFloatChangedCallback& callback, const void* cookie);

    using ParamIntChangedCallback = std::function<void(int value)>;
    void subscribe_param_int_changed(
        const std::string& name, const ParamIntChangedCallback& callback, const void* cookie);

    using ParamCustomChangedCallback = std::function<void(std::string)>;
    void subscribe_param_custom_changed(
        const std::string& name, const ParamCustomChangedCallback& callback, const void* cookie);

    void cancel_all_param(const void* cookie);

    void do_work();

    friend std::ostream& operator<<(std::ostream&, const Result&);

    // Non-copyable
    MAVLinkParameters(const MAVLinkParameters&) = delete;
    const MAVLinkParameters& operator=(const MAVLinkParameters&) = delete;

private:
    using ParamChangedCallbacks = std::variant<ParamFloatChangedCallback, ParamIntChangedCallback, ParamCustomChangedCallback>;

    void process_param_value(const mavlink_message_t& message);
    void process_param_set(const mavlink_message_t& message);
    void process_param_ext_set(const mavlink_message_t& message);
    void process_param_ext_value(const mavlink_message_t& message);
    void process_param_ext_ack(const mavlink_message_t& message);
    void receive_timeout();

    void notify_param_subscriptions(const mavlink_param_value_t& param_value);

    static std::string extract_safe_param_id(const char param_id[]);

    static void
    call_param_changed_callback(const ParamChangedCallbacks& callback, const parameters::ParamValue& value);

    Sender& _sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;
    TimeoutSCallback _timeout_s_callback;

    // Params can be up to 16 chars without 0-termination.
    static constexpr size_t PARAM_ID_LEN = 16;

    struct WorkItem {
        enum class Type { Get, Set, Value, Ack };
        const Type type;
        const std::string param_name;
        using VariantCallback=std::variant<
            GetParamAnyCallback,
            SetParamCallback>;
        VariantCallback callback{};
        parameters::ParamValue param_value{};
        std::optional<uint8_t> maybe_component_id{};
        bool extended{false};
        bool already_requested{false};
        const void* cookie{nullptr};
        int retries_to_do{3};
        double timeout_s;
        int param_count{1};
        int param_index{0};
        mavlink_message_t mavlink_message{};

        explicit WorkItem(Type type1,std::string param_name1,double new_timeout_s) : type(type1),param_name(std::move(param_name1)),timeout_s(new_timeout_s){};
    };
    LockedQueue<WorkItem> _work_queue{};

    void* _timeout_cookie = nullptr;

    struct ParamChangedSubscription {
        std::string param_name{};
        ParamChangedCallbacks callback{};
	parameters::ParamValue value_type{};
        bool any_type{false};
        const void* cookie{nullptr};
    };

    std::mutex _param_changed_subscriptions_mutex{};
    std::vector<ParamChangedSubscription> _param_changed_subscriptions{};

    std::mutex _all_params_mutex{};
    GetAllParamsCallback _all_params_callback;
    void* _all_params_timeout_cookie{nullptr};
    std::map<std::string, parameters::ParamValue> _all_params{};

    const bool _is_server;

    void process_param_request_read(const mavlink_message_t& message);
    void process_param_ext_request_read(const mavlink_message_t& message);
    void process_param_request_list(const mavlink_message_t& message);
    // request list from a component that wants to also do strings and therefore has to use the extended parameter protocol).
    void process_param_ext_request_list(const mavlink_message_t& message);

    bool _parameter_debugging{false};

    // Return the n of parameters, either from an extended or non-extended perspective.
    // ( we need to hide parameters that need extended from non-extended queries).
    [[nodiscard]] int get_current_parameters_count(bool extended)const;
};

} // namespace mavsdk
