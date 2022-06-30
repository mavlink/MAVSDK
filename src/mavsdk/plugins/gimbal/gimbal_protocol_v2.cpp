#include "gimbal_protocol_v2.h"
#include "gimbal_impl.h"
#include "mavsdk_math.h"
#include <functional>
#include <cmath>

namespace mavsdk {

GimbalProtocolV2::GimbalProtocolV2(
    SystemImpl& system_impl,
    const mavlink_gimbal_manager_information_t& information,
    uint8_t gimbal_manager_sysid,
    uint8_t gimbal_manager_compid) :
    GimbalProtocolBase(system_impl),
    _gimbal_device_id(information.gimbal_device_id),
    _gimbal_manager_sysid(gimbal_manager_sysid),
    _gimbal_manager_compid(gimbal_manager_compid)
{}

void GimbalProtocolV2::process_gimbal_manager_status(const mavlink_message_t& message)
{
    Gimbal::ControlMode new_control_mode;
    mavlink_gimbal_manager_status_t gimbal_manager_status;
    mavlink_msg_gimbal_manager_status_decode(&message, &gimbal_manager_status);

    const int primary_control_sysid = gimbal_manager_status.primary_control_sysid;
    const int primary_control_compid = gimbal_manager_status.primary_control_compid;
    const int secondary_control_sysid = gimbal_manager_status.secondary_control_sysid;
    const int secondary_control_compid = gimbal_manager_status.secondary_control_compid;

    if (primary_control_sysid == static_cast<int>(_system_impl.get_own_system_id()) &&
        primary_control_compid == static_cast<int>(_system_impl.get_own_component_id())) {
        new_control_mode = Gimbal::ControlMode::Primary;
    } else if (
        secondary_control_sysid == static_cast<int>(_system_impl.get_own_system_id()) &&
        secondary_control_compid == static_cast<int>(_system_impl.get_own_component_id())) {
        new_control_mode = Gimbal::ControlMode::Secondary;
    } else {
        new_control_mode = Gimbal::ControlMode::None;
    }

    _current_control_status.control_mode = new_control_mode;
    _current_control_status.sysid_primary_control = primary_control_sysid;
    _current_control_status.compid_primary_control = primary_control_compid;
    _current_control_status.sysid_secondary_control = secondary_control_sysid;
    _current_control_status.compid_secondary_control = secondary_control_compid;

    if (_control_callback) {
        // The queue is called outside of this class.
        _control_callback(_current_control_status);
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
        _gimbal_manager_sysid,
        _gimbal_manager_compid,
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

Gimbal::Result
GimbalProtocolV2::set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s)
{
    const uint32_t flags =
        GIMBAL_MANAGER_FLAGS_ROLL_LOCK | GIMBAL_MANAGER_FLAGS_PITCH_LOCK |
        ((_gimbal_mode == Gimbal::GimbalMode::YawLock) ? GIMBAL_MANAGER_FLAGS_YAW_LOCK : 0);

    const float quaternion[4] = {NAN, NAN, NAN, NAN};

    mavlink_message_t message;
    mavlink_msg_gimbal_manager_set_attitude_pack(
        _system_impl.get_own_system_id(),
        _system_impl.get_own_component_id(),
        &message,
        _gimbal_manager_sysid,
        _gimbal_manager_compid,
        flags,
        _gimbal_device_id,
        quaternion,
        0.0f,
        to_rad_from_deg(pitch_rate_deg_s),
        to_rad_from_deg(yaw_rate_deg_s));

    return _system_impl.send_message(message) ? Gimbal::Result::Success : Gimbal::Result::Error;
}

void GimbalProtocolV2::set_pitch_rate_and_yaw_rate_async(
    float pitch_rate_deg_s, float yaw_rate_deg_s, Gimbal::ResultCallback callback)
{
    // Sending the message should be quick and we can just do that straighaway.
    Gimbal::Result result = set_pitch_rate_and_yaw_rate(pitch_rate_deg_s, yaw_rate_deg_s);

    if (callback) {
        auto temp_callback = callback;
        _system_impl.call_user_callback([temp_callback, result]() { temp_callback(result); });
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
    MavlinkCommandSender::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = static_cast<int32_t>(std::round(latitude_deg * 1e7));
    command.params.y = static_cast<int32_t>(std::round(longitude_deg * 1e7));
    command.params.maybe_z = altitude_m;
    command.target_system_id = _gimbal_manager_sysid;
    command.target_component_id = _gimbal_manager_compid;

    return GimbalImpl::gimbal_result_from_command_result(_system_impl.send_command(command));
}

void GimbalProtocolV2::set_roi_location_async(
    double latitude_deg, double longitude_deg, float altitude_m, Gimbal::ResultCallback callback)
{
    MavlinkCommandSender::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = static_cast<int32_t>(std::round(latitude_deg * 1e7));
    command.params.y = static_cast<int32_t>(std::round(longitude_deg * 1e7));
    command.params.maybe_z = altitude_m;
    command.target_system_id = _gimbal_manager_sysid;
    command.target_component_id = _gimbal_manager_compid;

    _system_impl.send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            GimbalImpl::receive_command_result(result, callback);
        });
}

Gimbal::Result GimbalProtocolV2::take_control(Gimbal::ControlMode control_mode)
{
    auto prom = std::promise<Gimbal::Result>();
    auto fut = prom.get_future();

    take_control_async(control_mode, [&prom](Gimbal::Result result) { prom.set_value(result); });

    return fut.get();
}

void GimbalProtocolV2::take_control_async(
    Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback)
{
    if (control_mode == Gimbal::ControlMode::None) {
        release_control_async(callback);
        return;
    } else if (control_mode == Gimbal::ControlMode::Secondary) {
        _system_impl.call_user_callback([callback]() { callback(Gimbal::Result::Unsupported); });
        LogErr() << "Gimbal secondary control is not implemented yet!";
        return;
    }

    float own_sysid = _system_impl.get_own_system_id();
    float own_compid = _system_impl.get_own_component_id();

    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_GIMBAL_MANAGER_CONFIGURE;
    command.params.maybe_param1 =
        control_mode == Gimbal::ControlMode::Primary ? own_sysid : -3.0f; // sysid primary control
    command.params.maybe_param2 =
        control_mode == Gimbal::ControlMode::Primary ? own_compid : -3.0f; // compid primary control
    command.params.maybe_param3 =
        control_mode == Gimbal::ControlMode::Primary ? own_sysid : -3.0f; // sysid secondary control
    command.params.maybe_param4 = control_mode == Gimbal::ControlMode::Primary ?
                                      own_compid :
                                      -3.0f; // compid secondary control

    command.params.maybe_param7 = _gimbal_device_id;
    command.target_system_id = _gimbal_manager_sysid;
    command.target_component_id = _gimbal_manager_compid;

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
    command.params.maybe_param1 = -3.0f; // sysid primary control
    command.params.maybe_param2 = -3.0f; // compid primary control
    command.params.maybe_param3 = -3.0f; // sysid secondary control
    command.params.maybe_param4 = -3.0f; // compid secondary control
    command.params.maybe_param7 = _gimbal_device_id;
    command.target_system_id = _gimbal_manager_sysid;
    command.target_component_id = _gimbal_manager_compid;

    _system_impl.send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            GimbalImpl::receive_command_result(result, callback);
        });
}

Gimbal::ControlStatus GimbalProtocolV2::control()
{
    auto prom = std::promise<Gimbal::ControlStatus>();
    auto fut = prom.get_future();

    control_async(
        [&prom](Gimbal::ControlStatus control_status) { prom.set_value(control_status); });

    return fut.get();
}

void GimbalProtocolV2::control_async(Gimbal::ControlCallback callback)
{
    _control_callback = callback;

    if (_control_callback != nullptr) {
        if (!_is_mavlink_manager_status_registered) {
            _is_mavlink_manager_status_registered = true;

            _system_impl.register_mavlink_message_handler(
                MAVLINK_MSG_ID_GIMBAL_MANAGER_STATUS,
                [this](const mavlink_message_t& message) {
                    process_gimbal_manager_status(message);
                },
                this);
        }

        // We don't need to use the queue here. This is done outside of this class.
        _control_callback(_current_control_status);

    } else {
        if (_is_mavlink_manager_status_registered) {
            _is_mavlink_manager_status_registered = false;

            _system_impl.unregister_mavlink_message_handler(
                MAVLINK_MSG_ID_GIMBAL_MANAGER_STATUS, this);
        }
    }
}

} // namespace mavsdk
