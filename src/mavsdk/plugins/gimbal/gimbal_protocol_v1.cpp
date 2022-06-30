#include <functional>
#include <cmath>
#include "gimbal_protocol_v1.h"
#include "gimbal_impl.h"
#include "unused.h"

namespace mavsdk {

GimbalProtocolV1::GimbalProtocolV1(SystemImpl& system_impl) : GimbalProtocolBase(system_impl) {}

GimbalProtocolV1::~GimbalProtocolV1()
{
    _system_impl.remove_call_every(_control_cookie);
}

Gimbal::Result GimbalProtocolV1::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    const float roll_deg = 0.0f;
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_MOUNT_CONTROL;
    command.params.maybe_param1 = pitch_deg;
    command.params.maybe_param2 = roll_deg;
    command.params.maybe_param3 = yaw_deg;
    command.params.maybe_param7 = static_cast<float>(MAV_MOUNT_MODE_MAVLINK_TARGETING);
    command.target_component_id = _system_impl.get_autopilot_id();

    return GimbalImpl::gimbal_result_from_command_result(_system_impl.send_command(command));
}

void GimbalProtocolV1::set_pitch_and_yaw_async(
    float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    const float roll_deg = 0.0f;
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_MOUNT_CONTROL;
    command.params.maybe_param1 = pitch_deg;
    command.params.maybe_param2 = roll_deg;
    command.params.maybe_param3 = yaw_deg;
    command.params.maybe_param7 = static_cast<float>(MAV_MOUNT_MODE_MAVLINK_TARGETING);
    command.target_component_id = _system_impl.get_autopilot_id();

    _system_impl.send_command_async(
        command, [callback](MavlinkCommandSender::Result command_result, float progress) {
            UNUSED(progress);
            GimbalImpl::receive_command_result(command_result, callback);
        });
}

Gimbal::Result
GimbalProtocolV1::set_pitch_rate_and_yaw_rate(float pitch_rate_deg_s, float yaw_rate_deg_s)
{
    UNUSED(pitch_rate_deg_s);
    UNUSED(yaw_rate_deg_s);
    return Gimbal::Result::Unsupported;
}

void GimbalProtocolV1::set_pitch_rate_and_yaw_rate_async(
    float pitch_rate_deg_s, float yaw_rate_deg_s, Gimbal::ResultCallback callback)
{
    UNUSED(pitch_rate_deg_s);
    UNUSED(yaw_rate_deg_s);

    if (callback) {
        auto temp_callback = callback;
        _system_impl.call_user_callback(
            [temp_callback]() { temp_callback(Gimbal::Result::Unsupported); });
    }
}

Gimbal::Result GimbalProtocolV1::set_mode(const Gimbal::GimbalMode gimbal_mode)
{
    MavlinkCommandSender::CommandInt command{};

    // Correct here would actually be to:
    // - set yaw stabilize / param4 to 0 as usual
    // - set param7/paramz to 2.
    command.command =
        MAV_CMD_DO_MOUNT_CONFIGURE; // Mission command to configure a camera or antenna mount
    command.params.maybe_param1 =
        static_cast<float>(MAV_MOUNT_MODE_MAVLINK_TARGETING); // Mount operation mode
    command.params.maybe_param2 = 0.0f; // stabilize roll
    command.params.maybe_param3 = 0.0f; // stabilize pitch
    command.params.maybe_param4 =
        to_float_gimbal_mode(gimbal_mode); // stabilize yaw (1 = yes, 0 = no)
    command.params.x = 0; // roll input
    command.params.y = 0; // pitch input
    command.params.maybe_z =
        2.0f; // yaw input (0 = angle body frame, 1 = angular rate, 2 = angle absolute frame)
    command.target_component_id = _system_impl.get_autopilot_id();

    return GimbalImpl::gimbal_result_from_command_result(_system_impl.send_command(command));
}

