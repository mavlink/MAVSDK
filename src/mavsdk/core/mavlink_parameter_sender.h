#pragma once

#include "log.h"
#include "mavlink_include.h"
#include "timeout_s_callback.h"
#include "locked_queue.h"
#include "param_value.h"
#include "mavlink_parameter_subscription.h"
#include "mavlink_parameter_set.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <functional>
#include <utility>
#include <vector>
#include <map>
#include <optional>
#include <variant>

namespace mavsdk {

class Sender;
class MavlinkMessageHandler;
class TimeoutHandler;

class MavlinkParameterSender : public MavlinkParameterSubscription{
public:
    MavlinkParameterSender() = delete;
    explicit MavlinkParameterSender(
        Sender& parent,
        MavlinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler,
        TimeoutSCallback timeout_s_callback,
        // target component id (the param providing component this sender should talk to)
        uint8_t target_component_id=MAV_COMP_ID_AUTOPILOT1,
        // weather to use the extended protocol or not - it is not possible to mix them up.
        bool use_extended=false);
    ~MavlinkParameterSender();
    // Non-copyable
    MavlinkParameterSender(const MavlinkParameterSender&) = delete;
    const MavlinkParameterSender& operator=(const MavlinkParameterSender&) = delete;

    /**
     * argh what the crap, we could have nicely done these in the constructor with const
     * @param target_component_id target component id (the param providing component this sender should talk to)
     * @param use_extended weather to use the extended protocol or not - it is not possible to mix them up.
     */
    void late_init(uint8_t target_component_id,bool use_extended);

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
        ParamValue value);

    using SetParamCallback = std::function<void(Result result)>;

    /**
     * Send a message to the server to change the parameter indexed by name to the specified value.
     * Once the result of this operation is known, the user-specified callback is called with the result of this (set value) operation.
     * Note that to change a server parameter, one needs to know not only the value to set, but also the exact type this value has.
     */
    void set_param_async(
        const std::string& name,
        ParamValue value,
        const SetParamCallback& callback,
        const void* cookie);

    Result set_param_int(
        const std::string& name,
        int32_t value,
        // Needs to be false by default, I don't know where people using the library assume the internal type hack is applied
        bool adhere_to_mavlink_specs= false);

    void set_param_int_async(
        const std::string& name,
        int32_t value,
        const SetParamCallback& callback,
        const void* cookie,
        // Needs to be false by default, I don't know where people using the library assume the internal type hack is applied
        bool adhere_to_mavlink_specs= false);

    Result set_param_float(
        const std::string& name,
        float value);

    void set_param_float_async(
        const std::string& name,
        float value,
        const SetParamCallback& callback,
        const void* cookie);

    Result set_param_custom(const std::string& name, const std::string& value);

    void set_param_custom_async(
        const std::string& name,
        const std::string& value,
        const SetParamCallback& callback,
        const void* cookie = nullptr);

    using GetParamAnyCallback = std::function<void(Result, ParamValue)>;
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
        const void* cookie);
    std::pair<Result, ParamValue>
    get_param(const std::string& name);

    /**
     * This is legacy code, the original implementation takes a ParamValue to check and infer the type.
     * If the type returned by get_param_async( typeless) matches the type provided by @param value_type, the callback is called with
     * Result::Success, one of the error codes otherwise.
     * TODO: In my opinion, this is not the most verbose implementation, since a ParamValue is constructed just to infer the type.
     */
    void get_param_async(
        const std::string& name,
        ParamValue value_type,
        const GetParamAnyCallback& callback,
        const void* cookie);

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
        const void* cookie);

    std::pair<Result, float> get_param_float(const std::string& name);

    using GetParamFloatCallback = std::function<void(Result, float)>;

    void get_param_float_async(
        const std::string& name,
        const GetParamFloatCallback& callback,
        const void* cookie);

    std::pair<Result, int32_t> get_param_int(const std::string& name);

    using GetParamIntCallback = std::function<void(Result, int32_t)>;

    void get_param_int_async(
        const std::string& name,
        const GetParamIntCallback& callback,
        const void* cookie);

    std::pair<Result, std::string> get_param_custom(const std::string& name);

    using GetParamCustomCallback = std::function<void(Result, const std::string& value)>;

    void get_param_custom_async(
        const std::string& name, const GetParamCustomCallback& callback, const void* cookie);

    enum GetAllParamsResult{
        ConnectionError, // Cannot send message(s)
        InconsistentData, // The param server is buggy and doesn't allow parameter synchronization
        Timeout, // There is too much packet loss, the synchronization did not succeed. Perhaps try again.
        Unknown, // Something unexpected happened, can point to a buggy param server
        Success // We got the full parameter set from the server
    };

    using GetAllParamsCallback = std::function<void(GetAllParamsResult result,std::map<std::string, ParamValue> set)>;
    /**
     * Try to obtain the complete parameter set (all the parameters the server provides). In case of packet loss/
     * a param server with a lot of parameters, this might take a significant amount of time. The callback is called with
     * a full parameter set on success, an empty parameter set on failure.
     * @param callback callback to be called when done.
     * @param clear_cache when set to true, clear the previous full / partial parameter set from the server. This is needed
     * in case the server parameter set is invariant.
     */
    void get_all_params_async(GetAllParamsCallback callback,bool clear_cache=false);
    std::map<std::string, ParamValue> get_all_params(bool clear_cache=false);

    void cancel_all_param(const void* cookie);

    void do_work();

    friend std::ostream& operator<<(std::ostream&, const Result&);
    friend std::ostream& operator<<(std::ostream&, const GetAllParamsResult&);

