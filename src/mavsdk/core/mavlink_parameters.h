#pragma once

#include "log.h"
#include "global_include.h"
#include "mavlink_include.h"
#include "locked_queue.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <functional>
#include <cassert>
#include <vector>
#include <map>
#include <variant>

namespace mavsdk {

class SystemImpl;

class MAVLinkParameters {
public:
    explicit MAVLinkParameters(SystemImpl& parent);
    ~MAVLinkParameters();

    class ParamValue {
    public:
        bool set_from_mavlink_param_value(const mavlink_param_value_t& mavlink_value);
        bool set_from_mavlink_param_set(const mavlink_param_set_t& mavlink_set);
        bool set_from_mavlink_param_ext_set(const mavlink_param_ext_set_t& mavlink_ext_set);
        bool set_from_mavlink_param_ext_value(const mavlink_param_ext_value_t& mavlink_ext_value);
        bool set_from_xml(const std::string& type_str, const std::string& value_str);
        bool set_empty_type_from_xml(const std::string& type_str);

        [[nodiscard]] MAV_PARAM_TYPE get_mav_param_type() const;
        [[nodiscard]] MAV_PARAM_EXT_TYPE get_mav_param_ext_type() const;

        bool set_as_same_type(const std::string& value_str);

        [[nodiscard]] float get_4_float_bytes() const;

        void get_128_bytes(char* bytes) const;

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
            double>
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
        UnknownError
    };

    typedef std::function<void(Result result)> set_param_callback_t;

    Result set_param(const std::string& name, const ParamValue& value, bool extended = false);

    void set_param_async(
        const std::string& name,
        const ParamValue& value,
        const set_param_callback_t& callback,
        const void* cookie = nullptr,
        bool extended = false);

    void provide_server_param(const std::string& name, const ParamValue& value);
    std::map<std::string, MAVLinkParameters::ParamValue> retrieve_all_server_params();

    std::pair<Result, ParamValue>
    retrieve_server_param(const std::string& name, ParamValue value_type);
    std::pair<Result, ParamValue>
    get_param(const std::string& name, ParamValue value_type, bool extended);
    typedef std::function<void(Result, ParamValue value)> get_param_callback_t;
    void get_param_async(
        const std::string& name,
        ParamValue value_type,
        const get_param_callback_t& callback,
        const void* cookie,
        bool extended = false);

    std::map<std::string, MAVLinkParameters::ParamValue> get_all_params();
    typedef std::function<void(std::map<std::string, MAVLinkParameters::ParamValue>)>
        get_all_params_callback_t;
    void get_all_params_async(const get_all_params_callback_t& callback);

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

    SystemImpl& _parent;

    // Params can be up to 16 chars without 0-termination.
    static constexpr size_t PARAM_ID_LEN = 16;

    struct WorkItem {
        enum class Type { Get, Set, Value, Ack } type{Type::Get};
        // TODO: a union would be nicer for the callback
        get_param_callback_t get_param_callback{nullptr};
        set_param_callback_t set_param_callback{nullptr};
        std::string param_name{};
        ParamValue param_value{};
        bool extended{false};
        int retries_done{0};
        bool already_requested{false};
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

    struct AllParameters {
        std::map<std::string, ParamValue> all_params{};
        get_all_params_callback_t callback{nullptr};
        void* timeout_cookie{nullptr};
    };
    std::shared_ptr<AllParameters> _all_param_store{nullptr};
    std::mutex _all_param_mutex{};

    // dl_time_t _last_request_time = {};

    std::map<std::string, ParamValue> _param_server_store;
    void process_param_request_read(const mavlink_message_t& message);
    void process_param_ext_request_read(const mavlink_message_t& message);
    void process_param_request_list(const mavlink_message_t& message);
};

} // namespace mavsdk
