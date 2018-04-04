#pragma once

#include "log.h"
#include "global_include.h"
#include "mavlink_include.h"
#include "locked_queue.h"
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
        enum class Type {
            UNKNOWN,
            FLOAT,
            INT
        };

        void set_float(float value)
        {
            _float_value = value;
            _type = Type::FLOAT;
        }

        void set_int(int32_t value)
        {
            _int_value = value;
            _type = Type::INT;
        }

        void set_from_mavlink_param_value(mavlink_param_value_t mavlink_value)
        {
            switch (mavlink_value.param_type) {
                case MAV_PARAM_TYPE_UINT32:
                // FALLTHROUGH
                case MAV_PARAM_TYPE_INT32: {
                        int32_t temp_int;
                        memcpy(&temp_int, &mavlink_value.param_value, sizeof(int32_t));
                        set_int(temp_int);
                    }
                    break;
                case MAV_PARAM_TYPE_REAL32:
                    set_float(mavlink_value.param_value);
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
                // FIXME: the camera params are others but we treat them as INT32.
                case MAV_PARAM_TYPE_UINT8:
                // FALLTHROUGH
                case MAV_PARAM_TYPE_INT8:
                // FALLTHROUGH
                case MAV_PARAM_TYPE_UINT16:
                // FALLTHROUGH
                case MAV_PARAM_TYPE_INT16:
                // FALLTHROUGH
                case MAV_PARAM_TYPE_UINT32:
                // FALLTHROUGH
                case MAV_PARAM_TYPE_INT32: {
                        int32_t temp_int;
                        memcpy(&temp_int, &mavlink_ext_value.param_value[0], sizeof(int32_t));
                        set_int(temp_int);
                    }
                    break;
                case MAV_PARAM_TYPE_REAL32:
                    float temp_float;
                    memcpy(&temp_float, &mavlink_ext_value.param_value[0], sizeof(float));
                    set_float(temp_float);
                    break;
                default:
                    // This would be worrying
                    LogErr() << "Error: unknown mavlink ext param type";
                    assert(false);
                    break;
            }
        }

        float get_float_casted_value() const
        {
            switch (_type) {
                case Type::FLOAT:
                    return _float_value;
                case Type::INT:
                    return *(reinterpret_cast<const float *>(&_int_value));
                default:
                    return NAN;
            }
        }

        MAV_PARAM_TYPE get_mav_param_type() const
        {
            switch (_type) {
                case Type::FLOAT:
                    return MAV_PARAM_TYPE_REAL32;
                case Type::INT:
                    return MAV_PARAM_TYPE_INT32;
                default:
                    return MAV_PARAM_TYPE_REAL32;
            }
        }

        float get_float() const
        {
            return _float_value;
        }

        int32_t get_int() const
        {
            return _int_value;
        }

        bool is_float() const
        {
            return (_type == Type::FLOAT);
        }

    private:
        float _float_value = NAN;
        int32_t _int_value = 0;
        Type _type = Type::UNKNOWN;
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