void GimbalProtocolV1::set_mode_async(
    const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback)
{
    MavlinkCommandSender::CommandInt command{};

    // Correct here would actually be to:
    // - set yaw stabilize / param4 to 0 as usual
    // - set param7/paramz to 2.
    command.command = MAV_CMD_DO_MOUNT_CONFIGURE;
    command.params.maybe_param1 =
        static_cast<float>(MAV_MOUNT_MODE_MAVLINK_TARGETING); // Mount operation mode
    command.params.maybe_param2 = 0.0f; // stabilize roll
    command.params.maybe_param3 = 0.0f; // stabilize pitch
    command.params.maybe_param4 =
        to_float_gimbal_mode(gimbal_mode); // stabilize yaw (1 = yes, 0 = no)
    command.params.x = 0; // roll input
    command.params.y = 0; // pitch input
    command.params.maybe_z =
        2.0f; // yaw input (0 = angle body frame, 1 = angular rate, 2 = angle absolute frame)
    command.target_component_id = _system_impl.get_autopilot_id();

    _system_impl.send_command_async(
        command, [callback](MavlinkCommandSender::Result command_result, float progress) {
            UNUSED(progress);
            GimbalImpl::receive_command_result(command_result, callback);
        });
}

float GimbalProtocolV1::to_float_gimbal_mode(const Gimbal::GimbalMode gimbal_mode)
{
    switch (gimbal_mode) {
        case Gimbal::GimbalMode::YawFollow:
            return 0.0f;
        case Gimbal::GimbalMode::YawLock:
            return 1.0f;
        default:
            return 0.0f;
    }
}

Gimbal::Result
GimbalProtocolV1::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    MavlinkCommandSender::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = static_cast<int32_t>(std::round(latitude_deg * 1e7));
    command.params.y = static_cast<int32_t>(std::round(longitude_deg * 1e7));
    command.params.maybe_z = altitude_m;
    command.target_component_id = _system_impl.get_autopilot_id();

    return GimbalImpl::gimbal_result_from_command_result(_system_impl.send_command(command));
}

void GimbalProtocolV1::set_roi_location_async(
    double latitude_deg, double longitude_deg, float altitude_m, Gimbal::ResultCallback callback)
{
    MavlinkCommandSender::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = static_cast<int32_t>(std::round(latitude_deg * 1e7));
    command.params.y = static_cast<int32_t>(std::round(longitude_deg * 1e7));
    command.params.maybe_z = altitude_m;
    command.target_component_id = _system_impl.get_autopilot_id();

    _system_impl.send_command_async(
        command, [callback](MavlinkCommandSender::Result command_result, float progress) {
            UNUSED(progress);
            GimbalImpl::receive_command_result(command_result, callback);
        });
}

Gimbal::Result GimbalProtocolV1::take_control(Gimbal::ControlMode control_mode)
{
    _current_control_status.control_mode = control_mode;
    return Gimbal::Result::Success;
}

void GimbalProtocolV1::take_control_async(
    Gimbal::ControlMode control_mode, Gimbal::ResultCallback callback)
{
    _current_control_status.control_mode = control_mode;

    if (callback) {
        _system_impl.call_user_callback([callback]() { callback(Gimbal::Result::Success); });
    }
}

Gimbal::Result GimbalProtocolV1::release_control()
{
    _current_control_status.control_mode = Gimbal::ControlMode::None;
    return Gimbal::Result::Success;
}

void GimbalProtocolV1::release_control_async(Gimbal::ResultCallback callback)
{
    _current_control_status.control_mode = Gimbal::ControlMode::None;

    if (callback) {
        _system_impl.call_user_callback([callback]() { callback(Gimbal::Result::Success); });
    }
}

Gimbal::ControlStatus GimbalProtocolV1::control()
{
    return _current_control_status;
}

void GimbalProtocolV1::control_async(Gimbal::ControlCallback callback)
{
    if (_control_callback == nullptr && callback != nullptr) {
        _control_callback = callback;
        _system_impl.add_call_every(
            [this]() { _control_callback(_current_control_status); }, 1.0, &_control_cookie);

    } else if (_control_callback != nullptr && callback == nullptr) {
        _control_callback = callback;
        _system_impl.remove_call_every(_control_cookie);

    } else {
        _control_callback = callback;
    }
}

} // namespace mavsdk
