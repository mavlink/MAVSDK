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
        bool extended = false);

    void set_param_int_async(
        const std::string& name,
        int32_t value,
        const SetParamCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false);

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
    // convenient implementations for the most commonly used types
    Result provide_server_param_float(const std::string& name, float value);
    Result provide_server_param_int(const std::string& name, int value);
    Result provide_server_param_custom(const std::string& name, const std::string& value);
    std::map<std::string, parameters::ParamValue> retrieve_all_server_params();

    template<class T>
    std::pair<Result,T> retrieve_server_param(const std::string& name);
    std::pair<Result, float> retrieve_server_param_float(const std::string& name);
    std::pair<Result, int> retrieve_server_param_int(const std::string& name);
    std::pair<Result, std::string> retrieve_server_param_custom(const std::string& name);

    using GetParamAnyCallback = std::function<void(Result, parameters::ParamValue)>;

    std::pair<Result, parameters::ParamValue>
    get_param(const std::string& name, parameters::ParamValue value_type, bool extended = false);

    void get_param_async(
        const std::string& name,
	parameters::ParamValue value,
        const GetParamAnyCallback& callback,
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
        const std::string& name, const GetParamCustomCallback& callback, const void* cookie);

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
            GetParamFloatCallback,
            GetParamIntCallback,
            GetParamCustomCallback,
            GetParamAnyCallback,
            SetParamCallback>;
        VariantCallback callback{};
        parameters::ParamValue param_value{};
        std::optional<uint8_t> maybe_component_id{};
        bool extended{false};
        bool already_requested{false};
        bool exact_type_known{false};
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
