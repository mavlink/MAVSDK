#include "gimbal_impl.h"
#include "global_include.h"
#include <functional>
#include <cmath>

namespace mavsdk {

GimbalImpl::GimbalImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

GimbalImpl::~GimbalImpl()
{
    _parent->unregister_plugin(this);
}

void GimbalImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION,
        [this](const mavlink_message_t& message) { process_gimbal_manager_information(message); },
        this);
}

void GimbalImpl::deinit() {}

void GimbalImpl::enable()
{
    _parent->register_timeout_handler(
        [this]() { receive_protocol_timeout(); }, 1.0, &_protocol_cookie);

    MAVLinkCommands::CommandLong command{};
    command.command = MAV_CMD_REQUEST_MESSAGE;
    command.params.param1 = static_cast<float>(MAVLINK_MSG_ID_GIMBAL_MANAGER_INFORMATION);
    command.target_component_id = 0; // any component
    _parent->send_command_async(command, nullptr);
}

void GimbalImpl::disable()
{
    _protocol = Protocol::Unknown;
}

void GimbalImpl::receive_protocol_timeout()
{
    // We did not receive a GIMBAL_MANAGER_INFORMATION in time, so we have to
    // assume Version2 is not available.
    _protocol = Protocol::Version1;
    LogDebug() << "Falling back to Gimbal Version 1";
}

void GimbalImpl::process_gimbal_manager_information(const mavlink_message_t& message)
{
    mavlink_gimbal_manager_information_t gimbal_manager_information;
    mavlink_msg_gimbal_manager_information_decode(&message, &gimbal_manager_information);

    LogDebug() << "Using Gimbal Version 2 as gimbal manager for gimbal device "
               << static_cast<int>(gimbal_manager_information.gimbal_device_id)
               << " was discovered";

    _protocol = Protocol::Version2;
}

Gimbal::Result GimbalImpl::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    const float roll_deg = 0.0f;
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_MOUNT_CONTROL;
    command.params.param1 = pitch_deg;
    command.params.param2 = roll_deg;
    command.params.param3 = yaw_deg;
    command.params.param7 = float(MAV_MOUNT_MODE_MAVLINK_TARGETING);
    command.target_component_id = _parent->get_autopilot_id();

    return gimbal_result_from_command_result(_parent->send_command(command));
}

void GimbalImpl::set_pitch_and_yaw_async(
    float pitch_deg, float yaw_deg, Gimbal::ResultCallback callback)
{
    const float roll_deg = 0.0f;
    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_DO_MOUNT_CONTROL;
    command.params.param1 = pitch_deg;
    command.params.param2 = roll_deg;
    command.params.param3 = yaw_deg;
    command.params.param7 = float(MAV_MOUNT_MODE_MAVLINK_TARGETING);
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
        command, [callback](MAVLinkCommands::Result command_result, float progress) {
            UNUSED(progress);
            receive_command_result(command_result, callback);
        });
}

Gimbal::Result GimbalImpl::set_mode(const Gimbal::GimbalMode gimbal_mode)
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
    command.target_component_id = _parent->get_autopilot_id();

    return gimbal_result_from_command_result(_parent->send_command(command));
}

void GimbalImpl::set_mode_async(
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
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
        command, [callback](MAVLinkCommands::Result command_result, float progress) {
            UNUSED(progress);
            receive_command_result(command_result, callback);
        });
}

float GimbalImpl::to_float_gimbal_mode(const Gimbal::GimbalMode gimbal_mode) const
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
GimbalImpl::set_roi_location(double latitude_deg, double longitude_deg, float altitude_m)
{
    MAVLinkCommands::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = int32_t(std::round(latitude_deg * 1e7));
    command.params.y = int32_t(std::round(longitude_deg * 1e7));
    command.params.z = altitude_m;
    command.target_component_id = _parent->get_autopilot_id();

    return gimbal_result_from_command_result(_parent->send_command(command));
}

void GimbalImpl::set_roi_location_async(
    double latitude_deg, double longitude_deg, float altitude_m, Gimbal::ResultCallback callback)
{
    MAVLinkCommands::CommandInt command{};

    command.command = MAV_CMD_DO_SET_ROI_LOCATION;
    command.params.x = int32_t(std::round(latitude_deg * 1e7));
    command.params.y = int32_t(std::round(longitude_deg * 1e7));
    command.params.z = altitude_m;
    command.target_component_id = _parent->get_autopilot_id();

    _parent->send_command_async(
        command, [callback](MAVLinkCommands::Result command_result, float progress) {
            UNUSED(progress);
            receive_command_result(command_result, callback);
        });
}

void GimbalImpl::receive_command_result(
    MAVLinkCommands::Result command_result, const Gimbal::ResultCallback& callback)
{
    Gimbal::Result gimbal_result = gimbal_result_from_command_result(command_result);

    if (callback) {
        callback(gimbal_result);
    }
}

Gimbal::Result GimbalImpl::gimbal_result_from_command_result(MAVLinkCommands::Result command_result)
{
    switch (command_result) {
        case MAVLinkCommands::Result::Success:
            return Gimbal::Result::Success;
        case MAVLinkCommands::Result::Timeout:
            return Gimbal::Result::Timeout;
        case MAVLinkCommands::Result::NoSystem:
        case MAVLinkCommands::Result::ConnectionError:
        case MAVLinkCommands::Result::Busy:
        case MAVLinkCommands::Result::CommandDenied:
        default:
            return Gimbal::Result::Error;
    }
}

} // namespace mavsdk