private:

    void process_param_value(const mavlink_message_t& message);
    void process_param_ext_value(const mavlink_message_t& message);
    void process_param_ext_ack(const mavlink_message_t& message);
    void receive_timeout();

    Sender& _sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;
    TimeoutSCallback _timeout_s_callback;
    // target component id (the param providing component this sender should talk to)
    uint8_t _target_component_id=MAV_COMP_ID_AUTOPILOT1;
    // weather to use the extended protocol or not - it is not possible to mix them up.
    bool _use_extended=false;

    // These are specific depending on the work item type
    struct WorkItemSet{
        const std::string param_name;
        const ParamValue param_value;
        const SetParamCallback callback;
    };
    struct WorkItemGet{
        // We can get a parameter from the server by either its string id or index.
        const std::variant<std::string,int16_t> param_identifier;
        const GetParamAnyCallback callback;
    };
    struct WorkItem {
        enum class Type { Get, Set};
        const double timeout_s;
        using WorkItemVariant=std::variant<WorkItemGet,WorkItemSet>;
        WorkItemVariant work_item_variant;
        bool already_requested{false};
        const void* cookie{nullptr};
        int retries_to_do{3};
        // we need to keep a copy of the message in case a transmission is lost and we want to re-transmit it.
        // TODO: Don't we need a new message sequence number for that ? Not sure.
        mavlink_message_t mavlink_message{};

        explicit WorkItem(double new_timeout_s,WorkItemVariant work_item_variant1,const void* cookie1) :
                timeout_s(new_timeout_s),work_item_variant(std::move(work_item_variant1)),cookie(cookie1){

            };
        [[nodiscard]] Type get_type()const{
            if(std::holds_alternative<WorkItemGet>(work_item_variant)) {
                return Type::Get;
            }
            return Type::Set;
        }
    };
    LockedQueue<WorkItem> _work_queue{};

    void* _timeout_cookie = nullptr;

    std::mutex _all_params_mutex{};
    GetAllParamsCallback _all_params_callback;
    void* _all_params_timeout_cookie{nullptr};
    ParamSetFromServer _param_set_from_server;

    bool _parameter_debugging=true;

    // Validate if the response matches what was given in the work queue
    static bool validate_id_or_index(const std::variant<std::string,int16_t>& original,const std::string& param_id,int16_t param_index);

    // This adds the given parameter to the parameter set cache (if possible) and then checks and call the
    // _all_params_callback() if it is set and the parameter set has become complete after adding this parameter.
    void add_param_to_cached_parameter_set(const std::string& safe_param_id,uint16_t param_idx,uint16_t all_param_count,const ParamValue& received_value);
    void check_all_params_timeout();
    // Create a callback for a WorkItemGet that performs the following steps:
    // 1) Check if any parameter of the parameter set is missing.
    // 2) If yes, request the first missing parameter and recursively add the same callback to check and request again
    // 3) If no, terminate.
    GetParamAnyCallback create_recursive_callback();
};

} // namespace mavsdk
