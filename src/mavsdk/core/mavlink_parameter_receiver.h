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

    Result provide_server_param_float(const std::string& name, float value);
    Result provide_server_param_int(const std::string& name, int value);
    Result provide_server_param_custom(const std::string& name, const std::string& value);
    std::map<std::string, ParamValue> retrieve_all_server_params();

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
    void receive_timeout();

    void notify_param_subscriptions(const mavlink_param_value_t& param_value);

    static std::string extract_safe_param_id(const char param_id[]);

    static void
    call_param_changed_callback(const ParamChangedCallbacks& callback, const ParamValue& value);

    Sender& _sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;
    TimeoutSCallback _timeout_s_callback;

    // Params can be up to 16 chars without 0-termination.
    static constexpr size_t PARAM_ID_LEN = 16;

    void* _timeout_cookie = nullptr;

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
    void* _all_params_timeout_cookie{nullptr};
    std::map<std::string, ParamValue> _all_params{};

    void process_param_request_read(const mavlink_message_t& message);
    void process_param_ext_request_read(const mavlink_message_t& message);
    void process_param_request_list(const mavlink_message_t& message);

    bool _parameter_debugging{false};

    struct WorkItem {
        enum class Type { Value, Ack } type{Type::Value};
        std::string param_name{};
        ParamValue param_value{};
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

        explicit WorkItem(double new_timeout_s) : timeout_s(new_timeout_s){};
    };
    LockedQueue<WorkItem> _work_queue{};
};

} // namespace mavsdk