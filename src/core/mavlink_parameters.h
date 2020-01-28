#pragma once

#include "log.h"
#include "global_include.h"
#include "mavlink_include.h"
#include "locked_queue.h"
#include "any.h"
#include <cstdint>
#include <string>
#include <functional>
#include <cassert>
#include <map>

namespace mavsdk {

class SystemImpl;

class MAVLinkParameters {
public:
    explicit MAVLinkParameters(SystemImpl& parent);
    ~MAVLinkParameters();

    class ParamValue {
    public:
        typedef char custom_type_t[128];

        ParamValue() {}

        ParamValue(ParamValue& rhs) { _value = rhs._value; }

        ParamValue(const ParamValue& rhs) { _value = rhs._value; }

        ParamValue& operator=(ParamValue rhs)
        {
            _value = rhs._value;
            return *this;
        }

        void set_from_mavlink_param_value(mavlink_param_value_t mavlink_value)
        {
            switch (mavlink_value.param_type) {
                case MAV_PARAM_TYPE_UINT32:
                // FALLTHROUGH
                case MAV_PARAM_TYPE_INT32: {
                    int32_t temp;
                    memcpy(&temp, &mavlink_value.param_value, sizeof(temp));
                    _value = temp;
                } break;
                case MAV_PARAM_TYPE_REAL32:
                    float temp;
                    memcpy(&temp, &mavlink_value.param_value, sizeof(temp));
                    _value = temp;
                    break;
                default:
                    // This would be worrying
                    LogErr() << "Error: unknown mavlink param type";
                    break;
            }
        }

        void set_from_mavlink_param_ext_value(mavlink_param_ext_value_t mavlink_ext_value)
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
                case MAV_PARAM_EXT_TYPE_CUSTOM: {
                    custom_type_t temp;
                    memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                    _value = temp;
                } break;
                default:
                    // This would be worrying
                    LogErr() << "Error: unknown mavlink ext param type";
                    assert(false);
                    break;
            }
        }

