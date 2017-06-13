#pragma once

#include "global_include.h"
#include "mavlink_include.h"
#include "locked_queue.h"
#include <cstdint>
#include <string>
#include <functional>

namespace dronelink {

class DeviceImpl;

class MavlinkParameters
{
public:
    explicit MavlinkParameters(DeviceImpl *parent);
    ~MavlinkParameters();

    class ParamValue
    {
    public:
        ParamValue() :
            _float_value(NAN),
            _int_value(0),
            _type(Type::UNKNOWN)
        {}

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

        void set_int(int value)
        {
            _int_value = value;
            _type = Type::INT;
        }

        void set_from_mavlink_param_value(mavlink_param_value_t mavlink_value)
        {
            switch (mavlink_value.param_type) {
                case MAV_PARAM_TYPE_INT32:
                    set_int(*(reinterpret_cast<const int *>(&mavlink_value.param_value)));
                    break;
                case MAV_PARAM_TYPE_REAL32:
                    set_float(mavlink_value.param_value);
                    break;
                default:
                    // This would be worrying
                    Debug() << "Error: unknown mavlink param type";
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

    private:
        float _float_value;
        int32_t _int_value;
        Type _type;
    };

    typedef std::function <void(bool success)> set_param_callback_t;
    void set_param_async(const std::string &name, const ParamValue &value,
                         set_param_callback_t callback);

    typedef std::function <void(bool success, ParamValue value)> get_param_callback_t;
    void get_param_async(const std::string &name, get_param_callback_t callback);

    //void save_async();
    void do_work();

    // Non-copyable
    MavlinkParameters(const MavlinkParameters &) = delete;
    const MavlinkParameters &operator=(const MavlinkParameters &) = delete;
private:
    void process_param_value(const mavlink_message_t &message);
    void receive_timeout();

    DeviceImpl *_parent;

    // TODO: lock this
    enum class RequestState {
        NONE,
        SET_PARAM_BUSY,
        GET_PARAM_BUSY
    } _request_state;

    // Params can be up to 16 chars and without 0-termination.
    // Therefore we add a 0 here for storing.
    static constexpr size_t PARAM_ID_LEN = 16 + 1;

    struct SetParamWork {
        SetParamWork() :
            callback(nullptr),
            param_name(),
            param_value()
        {}

        set_param_callback_t callback;
        std::string param_name;
        ParamValue param_value;
    };

    LockedQueue<SetParamWork> _set_param_queue;

    struct GetParamWork {
        GetParamWork() :
            callback(nullptr),
            param_name()
        {}

        get_param_callback_t callback;
        std::string param_name;
    };
    LockedQueue<GetParamWork> _get_param_queue;
};

} // namespace dronelink
