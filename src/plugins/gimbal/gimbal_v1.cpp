#include "gimbal_v1.h"
#include "gimbal_impl.h"
#include "global_include.h"
#include <functional>
#include <cmath>

namespace mavsdk {

GimbalV1::GimbalV1(SystemImpl &system_impl) :
    _system_impl(system_impl) {}

Gimbal::Result GimbalV1::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    const float roll_deg = 0.0f;
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_MOUNT_CONTROL;
    command.params.param1 = pitch_deg;
    command.params.param2 = roll_deg;
    command.params.param3 = yaw_deg;
    command.params.param7 = float(MAV_MOUNT_MODE_MAVLINK_TARGETING);
    command.target_component_id = _system_impl.get_autopilot_id();

    return GimbalImpl::gimbal_result_from_command_result(_system_impl.send_command(command));
}

void GimbalV1::set_pitch_and_yaw_async(
    float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    const float roll_deg = 0.0f;
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_MOUNT_CONTROL;
    command.params.param1 = pitch_deg;
    command.params.param2 = roll_deg;
    command.params.param3 = yaw_deg;
    command.params.param7 = float(MAV_MOUNT_MODE_MAVLINK_TARGETING);
    command.target_component_id = _system_impl.get_autopilot_id();

    _system_impl.send_command_async(
        command, [callback](MAVLinkCommands::Result command_result, float progress) {
            UNUSED(progress);
            GimbalImpl::receive_command_result(command_result, callback);
        });
}

Gimbal::Result GimbalV1::set_mode(const Gimbal::GimbalMode gimbal_mode)
{
    MAVLinkCommands::CommandInt command{};

    command.command =
        MAV_CMD_DO_MOUNT_CONFIGURE; // Mission command to configure a camera or antenna mount
    command.params.param1 = float(MAV_MOUNT_MODE_MAVLINK_TARGETING); // Mount operation mode
    command.params.param2 = 0.0f; // stabilize roll
    command.params.param3 = 0.0f; // stabilize pitch
    command.params.param4 = to_float_gimbal_mode(gimbal_mode); // stabilize yaw (1 = yes, 0 = no)
    command.params.x = 0; // roll input
    command.params.y = 0; // pitch input
    command.params.z =
        2.0f; // yaw input (0 = angle body frame, 1 = angular rate, 2 = angle absolute frame)
    command.target_component_id = _system_impl.get_autopilot_id();

    return GimbalImpl::gimbal_result_from_command_result(_system_impl.send_command(command));
}

void GimbalV1::set_mode_async(
    const Gimbal::GimbalMode gimbal_mode, Gimbal::ResultCallback callback)
{
    MAVLinkCommands::CommandInt command{};

    command.command = MAV_CMD_DO_MOUNT_CONFIGURE;
    command.params.param1 = float(MAV_MOUNT_MODE_MAVLINK_TARGETING); // Mount operation mode
    command.params.param2 = 0.0f; // stabilize roll
    command.params.param3 = 0.0f; // stabilize pitch
    command.params.param4 = to_float_gimbal_mode(gimbal_mode); // stabilize yaw (1 = yes, 0 = no)
    command.params.x = 0; // roll input
    command.params.y = 0; // pitch input
    command.params.z =
        2.0f; // yaw input (0 = angle body frame, 1 = angular rate, 2 = angle absolute frame)
    command.target_component_id = _system_impl.get_autopilot_id();

    _system_impl.send_command_async(
        command, [callback](MAVLinkCommands::Result command_result, float progress) {
            UNUSED(progress);
            GimbalImpl::receive_command_result(command_result, callback);
        });
}

float GimbalV1::to_float_gimbal_mode(const Gimbal::GimbalMode gimbal_mode)
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
GimbalV1::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    MAVLinkCommands::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = int32_t(std::round(latitude_deg * 1e7));
    command.params.y = int32_t(std::round(longitude_deg * 1e7));
    command.params.z = altitude_m;
    command.target_component_id = _system_impl.get_autopilot_id();

    return GimbalImpl::gimbal_result_from_command_result(_system_impl.send_command(command));
}

void GimbalV1::set_roi_location_async(
    double latitude_deg, double longitude_deg, float altitude_m, Gimbal::ResultCallback callback)
{
    MAVLinkCommands::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = int32_t(std::round(latitude_deg * 1e7));
    command.params.y = int32_t(std::round(longitude_deg * 1e7));
    command.params.z = altitude_m;
    command.target_component_id = _system_impl.get_autopilot_id();

    _system_impl.send_command_async(
        command, [callback](MAVLinkCommands::Result command_result, float progress) {
            UNUSED(progress);
            GimbalImpl::receive_command_result(command_result, callback);
        });
}

} // namespace mavsdk