        bool set_from_xml(const std::string& type_str, const std::string& value_str)
        {
            if (strcmp(type_str.c_str(), "uint8") == 0) {
                uint8_t temp = std::stoi(value_str.c_str());
                _value = temp;
            } else if (strcmp(type_str.c_str(), "int8") == 0) {
                int8_t temp = std::stoi(value_str.c_str());
                _value = temp;
            } else if (strcmp(type_str.c_str(), "uint16") == 0) {
                uint16_t temp = std::stoi(value_str.c_str());
                _value = temp;
            } else if (strcmp(type_str.c_str(), "int16") == 0) {
                int16_t temp = std::stoi(value_str.c_str());
                _value = temp;
            } else if (strcmp(type_str.c_str(), "uint32") == 0) {
                uint32_t temp = std::stoi(value_str.c_str());
                _value = temp;
            } else if (strcmp(type_str.c_str(), "int32") == 0) {
                int32_t temp = std::stoi(value_str.c_str());
                _value = temp;
            } else if (strcmp(type_str.c_str(), "uint64") == 0) {
                uint64_t temp = std::stoll(value_str.c_str());
                _value = temp;
            } else if (strcmp(type_str.c_str(), "int64") == 0) {
                int64_t temp = std::stoll(value_str.c_str());
                _value = temp;
            } else if (strcmp(type_str.c_str(), "float") == 0) {
                float temp = std::stof(value_str.c_str());
                _value = temp;
            } else if (strcmp(type_str.c_str(), "double") == 0) {
                double temp = std::stod(value_str.c_str());
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

        MAV_PARAM_TYPE get_mav_param_type() const
        {
            if (_value.is<float>()) {
                return MAV_PARAM_TYPE_REAL32;
            } else if (_value.is<int32_t>()) {
                return MAV_PARAM_TYPE_INT32;
            } else {
                LogErr() << "Unknown param type sent";
                return MAV_PARAM_TYPE_REAL32;
            }
        }

        MAV_PARAM_EXT_TYPE get_mav_param_ext_type() const
        {
            if (_value.is<uint8_t>()) {
                return MAV_PARAM_EXT_TYPE_UINT8;
            } else if (_value.is<int8_t>()) {
                return MAV_PARAM_EXT_TYPE_INT8;
            } else if (_value.is<uint16_t>()) {
                return MAV_PARAM_EXT_TYPE_UINT16;
            } else if (_value.is<int16_t>()) {
                return MAV_PARAM_EXT_TYPE_INT16;
            } else if (_value.is<uint32_t>()) {
                return MAV_PARAM_EXT_TYPE_UINT32;
            } else if (_value.is<int32_t>()) {
                return MAV_PARAM_EXT_TYPE_INT32;
            } else if (_value.is<uint64_t>()) {
                return MAV_PARAM_EXT_TYPE_UINT64;
            } else if (_value.is<int64_t>()) {
                return MAV_PARAM_EXT_TYPE_INT64;
            } else if (_value.is<float>()) {
                return MAV_PARAM_EXT_TYPE_REAL32;
            } else if (_value.is<double>()) {
                return MAV_PARAM_EXT_TYPE_REAL64;
            } else if (_value.is<custom_type_t>()) {
                return MAV_PARAM_EXT_TYPE_CUSTOM;
            } else {
                LogErr() << "Unknown data type for param.";
                assert(false);
                return MAV_PARAM_EXT_TYPE_INT32;
            }
        }

        bool set_as_same_type(const std::string& value_str)
        {
            if (_value.is<uint8_t>()) {
                _value = uint8_t(std::stoi(value_str.c_str()));
            } else if (_value.is<int8_t>()) {
                _value = int8_t(std::stoi(value_str.c_str()));
            } else if (_value.is<uint16_t>()) {
                _value = uint16_t(std::stoi(value_str.c_str()));
            } else if (_value.is<int16_t>()) {
                _value = int16_t(std::stoi(value_str.c_str()));
            } else if (_value.is<uint32_t>()) {
                _value = uint32_t(std::stoi(value_str.c_str()));
            } else if (_value.is<int32_t>()) {
                _value = int32_t(std::stoi(value_str.c_str()));
            } else if (_value.is<uint64_t>()) {
                _value = uint64_t(std::stoll(value_str.c_str()));
            } else if (_value.is<int64_t>()) {
                _value = int64_t(std::stoll(value_str.c_str()));
            } else if (_value.is<float>()) {
                _value = float(std::stof(value_str.c_str()));
            } else if (_value.is<double>()) {
                _value = double(std::stod(value_str.c_str()));
            } else {
                LogErr() << "Unknown type";
                return false;
            }
            return true;
        }

        float get_4_float_bytes() const
        {
            if (_value.is<float>()) {
                return _value.as<float>();
            } else {
                return *(reinterpret_cast<const float*>(&_value.as<int32_t>()));
            }
        }

        void get_128_bytes(char* bytes) const
        {
            if (_value.is<uint8_t>()) {
                memcpy(bytes, &_value.as<uint8_t>(), sizeof(uint8_t));
            } else if (_value.is<int8_t>()) {
                memcpy(bytes, &_value.as<int8_t>(), sizeof(int8_t));
            } else if (_value.is<uint16_t>()) {
                memcpy(bytes, &_value.as<uint16_t>(), sizeof(uint16_t));
            } else if (_value.is<int16_t>()) {
                memcpy(bytes, &_value.as<int16_t>(), sizeof(int16_t));
            } else if (_value.is<uint32_t>()) {
                memcpy(bytes, &_value.as<uint32_t>(), sizeof(uint32_t));
            } else if (_value.is<int32_t>()) {
                memcpy(bytes, &_value.as<int32_t>(), sizeof(int32_t));
            } else if (_value.is<uint64_t>()) {
                memcpy(bytes, &_value.as<uint64_t>(), sizeof(uint64_t));
            } else if (_value.is<int64_t>()) {
                memcpy(bytes, &_value.as<int64_t>(), sizeof(int64_t));
            } else if (_value.is<float>()) {
                memcpy(bytes, &_value.as<float>(), sizeof(float));
            } else if (_value.is<double>()) {
                memcpy(bytes, &_value.as<double>(), sizeof(double));
            } else if (_value.is<custom_type_t>()) {
                memcpy(bytes, &_value.as<custom_type_t>(), sizeof(custom_type_t));
            } else {
                LogErr() << "Unknown data type for param.";
                assert(false);
            }
        }

        std::string get_string() const
        {
            if (_value.is<uint8_t>()) {
                return std::to_string(_value.as<uint8_t>());
            } else if (_value.is<int8_t>()) {
                return std::to_string(_value.as<int8_t>());
            } else if (_value.is<uint16_t>()) {
                return std::to_string(_value.as<uint16_t>());
            } else if (_value.is<int16_t>()) {
                return std::to_string(_value.as<int16_t>());
            } else if (_value.is<uint32_t>()) {
                return std::to_string(_value.as<uint32_t>());
            } else if (_value.is<int32_t>()) {
                return std::to_string(_value.as<int32_t>());
            } else if (_value.is<uint64_t>()) {
                return std::to_string(_value.as<uint64_t>());
            } else if (_value.is<int64_t>()) {
                return std::to_string(_value.as<int64_t>());
            } else if (_value.is<float>()) {
                return std::to_string(_value.as<float>());
            } else if (_value.is<double>()) {
                return std::to_string(_value.as<double>());
            } else if (_value.is<custom_type_t>()) {
                return std::string("(custom type)");
            } else {
                LogErr() << "Unknown data type for param.";
                assert(false);
                return std::string("(unknown)");
            }
        }
        float get_float() const { return float(_value); }

        double get_double() const { return double(_value); }

        int8_t get_int8() const { return int8_t(_value); }

        uint8_t get_uint8() const { return uint8_t(_value); }

        int16_t get_int16() const { return int16_t(_value); }

        uint16_t get_uint16() const { return uint16_t(_value); }

        int32_t get_int32() const { return int32_t(_value); }

        uint32_t get_uint32() const { return uint32_t(_value); }

        void set_float(float value) { _value = value; }

        void set_double(double value) { _value = value; }

        void set_int8(int8_t value) { _value = value; }

        void set_uint8(uint8_t value) { _value = value; }

        void set_int16(int16_t value) { _value = value; }

        void set_uint16(uint16_t value) { _value = value; }

        void set_int32(int32_t value) { _value = value; }

        void set_uint32(uint32_t value) { _value = value; }

        void set_int64(int64_t value) { _value = value; }

        void set_uint64(uint64_t value) { _value = value; }

        bool is_uint8() const { return (_value.is<uint8_t>()); }

        bool is_int8() const { return (_value.is<int8_t>()); }

        bool is_uint16() const { return (_value.is<uint16_t>()); }

        bool is_int16() const { return (_value.is<int16_t>()); }

        bool is_uint32() const { return (_value.is<uint32_t>()); }

        bool is_int32() const { return (_value.is<int32_t>()); }

        bool is_uint64() const { return (_value.is<uint64_t>()); }

        bool is_int64() const { return (_value.is<int64_t>()); }

        bool is_float() const { return (_value.is<float>()); }

        bool is_double() const { return (_value.is<double>()); }

        bool is_same_type(const ParamValue& rhs) const
        {
            if ((_value.is<uint8_t>() && rhs._value.is<uint8_t>()) ||
                (_value.is<int8_t>() && rhs._value.is<int8_t>()) ||
                (_value.is<uint16_t>() && rhs._value.is<uint16_t>()) ||
                (_value.is<int16_t>() && rhs._value.is<int16_t>()) ||
                (_value.is<uint32_t>() && rhs._value.is<uint32_t>()) ||
                (_value.is<int32_t>() && rhs._value.is<int32_t>()) ||
                (_value.is<uint64_t>() && rhs._value.is<uint64_t>()) ||
                (_value.is<int64_t>() && rhs._value.is<int64_t>()) ||
                (_value.is<float>() && rhs._value.is<float>()) ||
                (_value.is<double>() && rhs._value.is<double>()) ||
                (_value.is<custom_type_t>() && rhs._value.is<custom_type_t>())) {
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
            if (_value.is<uint8_t>()) {
                return _value.as<uint8_t>() == rhs._value.as<uint8_t>();
            } else if (_value.is<int8_t>()) {
                return _value.as<int8_t>() == rhs._value.as<int8_t>();
            } else if (_value.is<uint16_t>()) {
                return _value.as<uint16_t>() == rhs._value.as<uint16_t>();
            } else if (_value.is<int16_t>()) {
                return _value.as<int16_t>() == rhs._value.as<int16_t>();
            } else if (_value.is<uint32_t>()) {
                return _value.as<uint32_t>() == rhs._value.as<uint32_t>();
            } else if (_value.is<int32_t>()) {
                return _value.as<int32_t>() == rhs._value.as<int32_t>();
            } else if (_value.is<uint64_t>()) {
                return _value.as<uint64_t>() == rhs._value.as<uint64_t>();
            } else if (_value.is<int64_t>()) {
                return _value.as<int64_t>() == rhs._value.as<int64_t>();
            } else if (_value.is<float>()) {
                return _value.as<float>() == rhs._value.as<float>();
            } else if (_value.is<double>()) {
                return _value.as<double>() == rhs._value.as<double>();
            } else if (_value.is<custom_type_t>()) {
                LogErr() << "Comparing custom_type not supported.";
                return false;
            } else {
                LogErr() << "Comparing unknown types";
                return false;
            }
        }

        bool operator<(const ParamValue& rhs) const
        {
            if (!is_same_type(rhs)) {
                LogWarn() << "Trying to compare different types.";
                return false;
            }
            if (_value.is<uint8_t>()) {
                return _value.as<uint8_t>() < rhs._value.as<uint8_t>();
            } else if (_value.is<int8_t>()) {
                return _value.as<int8_t>() < rhs._value.as<int8_t>();
            } else if (_value.is<uint16_t>()) {
                return _value.as<uint16_t>() < rhs._value.as<uint16_t>();
            } else if (_value.is<int16_t>()) {
                return _value.as<int16_t>() < rhs._value.as<int16_t>();
            } else if (_value.is<uint32_t>()) {
                return _value.as<uint32_t>() < rhs._value.as<uint32_t>();
            } else if (_value.is<int32_t>()) {
                return _value.as<int32_t>() < rhs._value.as<int32_t>();
            } else if (_value.is<uint64_t>()) {
                return _value.as<uint64_t>() < rhs._value.as<uint64_t>();
            } else if (_value.is<int64_t>()) {
                return _value.as<int64_t>() < rhs._value.as<int64_t>();
            } else if (_value.is<float>()) {
                return _value.as<float>() < rhs._value.as<float>();
            } else if (_value.is<double>()) {
                return _value.as<double>() < rhs._value.as<double>();
            } else if (_value.is<custom_type_t>()) {
                LogErr() << "Comparing custom_type not supported.";
                return false;
            } else {
                LogErr() << "Comparing unknown types";
                return false;
            }
        }

        bool operator>(const ParamValue& rhs) const
        {
            if (!is_same_type(rhs)) {
                LogWarn() << "Trying to compare different types.";
                return false;
            }
            if (_value.is<uint8_t>()) {
                return _value.as<uint8_t>() > rhs._value.as<uint8_t>();
            } else if (_value.is<int8_t>()) {
                return _value.as<int8_t>() > rhs._value.as<int8_t>();
            } else if (_value.is<uint16_t>()) {
                return _value.as<uint16_t>() > rhs._value.as<uint16_t>();
            } else if (_value.is<int16_t>()) {
                return _value.as<int16_t>() > rhs._value.as<int16_t>();
            } else if (_value.is<uint32_t>()) {
                return _value.as<uint32_t>() > rhs._value.as<uint32_t>();
            } else if (_value.is<int32_t>()) {
                return _value.as<int32_t>() > rhs._value.as<int32_t>();
            } else if (_value.is<uint64_t>()) {
                return _value.as<uint64_t>() > rhs._value.as<uint64_t>();
            } else if (_value.is<int64_t>()) {
                return _value.as<int64_t>() > rhs._value.as<int64_t>();
            } else if (_value.is<float>()) {
                return _value.as<float>() > rhs._value.as<float>();
            } else if (_value.is<double>()) {
                return _value.as<double>() > rhs._value.as<double>();
            } else if (_value.is<custom_type_t>()) {
                LogErr() << "Comparing custom_type not supported.";
                return false;
            } else {
                LogErr() << "Comparing unknown types";
                return false;
            }
        }

        bool operator==(const std::string& value_str) const
        {
            // LogDebug() << "Compare " << typestr() << " and " << rhs.typestr();
            if (_value.is<uint8_t>()) {
                return _value.as<uint8_t>() == std::stoi(value_str.c_str());
            } else if (_value.is<int8_t>()) {
                return _value.as<int8_t>() == std::stoi(value_str.c_str());
            } else if (_value.is<uint16_t>()) {
                return _value.as<uint16_t>() == std::stoi(value_str.c_str());
            } else if (_value.is<int16_t>()) {
                return _value.as<int16_t>() == std::stoi(value_str.c_str());
            } else if (_value.is<uint32_t>()) {
                return _value.as<uint32_t>() == std::stoul(value_str.c_str());
            } else if (_value.is<int32_t>()) {
                return _value.as<int32_t>() == std::stol(value_str.c_str());
            } else if (_value.is<uint64_t>()) {
                return _value.as<uint64_t>() == std::stoull(value_str.c_str());
            } else if (_value.is<int64_t>()) {
                return _value.as<int64_t>() == std::stoll(value_str.c_str());
            } else if (_value.is<float>()) {
                return _value.as<float>() == std::stof(value_str.c_str());
            } else if (_value.is<double>()) {
                return _value.as<double>() == std::stod(value_str.c_str());
            } else {
                // This also covers custom_type_t
                return false;
            }
        }

        std::string typestr() const
        {
            if (_value.is<uint8_t>()) {
                return "uint8_t";
            } else if (_value.is<int8_t>()) {
                return "int8_t";
            } else if (_value.is<uint16_t>()) {
                return "uint16_t";
            } else if (_value.is<int16_t>()) {
                return "int16_t";
            } else if (_value.is<uint32_t>()) {
                return "uint32_t";
            } else if (_value.is<int32_t>()) {
                return "int32_t";
            } else if (_value.is<uint64_t>()) {
                return "uint64_t";
            } else if (_value.is<int64_t>()) {
                return "int64_t";
            } else if (_value.is<float>()) {
                return "float";
            } else if (_value.is<double>()) {
                return "double";
            } else if (_value.is<custom_type_t>()) {
                // FIXME: not clear how to handle this
                return "unknown";
            }
            // FIXME: Added to fix CI error (control reading end of non-void function)
            return "unknown";
        }

    private:
        Any _value{};
    };

    enum class Result { SUCCESS, TIMEOUT, CONNECTION_ERROR, WRONG_TYPE, PARAM_NAME_TOO_LONG };

    typedef std::function<void(Result result)> set_param_callback_t;

    Result set_param(const std::string& name, const ParamValue& value, bool extended = false);

    void set_param_async(
        const std::string& name,
        const ParamValue& value,
        set_param_callback_t callback,
        const void* cookie = nullptr,
        bool extended = false);

    std::pair<Result, ParamValue>
    get_param(const std::string& name, ParamValue value_type, bool extended);
    typedef std::function<void(Result, ParamValue value)> get_param_callback_t;
    void get_param_async(
        const std::string& name,
        ParamValue value_type,
        get_param_callback_t callback,
        const void* cookie,
        bool extended = false);

    void cancel_all_param(const void* cookie);

    void do_work();

    friend std::ostream& operator<<(std::ostream&, const ParamValue&);

    // Non-copyable
    MAVLinkParameters(const MAVLinkParameters&) = delete;
    const MAVLinkParameters& operator=(const MAVLinkParameters&) = delete;

private:
    void process_param_value(const mavlink_message_t& message);
    void process_param_ext_value(const mavlink_message_t& message);
    void process_param_ext_ack(const mavlink_message_t& message);
    void receive_timeout();

    static std::string extract_safe_param_id(const char param_id[]);

    SystemImpl& _parent;

    // Params can be up to 16 chars without 0-termination.
    static constexpr size_t PARAM_ID_LEN = 16;

    struct WorkItem {
        enum class Type { Get, Set } type{Type::Get};
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
        double timeout_s{1.0};
        mavlink_message_t mavlink_message{};
    };
    LockedQueue<WorkItem> _work_queue{};

    void* _timeout_cookie = nullptr;

    // dl_time_t _last_request_time = {};
};

} // namespace mavsdk
