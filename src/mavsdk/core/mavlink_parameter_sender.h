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
        TimeoutSCallback timeout_s_callback);
    ~MavlinkParameterSender();
    // Non-copyable
    MavlinkParameterSender(const MavlinkParameterSender&) = delete;
    const MavlinkParameterSender& operator=(const MavlinkParameterSender&) = delete;

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
        ParamValue value,
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
        ParamValue value,
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
        const void* cookie,
        std::optional<uint8_t> maybe_component_id,
        bool extended = false);
    // Blocking wrapper around get_param_async()
    std::pair<Result, ParamValue>
    get_param(const std::string& name, bool extended = false);

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
    std::map<std::string, ParamValue> get_all_params(bool use_extended=false);
    using GetAllParamsCallback = std::function<void(std::map<std::string, ParamValue>)>;
    void get_all_params_async(const GetAllParamsCallback& callback,bool use_extended=false);

    void cancel_all_param(const void* cookie);

    void do_work();

    friend std::ostream& operator<<(std::ostream&, const Result&);

private:

    void process_param_value(const mavlink_message_t& message);
    void process_param_ext_value(const mavlink_message_t& message);
    void process_param_ext_ack(const mavlink_message_t& message);
    void receive_timeout();

    Sender& _sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;
    TimeoutSCallback _timeout_s_callback;

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
        const bool extended;
        const std::optional<uint8_t> maybe_component_id;
        bool already_requested{false};
        const void* cookie{nullptr};
        int retries_to_do{3};
        // we need to keep a copy of the message in case a transmission is lost and we want to re-transmit it.
        // TODO: Don't we need a new message sequence number for that ? Not sure.
        mavlink_message_t mavlink_message{};

        explicit WorkItem(double new_timeout_s,WorkItemVariant work_item_variant1,const void* cookie1,
                          bool extended1,std::optional<uint8_t> maybe_component_id1) :
                timeout_s(new_timeout_s),work_item_variant(std::move(work_item_variant1)),
                extended(extended1),maybe_component_id(maybe_component_id1),cookie(cookie1){

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
    std::map<std::string, ParamValue> _all_params{};
    ParamSetFromServer _param_set_from_server;
    bool _all_params_request_extended=false;

    // once the parameter count has been set, it should not change - but we cannot say for certain since
    // the server might do whatever he wants.
    std::optional<uint16_t> _server_param_count;
    // log a warning when the parameter count from the server changes, this is not forbidden but dangerous
    // https://mavlink.io/en/services/parameter.html#parameters_invariant
    void validate_parameter_count(uint16_t param_count);

    bool _parameter_debugging{false};

    // Validate if the response matches what was given in the work queue
    static bool validate_id_or_index(const std::variant<std::string,int16_t>& original,const std::string& param_id,int16_t param_index);

    void check_for_full_parameter_set(const std::string& safe_param_id,uint16_t param_idx,uint16_t all_param_count,const ParamValue& received_value,bool extended);
    void check_all_params_timeout();

    GetParamAnyCallback create_recursive_callback(){
        const auto callback=[this](Result res,ParamValue unused){
            std::lock_guard<std::mutex> lock(_all_params_mutex);
            if(res==Result::Success){
                const auto missing=_param_set_from_server.get_missing_param_indices();
                if(missing.empty()){
                    assert(_param_set_from_server.is_complete());
                    // we are done, the parameter set is complete.
                }else{
                    // Request the next parameter still missing
                    const auto next_missing_param=missing.at(0);
                    LogDebug()<<"Requesting missing parameter "<<(int)next_missing_param;
                    auto new_work = std::make_shared<WorkItem>(_timeout_s_callback(),
                                                               WorkItemGet{static_cast<int16_t>(next_missing_param),create_recursive_callback()}, this,_all_params_request_extended,std::nullopt);
                    _work_queue.push_back(new_work);
                }
            }else{
                LogDebug()<<"Get param used for GetAllParameters failed";
                if(_all_params_callback){
                    _all_params_callback({});
                    _all_params_callback= nullptr;
                }
            }
        };
        return callback;
    }
};

} // namespace mavsdk
