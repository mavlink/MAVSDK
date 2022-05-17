#include "failure_impl.h"

namespace mavsdk {

FailureImpl::FailureImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

FailureImpl::FailureImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

FailureImpl::~FailureImpl()
{
    _system_impl->unregister_plugin(this);
}

void FailureImpl::init() {}

void FailureImpl::deinit() {}

void FailureImpl::enable()
{
    constexpr auto param_name = "SYS_FAILURE_EN";

    _system_impl->get_param_int_async(
        param_name,
        [this](MavlinkParameterClient::Result result, int32_t value) {
            if (result == MavlinkParameterClient::Result::Success) {
                if (value == 1) {
                    _enabled = EnabledState::Enabled;
                } else if (value == 0) {
                    _enabled = EnabledState::Disabled;
                } else {
                    _enabled = EnabledState::Unknown;
                }
            } else {
                _enabled = EnabledState::Unknown;
            }
        },
        this);

    _system_impl->subscribe_param_int(
        param_name,
        [this](int value) {
            if (value == 1) {
                _enabled = EnabledState::Enabled;
            } else if (value == 0) {
                _enabled = EnabledState::Disabled;
            } else {
                _enabled = EnabledState::Unknown;
            }
        },
        this);
}

void FailureImpl::disable()
{
    _enabled = EnabledState::Init;
}

Failure::Result FailureImpl::inject(
    Failure::FailureUnit failure_unit, Failure::FailureType failure_type, int32_t instance)
{
    while (_enabled == EnabledState::Init) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // If the param is unknown we ignore it and try anyway.
    if (_enabled == EnabledState::Disabled) {
        return Failure::Result::Disabled;
    }

    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_INJECT_FAILURE;
    command.params.maybe_param1 = failure_unit_to_mavlink_enum(failure_unit);
    command.params.maybe_param2 = failure_type_to_mavlink_enum(failure_type);
    command.params.maybe_param3 = static_cast<float>(instance);
    command.target_component_id = _system_impl->get_autopilot_id();

    return failure_result_from_command_result(_system_impl->send_command(command));
}

float FailureImpl::failure_unit_to_mavlink_enum(const Failure::FailureUnit& failure_unit)
{
    switch (failure_unit) {
        case Failure::FailureUnit::SensorGyro:
            return FAILURE_UNIT_SENSOR_GYRO;
        case Failure::FailureUnit::SensorAccel:
            return FAILURE_UNIT_SENSOR_ACCEL;
        case Failure::FailureUnit::SensorMag:
            return FAILURE_UNIT_SENSOR_MAG;
        case Failure::FailureUnit::SensorBaro:
            return FAILURE_UNIT_SENSOR_BARO;
        case Failure::FailureUnit::SensorGps:
            return FAILURE_UNIT_SENSOR_GPS;
        case Failure::FailureUnit::SensorOpticalFlow:
            return FAILURE_UNIT_SENSOR_OPTICAL_FLOW;
        case Failure::FailureUnit::SensorVio:
            return FAILURE_UNIT_SENSOR_VIO;
        case Failure::FailureUnit::SensorDistanceSensor:
            return FAILURE_UNIT_SENSOR_DISTANCE_SENSOR;
        case Failure::FailureUnit::SensorAirspeed:
            return FAILURE_UNIT_SENSOR_AIRSPEED;
        case Failure::FailureUnit::SystemBattery:
            return FAILURE_UNIT_SYSTEM_BATTERY;
        case Failure::FailureUnit::SystemMotor:
            return FAILURE_UNIT_SYSTEM_MOTOR;
        case Failure::FailureUnit::SystemServo:
            return FAILURE_UNIT_SYSTEM_SERVO;
        case Failure::FailureUnit::SystemAvoidance:
            return FAILURE_UNIT_SYSTEM_AVOIDANCE;
        case Failure::FailureUnit::SystemRcSignal:
            return FAILURE_UNIT_SYSTEM_RC_SIGNAL;
        case Failure::FailureUnit::SystemMavlinkSignal:
            return FAILURE_UNIT_SYSTEM_MAVLINK_SIGNAL;
        default:
            return -1;
    }
}

float FailureImpl::failure_type_to_mavlink_enum(const Failure::FailureType& failure_type)
{
    switch (failure_type) {
        case Failure::FailureType::Ok:
            return FAILURE_TYPE_OK;
        case Failure::FailureType::Off:
            return FAILURE_TYPE_OFF;
        case Failure::FailureType::Stuck:
            return FAILURE_TYPE_STUCK;
        case Failure::FailureType::Garbage:
            return FAILURE_TYPE_GARBAGE;
        case Failure::FailureType::Wrong:
            return FAILURE_TYPE_WRONG;
        case Failure::FailureType::Slow:
            return FAILURE_TYPE_SLOW;
        case Failure::FailureType::Delayed:
            return FAILURE_TYPE_DELAYED;
        case Failure::FailureType::Intermittent:
            return FAILURE_TYPE_INTERMITTENT;
        default:
            return -1;
    }
}

Failure::Result
FailureImpl::failure_result_from_command_result(MavlinkCommandSender::Result command_result)
{
    switch (command_result) {
        case MavlinkCommandSender::Result::Success:
            return Failure::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return Failure::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            return Failure::Result::ConnectionError;
        case MavlinkCommandSender::Result::Denied:
            // Fallthrough
        case MavlinkCommandSender::Result::TemporarilyRejected:
            return Failure::Result::Denied;
        case MavlinkCommandSender::Result::Unsupported:
            return Failure::Result::Unsupported;
        case MavlinkCommandSender::Result::Timeout:
            return Failure::Result::Timeout;
        default:
            return Failure::Result::Unknown;
    }
}

} // namespace mavsdk
