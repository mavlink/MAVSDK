#include "gimbal_protocol_v2.h"
#include "gimbal_impl.h"
#include "global_include.h"
#include <functional>
#include <cmath>

namespace mavsdk {

GimbalProtocolV2::GimbalProtocolV2(
    SystemImpl& system_impl, const mavlink_gimbal_manager_information_t& information) :
    GimbalProtocolBase(system_impl)
{
    set_gimbal_information(information);
}

void GimbalProtocolV2::set_gimbal_information(
    const mavlink_gimbal_manager_information_t& information)
{
    _gimbal_device_id = information.gimbal_device_id;
}

void GimbalProtocolV2::process_gimbal_manager_status(const mavlink_message_t& message)
{
    Gimbal::ControlMode new_control_mode;
    mavlink_gimbal_manager_status_t gimbal_manager_status;
    mavlink_msg_gimbal_manager_status_decode(&message, &gimbal_manager_status);

    if (static_cast<int>(gimbal_manager_status.primary_control_sysid) == static_cast<int>(_system_impl.get_own_system_id())
            && static_cast<int>(gimbal_manager_status.primary_control_compid) == static_cast<int>(_system_impl.get_own_component_id())) {
        new_control_mode = Gimbal::ControlMode::Primary;
    } else if (static_cast<int>(gimbal_manager_status.secondary_control_sysid) == static_cast<int>(_system_impl.get_own_system_id())
            && static_cast<int>(gimbal_manager_status.secondary_control_compid) == static_cast<int>(_system_impl.get_own_component_id())) {
        new_control_mode = Gimbal::ControlMode::Secondary;
    } else {
        new_control_mode = Gimbal::ControlMode::None;
    }

    _current_control_mode = new_control_mode;

    if (_control_callback) {
        auto temp_callback = _control_callback;
        _system_impl.call_user_callback([temp_callback, new_control_mode]() { temp_callback(new_control_mode); });
    }
}

Gimbal::Result GimbalProtocolV2::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    const float roll_rad = 0.0f;
    const float pitch_rad = to_rad_from_deg(pitch_deg);
    const float yaw_rad = to_rad_from_deg(yaw_deg);

    float quaternion[4];
    mavlink_euler_to_quaternion(roll_rad, pitch_rad, yaw_rad, quaternion);

    const uint32_t flags =
        GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK |
        ((_gimbal_mode == Gimbal::GimbalMode::YawLock) ? GIMBAL_MANAGER_FLAGS_YAW_LOCK : 0);

    mavlink_message_t message;
    mavlink_msg_gimbal_manager_set_attitude_pack(
        _system_impl.get_own_system_id(),
        _system_impl.get_own_component_id(),
        &message,
        _system_impl.get_system_id(),
        _system_impl.get_autopilot_id(), // FIXME: this is hard-coded to autopilot for now
        flags,
        _gimbal_device_id,
        quaternion,
        NAN,
        NAN,
        NAN);

    return _system_impl.send_message(message) ? Gimbal::Result::Success : Gimbal::Result::Error;
}

void GimbalProtocolV2::set_pitch_and_yaw_async(
    float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    // Sending the message should be quick and we can just do that straighaway.
    Gimbal::Result result = set_pitch_and_yaw(pitch_deg, yaw_deg);

    if (callback) {
        _system_impl.call_user_callback([callback, result]() { callback(result); });
    }
}

Gimbal::Result GimbalProtocolV2::set_mode(const Gimbal::GimbalMode gimbal_mode)
{
    _gimbal_mode = gimbal_mode;
    return Gimbal::Result::Success;
}

void GimbalProtocolV2::set_mode_async(
    const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback)
{
    _gimbal_mode = gimbal_mode;

    if (callback) {
        _system_impl.call_user_callback([callback]() { callback(Gimbal::Result::Success); });
    }
}

Gimbal::Result
GimbalProtocolV2::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    UNUSED(latitude_deg);
    UNUSED(longitude_deg);
    UNUSED(altitude_m);

    return Gimbal::Result::Unsupported;
}

void GimbalProtocolV2::set_roi_location_async(
    double latitude_deg, double longitude_deg, float altitude_m, Gimbal::ResultCallback callback)
{
    UNUSED(latitude_deg);
    UNUSED(longitude_deg);
    UNUSED(altitude_m);

    if (callback) {
        _system_impl.call_user_callback(
            [callback]() { callback(Gimbal::Result::Unsupported); });
    }
}

Gimbal::Result GimbalProtocolV2::take_control(Gimbal::ControlMode control_mode)
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    take_control_async(control_mode, [&prom](Gimbal::Result result) { prom.set_value(result); });

    return fut.get();
}

void GimbalProtocolV2::take_control_async(Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback)
{
    if (control_mode == Gimbal::ControlMode::None) {
        release_control_async(callback);
        return;
    }
    else if (control_mode == Gimbal::ControlMode::Secondary) {
        _system_impl.call_user_callback([callback]() { callback(Gimbal::Result::Unsupported); });
        LogErr() << "Gimbal secondary control is not implemented yet!";
        return;
    }

    float own_sysid = _system_impl.get_own_system_id();
    float own_compid = _system_impl.get_own_component_id();

    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_CONFIGURE;
    command.params.param1 = control_mode == Gimbal::ControlMode::Primary ? own_sysid : -3.0f; // sysid primary control
    command.params.param2 = control_mode == Gimbal::ControlMode::Primary ? own_compid : -3.0f; // compid primary control
    command.params.param3 = control_mode == Gimbal::ControlMode::Primary ? own_sysid : -3.0f; // sysid secondary control
    command.params.param4 = control_mode == Gimbal::ControlMode::Primary ? own_compid : -3.0f; // compid secondary control
    command.params.param5 = 0.0f; // gimbal device id
    command.target_component_id = _system_impl.get_autopilot_id(), // FIXME: this is hard-coded to autopilot for now

    _system_impl.send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            GimbalImpl::receive_command_result(result, callback);
        });
}

Gimbal::Result GimbalProtocolV2::release_control()
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    release_control_async([&prom](Gimbal::Result result) { prom.set_value(result); });

    return fut.get();
}

void GimbalProtocolV2::release_control_async(Gimbal::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_CONFIGURE;
    command.params.param1 = -3.0f; // sysid primary control
    command.params.param2 = -3.0f; // compid primary control
    command.params.param3 = -3.0f; // sysid secondary control
    command.params.param4 = -3.0f; // compid secondary control
    command.params.param5 = 0.0f; // gimbal device id
    command.target_component_id = _system_impl.get_autopilot_id(), // FIXME: this is hard-coded to autopilot for now

    _system_impl.send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            GimbalImpl::receive_command_result(result, callback);
        });
}

Gimbal::ControlMode GimbalProtocolV2::control()
{
    auto prom = std::promise<Gimbal::ControlMode>();
    auto fut = prom.get_future();

    control_async([&prom](Gimbal::ControlMode control_mode) { prom.set_value(control_mode); });

    return fut.get();
}

void GimbalProtocolV2::control_async(Gimbal::ControlCallback callback)
{
    if (!_is_mavlink_manager_status_registered) {
        _is_mavlink_manager_status_registered = true;

        _system_impl.register_mavlink_message_handler(
            MAVLINK_MSG_ID_GIMBAL_MANAGER_STATUS,
            std::bind(&GimbalProtocolV2::process_gimbal_manager_status, this, std::placeholders::_1),
            this);
    }

    _control_callback = callback;
    _system_impl.call_user_callback([this, callback]() { callback(_current_control_mode); });
}

} // namespace mavsdk
