#pragma once

#include "sender.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "timeout_s_callback.h"
#include "param_value.h"
#include "locked_queue.h"
#include "mavlink_parameter_subscription.h"
#include "mavlink_parameter_cache.h"

#include <map>
#include <string>
#include <list>
#include <utility>

namespace mavsdk {

// This class provides parameters for other components to request or change.

class MavlinkParameterServer : public MavlinkParameterSubscription {
public:
    MavlinkParameterServer() = delete;
    explicit MavlinkParameterServer(
        Sender& parent,
        MavlinkMessageHandler& message_handler,
        // By providing all the parameters on construction you can populate the
        // parameter set before the server starts reacting to clients.
        //
        // Also see:
        // https://mavlink.io/en/services/parameter.html#parameters_invariant
        std::optional<std::map<std::string, ParamValue>> optional_param_values = std::nullopt);
    ~MavlinkParameterServer();

    enum class Result {
        Success,
        WrongType,
        ParamNameTooLong,
        NotFound,
        ParamValueTooLong,
        ParamExistsAlready,
        TooManyParams,
        ParamNotFound,
        Unknown,
    };

    Result provide_server_param(const std::string& name, const ParamValue& param_value);
    Result provide_server_param_float(const std::string& name, float value);
    Result provide_server_param_int(const std::string& name, int32_t value);
    Result provide_server_param_custom(const std::string& name, const std::string& value);
    std::map<std::string, ParamValue> retrieve_all_server_params();

    template<class T> std::pair<Result, T> retrieve_server_param(const std::string& name);
    std::pair<Result, float> retrieve_server_param_float(const std::string& name);
    std::pair<Result, int32_t> retrieve_server_param_int(const std::string& name);
    std::pair<Result, std::string> retrieve_server_param_custom(const std::string& name);

    void do_work();

    friend std::ostream& operator<<(std::ostream&, const Result&);

    // Non-copyable
    MavlinkParameterServer(const MavlinkParameterServer&) = delete;
    const MavlinkParameterServer& operator=(const MavlinkParameterServer&) = delete;

private:
    void process_param_set_internally(
        const std::string& param_id, const ParamValue& value_to_set, bool extended);
    void process_param_set(const mavlink_message_t& message);
    void process_param_ext_set(const mavlink_message_t& message);

    void process_param_request_read(const mavlink_message_t& message);
    void process_param_ext_request_read(const mavlink_message_t& message);

    void internal_process_param_request_read_by_id(const std::string& id, bool extended);
    void internal_process_param_request_read_by_index(std::uint16_t index, bool extended);

    void process_param_request_list(const mavlink_message_t& message);
    void process_param_ext_request_list(const mavlink_message_t& message);
    void broadcast_all_parameters(bool extended);

    bool target_matches(uint16_t target_sys_id, uint16_t target_comp_id, bool is_request);
    void log_target_mismatch(uint16_t target_sys_id, uint16_t target_comp_id);

    static std::variant<std::monostate, std::string, std::uint16_t>
    extract_request_read_param_identifier(int16_t param_index, const char* param_id);

    struct WorkItemValue {
        const uint16_t param_index;
        const uint16_t param_count;
        const bool extended;
    };

    struct WorkItemAck {
        const PARAM_ACK param_ack;
    };

    struct WorkItem {
        // A response always has a valid param id
        const std::string param_id;
        // as well as a valid param value
        const ParamValue param_value;
        using WorkItemVariant = std::variant<WorkItemValue, WorkItemAck>;
        const WorkItemVariant work_item_variant;
        explicit WorkItem(
            std::string param_id1, ParamValue param_value1, WorkItemVariant work_item_variant1) :
            param_id(std::move(param_id1)),
            param_value(std::move(param_value1)),
            work_item_variant(std::move(work_item_variant1)){};
    };

    Sender& _sender;
    MavlinkMessageHandler& _message_handler;

    std::mutex _all_params_mutex{};
    MavlinkParameterCache _param_cache{};

    LockedQueue<WorkItem> _work_queue{};

    bool _parameter_debugging = false;
};

} // namespace mavsdk
