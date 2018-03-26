#pragma once

#include "log.h"
#include "global_include.h"
#include "mavlink_include.h"
#include "locked_queue.h"
#include "any.h"
#include <cstdint>
#include <string>
#include <functional>
#include <cstring> // for memcpy
#include <cassert>

namespace dronecore {

class MAVLinkSystem;

class MAVLinkParameters
{
public:
    explicit MAVLinkParameters(MAVLinkSystem &parent);
    ~MAVLinkParameters();

    class ParamValue
    {
    public:
        typedef char custom_type_t[128];

        void set_from_mavlink_param_value(mavlink_param_value_t mavlink_value)
        {
            switch (mavlink_value.param_type) {
                case MAV_PARAM_TYPE_UINT32:
                // FALLTHROUGH
                case MAV_PARAM_TYPE_INT32: {
                        int32_t temp;
                        memcpy(&temp, &mavlink_value.param_value, sizeof(temp));
                        _value = temp;
                    }
                    break;
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
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_INT8: {
                        int8_t temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_UINT16: {
                        uint16_t temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_INT16: {
                        int16_t temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_UINT32: {
                        uint32_t temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_INT32: {
                        int32_t temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_UINT64: {
                        uint64_t temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_INT64: {
                        int64_t temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_REAL32: {
                        float temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_REAL64: {
                        double temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                case MAV_PARAM_EXT_TYPE_CUSTOM: {
                        custom_type_t temp;
                        memcpy(&temp, &mavlink_ext_value.param_value[0], sizeof(temp));
                        _value = temp;
                    }
                    break;
                default:
                    // This would be worrying
                    LogErr() << "Error: unknown mavlink ext param type";
                    assert(false);
                    break;
            }
        }

        MAV_PARAM_TYPE get_mav_param_type() const
        {
            if (_value.is<float>()) {
                return MAV_PARAM_TYPE_REAL32;
            } else if (_value.is<int32_t>()) {
                return MAV_PARAM_TYPE_INT32;
            } else {
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
            }
        }

        float get_4_float_bytes() const
        {
            if (_value.is<float>()) {
                return _value.as<float>();
            } else {
                return *(reinterpret_cast<const float *>(&_value.as<int32_t>()));
            }
        }

        void get_128_bytes(char *bytes) const
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

        float get_float() const
        {
            return float(_value);
        }

        double get_double() const
        {
            return double(_value);
        }

        int32_t get_int() const
        {
            return int32_t(_value);
        }

        void set_float(float value)
        {
            _value = value;
        }

        void set_double(double value)
        {
            _value = value;
        }

        void set_int(int32_t value)
        {
            _value = value;
        }

        bool is_int() const
        {
            return (_value.is<int32_t>());
        }

        bool is_float() const
        {
            return (_value.is<float>());
        }

        bool is_double() const
        {
            return (_value.is<double>());
        }

    private:
        Any _value;
    };

    typedef std::function <void(bool success)> set_param_callback_t;
    void set_param_async(const std::string &name, const ParamValue &value,
                         set_param_callback_t callback, bool extended = false);

    typedef std::function <void(bool success, ParamValue value)> get_param_callback_t;
    void get_param_async(const std::string &name, get_param_callback_t callback, bool extended = false);

    //void save_async();
    void do_work();

    // Non-copyable
    MAVLinkParameters(const MAVLinkParameters &) = delete;
    const MAVLinkParameters &operator=(const MAVLinkParameters &) = delete;
private:
    void process_param_value(const mavlink_message_t &message);
    void process_param_ext_value(const mavlink_message_t &message);
    void process_param_ext_ack(const mavlink_message_t &message);
    void receive_timeout();

    MAVLinkSystem &_parent;

    enum class State {
        NONE,
        SET_PARAM_BUSY,
        GET_PARAM_BUSY
    } _state = State::NONE;
    std::mutex _state_mutex {};

    // Params can be up to 16 chars and without 0-termination.
    // Therefore we add a 0 here for storing.
    static constexpr size_t PARAM_ID_LEN = 16 + 1;

    struct SetParamWork {
        set_param_callback_t callback = nullptr;
        std::string param_name {};
        ParamValue param_value {};
        bool extended = false;
        int retries_done = 0;
    };

    LockedQueue<SetParamWork> _set_param_queue {};

    struct GetParamWork {
        get_param_callback_t callback = nullptr;
        std::string param_name {};
        bool extended = false;
        int retries_done = 0;
    };
    LockedQueue<GetParamWork> _get_param_queue {};

    void *_timeout_cookie = nullptr;

    // dl_time_t _last_request_time = {};
};

} // namespace dronecore
