#pragma once

#include "log.h"
#include "mavlink_include.h"
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

    class ParamValue {
    public:
        bool set_from_mavlink_param_value_bytewise(const mavlink_param_value_t& mavlink_value);
        bool set_from_mavlink_param_value_cast(const mavlink_param_value_t& mavlink_value);
        bool set_from_mavlink_param_set_bytewise(const mavlink_param_set_t& mavlink_set);
        bool set_from_mavlink_param_ext_set(const mavlink_param_ext_set_t& mavlink_ext_set);
        bool set_from_mavlink_param_ext_value(const mavlink_param_ext_value_t& mavlink_ext_value);
        bool set_from_xml(const std::string& type_str, const std::string& value_str);
        bool set_empty_type_from_xml(const std::string& type_str);

        [[nodiscard]] MAV_PARAM_TYPE get_mav_param_type() const;
        [[nodiscard]] MAV_PARAM_EXT_TYPE get_mav_param_ext_type() const;

        bool set_as_same_type(const std::string& value_str);

        [[nodiscard]] float get_4_float_bytes_bytewise() const;
        [[nodiscard]] float get_4_float_bytes_cast() const;

        [[nodiscard]] std::optional<int> get_int() const;
        [[nodiscard]] std::optional<float> get_float() const;
        [[nodiscard]] std::optional<std::string> get_custom() const;

        bool set_int(int new_value);
        void set_float(float new_value);
        void set_custom(const std::string& new_value);

        std::array<char, 128> get_128_bytes() const;

        [[nodiscard]] std::string get_string() const;

        template<typename T>[[nodiscard]] bool is() const
        {
            return (std::get_if<T>(&_value) != nullptr);
        }

        template<typename T> T get() const { return std::get<T>(_value); }

        template<typename T> void set(T new_value) { _value = new_value; }

        [[nodiscard]] bool is_same_type(const ParamValue& rhs) const;

        bool operator==(const ParamValue& rhs) const
        {
            if (!is_same_type(rhs)) {
                LogWarn() << "Trying to compare different types.";
                return false;
            }

            return _value == rhs._value;
        }

        bool operator<(const ParamValue& rhs) const
        {
            if (!is_same_type(rhs)) {
                LogWarn() << "Trying to compare different types.";
                return false;
            }

            return _value < rhs._value;
        }

        bool operator>(const ParamValue& rhs) const
        {
            if (!is_same_type(rhs)) {
                LogWarn() << "Trying to compare different types.";
                return false;
            }

            return _value > rhs._value;
        }

        bool operator==(const std::string& value_str) const;

        [[nodiscard]] std::string typestr() const;

        std::variant<
            uint8_t,
            int8_t,
            uint16_t,
            int16_t,
            uint32_t,
            int32_t,
            uint64_t,
            int64_t,
            float,
            double,
            std::string>
            _value{};
    };

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

    // Result provide_server_param(const std::string& name, const ParamValue& value);
    Result provide_server_param_float(const std::string& name, float value);
    Result provide_server_param_int(const std::string& name, int value);
    Result provide_server_param_custom(const std::string& name, const std::string& value);
    std::map<std::string, MAVLinkParameters::ParamValue> retrieve_all_server_params();

    std::pair<Result, ParamValue>
    retrieve_server_param(const std::string& name, ParamValue value_type);
    std::pair<Result, float> retrieve_server_param_float(const std::string& name);
    std::pair<Result, int> retrieve_server_param_int(const std::string& name);
    std::pair<Result, std::string> retrieve_server_param_custom(const std::string& name);

    using GetParamAnyCallback = std::function<void(Result, ParamValue)>;

    std::pair<Result, ParamValue>
    get_param(const std::string& name, ParamValue value_type, bool extended = false);

    void get_param_async(
        const std::string& name,
        ParamValue value,
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

    std::map<std::string, MAVLinkParameters::ParamValue> get_all_params();
    using GetAllParamsCallback =
        std::function<void(std::map<std::string, MAVLinkParameters::ParamValue>)>;
    void get_all_params_async(const GetAllParamsCallback& callback);

    using ParamChangedCallback = std::function<void(ParamValue value)>;
    void subscribe_param_changed(
        const std::string& name, const ParamChangedCallback& callback, const void* cookie);

    void subscribe_param_changed(
        const std::string& name,
        ParamValue value_type,
        const ParamChangedCallback& callback,
        const void* cookie);

    void cancel_all_param(const void* cookie);

    void do_work();

    friend std::ostream& operator<<(std::ostream&, const ParamValue&);

    friend std::ostream& operator<<(std::ostream&, const Result&);

    // Non-copyable
    MAVLinkParameters(const MAVLinkParameters&) = delete;
    const MAVLinkParameters& operator=(const MAVLinkParameters&) = delete;

private:
    void process_param_value(const mavlink_message_t& message);
    void process_param_set(const mavlink_message_t& message);
    void process_param_ext_set(const mavlink_message_t& message);
    void process_param_ext_value(const mavlink_message_t& message);
    void process_param_ext_ack(const mavlink_message_t& message);
    void receive_timeout();

    void notify_param_subscriptions(const mavlink_param_value_t& param_value);

    static std::string extract_safe_param_id(const char param_id[]);

    Sender& _sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;
    TimeoutSCallback _timeout_s_callback;

    // Params can be up to 16 chars without 0-termination.
    static constexpr size_t PARAM_ID_LEN = 16;

    struct WorkItem {
        enum class Type { Get, Set, Value, Ack } type{Type::Get};
        std::variant<
            GetParamFloatCallback,
            GetParamIntCallback,
            GetParamCustomCallback,
            GetParamAnyCallback,
            SetParamCallback>
            callback{};
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

    void* _timeout_cookie = nullptr;

    struct ParamChangedSubscription {
        std::string param_name{};
        ParamChangedCallback callback{};
        ParamValue value_type{};
        bool any_type{false};
        const void* cookie{nullptr};
    };

    std::mutex _param_changed_subscriptions_mutex{};
    std::vector<ParamChangedSubscription> _param_changed_subscriptions{};

    std::mutex _all_params_mutex{};
    GetAllParamsCallback _all_params_callback;
    void* _all_params_timeout_cookie{nullptr};
    std::map<std::string, ParamValue> _all_params{};

    bool _is_server;

    void process_param_request_read(const mavlink_message_t& message);
    void process_param_ext_request_read(const mavlink_message_t& message);
    void process_param_request_list(const mavlink_message_t& message);

    bool _parameter_debugging{false};
};

} // namespace mavsdk
