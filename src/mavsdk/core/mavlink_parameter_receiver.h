#pragma once

#include "sender.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "timeout_s_callback.h"
#include "param_value.h"
#include "locked_queue.h"
#include "mavlink_parameter_subscription.h"

#include <map>
#include <string>
#include <list>

namespace mavsdk {

/**
 * One could also call this MavlinkParameterProvider or MavlinkParameterServer but all these names don't really fit perfectly.
 * This class makes it easy to accomplish the following task:
 * Having a component that exposes some settings for a user to configure.
 * In General, the usage of this class is as following:
 * 1) provide parameters (now other components can request anc change these parameters via mavlink).
 * 2) add listener(s) for these parameters such that you can react to changes set by another component.
 * 3) let mavlink do its magic.
 * Note that this side is much more simple - it does not need to worry about re-transmission or such, the responsibility for
 * that lies completely on the part that wants to change parameters.
 */
class MavlinkParameterReceiver : public MavlinkParameterSubscription{
public:
    MavlinkParameterReceiver() = delete;
    explicit MavlinkParameterReceiver(
        Sender& parent,
        MavlinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler,
        TimeoutSCallback timeout_s_callback);
    ~MavlinkParameterReceiver();

    enum class Result {
        Success, // All Ok
        WrongType, // Wrong type provided
        ParamNameTooLong, // param name provided too long
        NotFound, // get_xxx param not found
        ParamValueTooLong // value for param of type string doesn't fit into extended protocol.
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

    void do_work();

    friend std::ostream& operator<<(std::ostream&, const Result&);

    // Non-copyable
    MavlinkParameterReceiver(const MavlinkParameterReceiver&) = delete;
    const MavlinkParameterReceiver& operator=(const MavlinkParameterReceiver&) = delete;

private:
    /**
     * internally process a param set, coming from either the extended or non-extended protocol.
     * This checks and properly handles the following conditions:
     * 1) weather the param is inside the parameter set
     * 2) weather the type of the param inside the parameter set matches the type from the request
     * 3) TODO what to do if the value to set matches the current value.
     * @param param_id the id of the parameter in the set request message
     * @param value the value obtained from the set request message
     * @param extended true if the message is coming from the extended protocol,false otherwise. The response workflow
     * is slightly different on the extended protocol.
     */
    void process_param_set_internally(const std::string& param_id,const ParamValue& value,bool extended);
    void process_param_set(const mavlink_message_t& message);
    void process_param_ext_set(const mavlink_message_t& message);

    static std::string extract_safe_param_id(const char param_id[]);

    Sender& _sender;
    MavlinkMessageHandler& _message_handler;

    // Params can be up to 16 chars without 0-termination.
    static constexpr size_t PARAM_ID_LEN = 16;

    std::mutex _all_params_mutex{};
    std::map<std::string, ParamValue> _all_params{};
    // broadcast a specific parameter if found, ignores string parameters
    void process_param_request_read(const mavlink_message_t& message);
    // broadcast a specific parameter if found
    void process_param_ext_request_read(const mavlink_message_t& message);
    // broadcast all parameters, ignores string parameters
    void process_param_request_list(const mavlink_message_t& message);
    // broadcast all parameters
    void process_param_ext_request_list(const mavlink_message_t& message);

    // On the server side, the only benefit of using the work item pattern (mavsdk specific)
    // is that the request all parameters command(s) are less likely to saturate the link.
    struct WorkItem {
        enum class Type {
            Value, // Emitted on a get value or set value for non-extended, broadcast the current value
            Ack  // Emitted on a set value for the extended protocol only
        };
        const Type type;
        const std::string param_name;
        const bool extended;
        ParamValue param_value{};
        bool already_requested{false};
        const void* cookie{nullptr};
        int param_count{1};
        int param_index{0};
        PARAM_ACK param_ack=PARAM_ACK_ACCEPTED; // only for extended protocol

        explicit WorkItem(Type type1,std::string param_name1,bool extended1) :
            type(type1),param_name(std::move(param_name1)),extended(extended1){};
    };
    LockedQueue<WorkItem> _work_queue{};
    /*
     * Return the n of parameters, either from an extended or non-extended perspective.
     * ( we need to hide parameters that need extended from non-extended queries).
     * Doesn't acquire the all-parameters lock, since when used it should already be locked.
     */
    [[nodiscard]] int get_current_parameters_count(bool extended)const;
};

} // namespace mavsdk