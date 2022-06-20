#pragma once

#include "sender.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "timeout_s_callback.h"
#include "param_value.h"
#include "locked_queue.h"

#include <map>
#include <string>

namespace mavsdk {

class MavlinkParameterReceiver {
public:
    MavlinkParameterReceiver() = delete;
    explicit MavlinkParameterReceiver(
        Sender& parent,
        MavlinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler,
        TimeoutSCallback timeout_s_callback);
    ~MavlinkParameterReceiver();

    enum class Result {
        Success,
        Timeout,
        ConnectionError,
        WrongType,
        ParamNameTooLong,
        NotFound,
        Failed,
        ParamValueTooLong,
        UnknownError
    };

    /**
     * Add a new parameter to the parameter set.
     * It is recommended to not change the parameter set after first communicating with any client.
     * (https://mavlink.io/en/services/parameter_ext.html#parameters_invariant).
     * @param name the unique id for this parameter
     * @param param_value the value for this parameter
     * @return Result::ParamNameTooLong if the parameter name is too long,
     * Result::WrongType if the same parameter name is provided with a different type (aka updating the parameter would mutate the type
     * of an already provided parameter), Result::ParamValueTooLong if the parameter type is std::string but the value is longer than
     * the extended protocol allows and
     * Result::Success otherwise.
     */
    Result provide_server_param(const std::string& name,ParamValue param_value);
    // convenient implementations for the 3 most commonly used types
    Result provide_server_param_float(const std::string& name, float value);
    Result provide_server_param_int(const std::string& name, int value);
    Result provide_server_param_custom(const std::string& name, const std::string& value);
    /**
     * @return a copy of the current parameter set of the server.
     */
    std::map<std::string, ParamValue> retrieve_all_server_params();

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
    std::pair<Result, float> retrieve_server_param_float(const std::string& name);
    std::pair<Result, int> retrieve_server_param_int(const std::string& name);
    std::pair<Result, std::string> retrieve_server_param_custom(const std::string& name);

    using ParamFloatChangedCallback = std::function<void(float value)>;
    void subscribe_param_float_changed(
        const std::string& name, const ParamFloatChangedCallback& callback, const void* cookie);

    using ParamIntChangedCallback = std::function<void(int value)>;
    void subscribe_param_int_changed(
        const std::string& name, const ParamIntChangedCallback& callback, const void* cookie);

    using ParamCustomChangedCallback = std::function<void(std::string)>;
    void subscribe_param_custom_changed(
        const std::string& name, const ParamCustomChangedCallback& callback, const void* cookie);

    void do_work();

    friend std::ostream& operator<<(std::ostream&, const Result&);

    // Non-copyable
    MavlinkParameterReceiver(const MavlinkParameterReceiver&) = delete;
    const MavlinkParameterReceiver& operator=(const MavlinkParameterReceiver&) = delete;

private:
    using ParamChangedCallbacks = std::
        variant<ParamFloatChangedCallback, ParamIntChangedCallback, ParamCustomChangedCallback>;

    void process_param_set(const mavlink_message_t& message);
    void process_param_ext_set(const mavlink_message_t& message);

    static std::string extract_safe_param_id(const char param_id[]);

    static void
    call_param_changed_callback(const ParamChangedCallbacks& callback, const ParamValue& value);

    Sender& _sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutSCallback _timeout_s_callback;

    // Params can be up to 16 chars without 0-termination.
    static constexpr size_t PARAM_ID_LEN = 16;

    struct ParamChangedSubscription {
        std::string param_name{};
        ParamChangedCallbacks callback{};
        ParamValue value_type{};
        bool any_type{false};
        const void* cookie{nullptr};
    };

    std::mutex _param_changed_subscriptions_mutex{};
    std::vector<ParamChangedSubscription> _param_changed_subscriptions{};

    std::mutex _all_params_mutex{};
    std::map<std::string, ParamValue> _all_params{};

    void process_param_request_read(const mavlink_message_t& message);
    void process_param_ext_request_read(const mavlink_message_t& message);
    void process_param_request_list(const mavlink_message_t& message);
    void process_param_ext_request_list(const mavlink_message_t& message);

    struct WorkItem {
        enum class Type {
            Value, // Emitted on a get value or set value for non-extended
            Ack  // Emitted on a set value for the extended protocoll only
        };
        const Type type;
        const std::string param_name;
        const bool extended;
        ParamValue param_value{};
        bool already_requested{false};
        const void* cookie{nullptr};
        const double timeout_s;
        int param_count{1};
        int param_index{0};
        mavlink_message_t mavlink_message{};

        explicit WorkItem(Type type1,std::string param_name1,bool extended1,double new_timeout_s) :
            type(type1),param_name(std::move(param_name1)),extended(extended1),timeout_s(new_timeout_s){};
    };
    LockedQueue<WorkItem> _work_queue{};
    // Return the n of parameters, either from an extended or non-extended perspective.
    // ( we need to hide parameters that need extended from non-extended queries).
    [[nodiscard]] int get_current_parameters_count(bool extended)const;
};

} // namespace mavsdk