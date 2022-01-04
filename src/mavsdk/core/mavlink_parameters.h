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
        bool set_from_mavlink_param_value(mavlink_param_value_t mavlink_value)
        {
            union {
                float float_value;
                int32_t int32_value;
            } temp{};

            temp.float_value = mavlink_value.param_value;
            switch (mavlink_value.param_type) {
                case MAV_PARAM_TYPE_UINT32:
                // FALLTHROUGH
                case MAV_PARAM_TYPE_INT32:
                    _value = temp.int32_value;
                    break;
                case MAV_PARAM_TYPE_REAL32:
                    _value = temp.float_value;
                    break;
                default:
                    // This would be worrying
                    LogErr() << "Error: unknown mavlink param type";
                    return false;
            }
            return true;
        }

        bool set_from_mavlink_param_set(mavlink_param_set_t mavlink_set)
        {
            mavlink_param_value_t mavlink_value{};
            mavlink_value.param_value = mavlink_set.param_value;
            mavlink_value.param_type = mavlink_set.param_type;
            return set_from_mavlink_param_value(mavlink_value);
        }

        bool set_from_mavlink_param_ext_set(const mavlink_param_ext_set_t& mavlink_ext_set)
        {
            switch (mavlink_ext_set.param_type) {
                case MAV_PARAM_EXT_TYPE_UINT8: {
                    uint8_t temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_INT8: {
                    int8_t temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_UINT16: {
                    uint16_t temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_INT16: {
                    int16_t temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_UINT32: {
                    uint32_t temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_INT32: {
                    int32_t temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_UINT64: {
                    uint64_t temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_INT64: {
                    int64_t temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_REAL32: {
                    float temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_REAL64: {
                    double temp;
                    memcpy(&temp, &mavlink_ext_set.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_CUSTOM:
                    LogErr() << "EXT_TYPE_CUSTOM is not supported";
                    return false;
                default:
                    // This would be worrying
                    LogErr() << "Error: unknown mavlink ext param type";
                    assert(false);
                    return false;
            }
            return true;
        }

        bool set_from_mavlink_param_ext_value(mavlink_param_ext_value_t mavlink_ext_value)
        {
            switch (mavlink_ext_value.param_type) {
                case MAV_PARAM_EXT_TYPE_UINT8: {
                    uint8_t temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_INT8: {
                    int8_t temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_UINT16: {
                    uint16_t temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_INT16: {
                    int16_t temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_UINT32: {
                    uint32_t temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_INT32: {
                    int32_t temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_UINT64: {
                    uint64_t temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_INT64: {
                    int64_t temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_REAL32: {
                    float temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_REAL64: {
                    double temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_EXT_TYPE_CUSTOM:
                    LogErr() << "EXT_TYPE_CUSTOM is not supported";
                    return false;
                default:
                    // This would be worrying
                    LogErr() << "Error: unknown mavlink ext param type";
                    assert(false);
                    return false;
            }
            return true;
        }

        bool set_from_xml(const std::string& type_str, const std::string& value_str)
        {
            if (strcmp(type_str.c_str(), "uint8") == 0) {
                uint8_t temp = std::stoi(value_str);
                _value = temp;
            } else if (strcmp(type_str.c_str(), "int8") == 0) {
                int8_t temp = std::stoi(value_str);
                _value = temp;
            } else if (strcmp(type_str.c_str(), "uint16") == 0) {
                uint16_t temp = std::stoi(value_str);
                _value = temp;
            } else if (strcmp(type_str.c_str(), "int16") == 0) {
                int16_t temp = std::stoi(value_str);
                _value = temp;
            } else if (strcmp(type_str.c_str(), "uint32") == 0) {
                uint32_t temp = std::stoi(value_str);
                _value = temp;
            } else if (strcmp(type_str.c_str(), "int32") == 0) {
                int32_t temp = std::stoi(value_str);
                _value = temp;
            } else if (strcmp(type_str.c_str(), "uint64") == 0) {
                uint64_t temp = std::stoll(value_str);
                _value = temp;
            } else if (strcmp(type_str.c_str(), "int64") == 0) {
                int64_t temp = std::stoll(value_str);
                _value = temp;
            } else if (strcmp(type_str.c_str(), "float") == 0) {
                float temp = std::stof(value_str);
                _value = temp;
            } else if (strcmp(type_str.c_str(), "double") == 0) {
                double temp = std::stod(value_str);
                _value = temp;
            } else {
                LogErr() << "Unknown type: " << type_str;
                return false;
            }
            return true;
        }

        bool set_empty_type_from_xml(const std::string& type_str)
        {
            if (strcmp(type_str.c_str(), "uint8") == 0) {
                _value = uint8_t(0);
            } else if (strcmp(type_str.c_str(), "int8") == 0) {
                _value = int8_t(0);
            } else if (strcmp(type_str.c_str(), "uint16") == 0) {
                _value = uint16_t(0);
            } else if (strcmp(type_str.c_str(), "int16") == 0) {
                _value = int16_t(0);
            } else if (strcmp(type_str.c_str(), "uint32") == 0) {
                _value = uint32_t(0);
            } else if (strcmp(type_str.c_str(), "int32") == 0) {
                _value = int32_t(0);
            } else if (strcmp(type_str.c_str(), "uint64") == 0) {
                _value = uint64_t(0);
            } else if (strcmp(type_str.c_str(), "int64") == 0) {
                _value = int64_t(0);
            } else if (strcmp(type_str.c_str(), "float") == 0) {
                _value = 0.0f;
            } else if (strcmp(type_str.c_str(), "double") == 0) {
                _value = 0.0;
            } else {
                LogErr() << "Unknown type: " << type_str;
                return false;
            }
            return true;
        }

        [[nodiscard]] MAV_PARAM_TYPE get_mav_param_type() const
        {
            if (std::get_if<float>(&_value)) {
                return MAV_PARAM_TYPE_REAL32;
            } else if (std::get_if<int32_t>(&_value)) {
                return MAV_PARAM_TYPE_INT32;
            } else {
                LogErr() << "Unknown param type sent";
                return MAV_PARAM_TYPE_REAL32;
            }
        }

        [[nodiscard]] MAV_PARAM_EXT_TYPE get_mav_param_ext_type() const
        {
            if (std::get_if<uint8_t>(&_value)) {
                return MAV_PARAM_EXT_TYPE_UINT8;
            } else if (std::get_if<int8_t>(&_value)) {
                return MAV_PARAM_EXT_TYPE_INT8;
            } else if (std::get_if<uint16_t>(&_value)) {
                return MAV_PARAM_EXT_TYPE_UINT16;
            } else if (std::get_if<int16_t>(&_value)) {
                return MAV_PARAM_EXT_TYPE_INT16;
            } else if (std::get_if<uint32_t>(&_value)) {
                return MAV_PARAM_EXT_TYPE_UINT32;
            } else if (std::get_if<int32_t>(&_value)) {
                return MAV_PARAM_EXT_TYPE_INT32;
            } else if (std::get_if<uint64_t>(&_value)) {
                return MAV_PARAM_EXT_TYPE_UINT64;
            } else if (std::get_if<int64_t>(&_value)) {
                return MAV_PARAM_EXT_TYPE_INT64;
            } else if (std::get_if<float>(&_value)) {
                return MAV_PARAM_EXT_TYPE_REAL32;
            } else if (std::get_if<double>(&_value)) {
                return MAV_PARAM_EXT_TYPE_REAL64;
            } else {
                LogErr() << "Unknown data type for param.";
                assert(false);
                return MAV_PARAM_EXT_TYPE_INT32;
            }
        }

        bool set_as_same_type(const std::string& value_str)
        {
            if (std::get_if<uint8_t>(&_value)) {
                _value = uint8_t(std::stoi(value_str));
            } else if (std::get_if<int8_t>(&_value)) {
                _value = int8_t(std::stoi(value_str));
            } else if (std::get_if<uint16_t>(&_value)) {
                _value = uint16_t(std::stoi(value_str));
            } else if (std::get_if<int16_t>(&_value)) {
                _value = int16_t(std::stoi(value_str));
            } else if (std::get_if<uint32_t>(&_value)) {
                _value = uint32_t(std::stoi(value_str));
            } else if (std::get_if<int32_t>(&_value)) {
                _value = int32_t(std::stoi(value_str));
            } else if (std::get_if<uint64_t>(&_value)) {
                _value = uint64_t(std::stoll(value_str));
            } else if (std::get_if<int64_t>(&_value)) {
                _value = int64_t(std::stoll(value_str));
            } else if (std::get_if<float>(&_value)) {
                _value = float(std::stof(value_str));
            } else if (std::get_if<double>(&_value)) {
                _value = double(std::stod(value_str));
            } else {
                LogErr() << "Unknown type";
                return false;
            }
            return true;
        }

        [[nodiscard]] float get_4_float_bytes() const
        {
            if (std::get_if<float>(&_value)) {
                return std::get<float>(_value);
            } else if (std::get_if<int32_t>(&_value)) {
                return *(reinterpret_cast<const float*>(&std::get<int32_t>(_value)));
            } else {
                LogErr() << "Unknown type";
                assert(false);
                return NAN;
            }
        }

        void get_128_bytes(char* bytes) const
        {
            if (std::get_if<uint8_t>(&_value)) {
                memcpy(bytes, &std::get<uint8_t>(_value), sizeof(uint8_t));
            } else if (std::get_if<int8_t>(&_value)) {
                memcpy(bytes, &std::get<int8_t>(_value), sizeof(int8_t));
            } else if (std::get_if<uint16_t>(&_value)) {
                memcpy(bytes, &std::get<uint16_t>(_value), sizeof(uint16_t));
            } else if (std::get_if<int16_t>(&_value)) {
                memcpy(bytes, &std::get<int16_t>(_value), sizeof(int16_t));
            } else if (std::get_if<uint32_t>(&_value)) {
                memcpy(bytes, &std::get<uint32_t>(_value), sizeof(uint32_t));
            } else if (std::get_if<int32_t>(&_value)) {
                memcpy(bytes, &std::get<int32_t>(_value), sizeof(int32_t));
            } else if (std::get_if<uint64_t>(&_value)) {
                memcpy(bytes, &std::get<uint64_t>(_value), sizeof(uint64_t));
            } else if (std::get_if<int64_t>(&_value)) {
                memcpy(bytes, &std::get<int64_t>(_value), sizeof(int64_t));
            } else if (std::get_if<float>(&_value)) {
                memcpy(bytes, &std::get<float>(_value), sizeof(float));
            } else if (std::get_if<double>(&_value)) {
                memcpy(bytes, &std::get<double>(_value), sizeof(double));
            } else {
                LogErr() << "Unknown type";
                assert(false);
            }
        }

        [[nodiscard]] std::string get_string() const
        {
            if (std::get_if<uint8_t>(&_value)) {
                return std::to_string(std::get<uint8_t>(_value));
            } else if (std::get_if<int8_t>(&_value)) {
                return std::to_string(std::get<int8_t>(_value));
            } else if (std::get_if<uint16_t>(&_value)) {
                return std::to_string(std::get<uint16_t>(_value));
            } else if (std::get_if<int16_t>(&_value)) {
                return std::to_string(std::get<int16_t>(_value));
            } else if (std::get_if<uint32_t>(&_value)) {
                return std::to_string(std::get<uint32_t>(_value));
            } else if (std::get_if<int32_t>(&_value)) {
                return std::to_string(std::get<int32_t>(_value));
            } else if (std::get_if<uint64_t>(&_value)) {
                return std::to_string(std::get<uint64_t>(_value));
            } else if (std::get_if<int64_t>(&_value)) {
                return std::to_string(std::get<int64_t>(_value));
            } else if (std::get_if<float>(&_value)) {
                return std::to_string(std::get<float>(_value));
            } else if (std::get_if<double>(&_value)) {
                return std::to_string(std::get<double>(_value));
            } else {
                LogErr() << "Unknown data type for param.";
                assert(false);
                return {"(unknown)"};
            }
        }

        template<typename T>[[nodiscard]] bool is() const
        {
            return (std::get_if<T>(&_value) != nullptr);
        }

        template<typename T> T get() const { return std::get<T>(_value); }

        template<typename T> void set(T new_value) { _value = new_value; }

        [[nodiscard]] bool is_same_type(const ParamValue& rhs) const
        {
            if ((std::get_if<uint8_t>(&_value) && std::get_if<uint8_t>(&rhs._value)) ||
                (std::get_if<int8_t>(&_value) && std::get_if<int8_t>(&rhs._value)) ||
                (std::get_if<uint16_t>(&_value) && std::get_if<uint16_t>(&rhs._value)) ||
                (std::get_if<int16_t>(&_value) && std::get_if<int16_t>(&rhs._value)) ||
                (std::get_if<uint32_t>(&_value) && std::get_if<uint32_t>(&rhs._value)) ||
                (std::get_if<int32_t>(&_value) && std::get_if<int32_t>(&rhs._value)) ||
                (std::get_if<uint64_t>(&_value) && std::get_if<uint64_t>(&rhs._value)) ||
                (std::get_if<int64_t>(&_value) && std::get_if<int64_t>(&rhs._value)) ||
                (std::get_if<float>(&_value) && std::get_if<float>(&rhs._value)) ||
                (std::get_if<double>(&_value) && std::get_if<double>(&rhs._value))) {
                return true;
            } else {
                LogWarn() << "Comparison type mismatch between " << typestr() << " and "
                          << rhs.typestr();
                return false;
            }
        }

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

        bool operator==(const std::string& value_str) const
        {
            // LogDebug() << "Compare " << value_str() << " and " << rhs.value_str();
            if (std::get_if<uint8_t>(&_value)) {
                return std::get<uint8_t>(_value) == std::stoi(value_str);
            } else if (std::get_if<int8_t>(&_value)) {
                return std::get<int8_t>(_value) == std::stoi(value_str);
            } else if (std::get_if<uint16_t>(&_value)) {
                return std::get<uint16_t>(_value) == std::stoi(value_str);
            } else if (std::get_if<int16_t>(&_value)) {
                return std::get<int16_t>(_value) == std::stoi(value_str);
            } else if (std::get_if<uint32_t>(&_value)) {
                return std::get<uint32_t>(_value) == std::stoul(value_str);
            } else if (std::get_if<int32_t>(&_value)) {
                return std::get<int32_t>(_value) == std::stol(value_str);
            } else if (std::get_if<uint64_t>(&_value)) {
                return std::get<uint64_t>(_value) == std::stoull(value_str);
            } else if (std::get_if<int64_t>(&_value)) {
                return std::get<int64_t>(_value) == std::stoll(value_str);
            } else if (std::get_if<float>(&_value)) {
                return std::get<float>(_value) == std::stof(value_str);
            } else if (std::get_if<double>(&_value)) {
                return std::get<double>(_value) == std::stod(value_str);
            } else {
                // This also covers custom_type_t
                return false;
            }
        }

        [[nodiscard]] std::string typestr() const
        {
            if (std::get_if<uint8_t>(&_value)) {
                return "uint8_t";
            } else if (std::get_if<int8_t>(&_value)) {
                return "int8_t";
            } else if (std::get_if<uint16_t>(&_value)) {
                return "uint16_t";
            } else if (std::get_if<int16_t>(&_value)) {
                return "int16_t";
            } else if (std::get_if<uint32_t>(&_value)) {
                return "uint32_t";
            } else if (std::get_if<int32_t>(&_value)) {
                return "int32_t";
            } else if (std::get_if<uint64_t>(&_value)) {
                return "uint64_t";
            } else if (std::get_if<int64_t>(&_value)) {
                return "int64_t";
            } else if (std::get_if<float>(&_value)) {
                return "float";
            } else if (std::get_if<double>(&_value)) {
                return "double";
            }
            // FIXME: Added to fix CI error (control reading end of non-void function)
            return "unknown";
        }

    private:
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
