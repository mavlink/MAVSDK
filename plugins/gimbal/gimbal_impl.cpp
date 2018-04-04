#include "gimbal_impl.h"
#include "mavlink_system.h"
#include "global_include.h"
#include "mavlink_include.h"
#include <functional>

namespace dronecore {

GimbalImpl::GimbalImpl(System &system) :
    PluginImplBase(system)
{
    _parent->register_plugin(this);
}

GimbalImpl::~GimbalImpl()
{
    _parent->unregister_plugin(this);
}

void GimbalImpl::init() {}

void GimbalImpl::deinit() {}

void GimbalImpl::enable() {}

void GimbalImpl::disable() {}

Gimbal::Result GimbalImpl::set_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    const float roll_deg = 0.0f;

    return gimbal_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_MOUNT_CONTROL,
                   MAVLinkCommands::Params {pitch_deg, roll_deg, yaw_deg, NAN, NAN, NAN,
                                            MAV_MOUNT_MODE_MAVLINK_TARGETING},
                   MAVLinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

void GimbalImpl::set_pitch_and_yaw_async(float pitch_deg, float yaw_deg,
                                         Gimbal::result_callback_t callback)
{
    const float roll_deg = 0.0f;
    _parent->send_command_with_ack_async(
        MAV_CMD_DO_MOUNT_CONTROL,
        MAVLinkCommands::Params {pitch_deg, roll_deg, yaw_deg, NAN, NAN, NAN,
                                 MAV_MOUNT_MODE_MAVLINK_TARGETING},
        std::bind(&GimbalImpl::receive_command_result, std::placeholders::_1, callback),
        MAVLinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

Gimbal::Result GimbalImpl::set_roi_location(double latitude_deg, double longitude_deg,
                                            float altitude_m)
{
    return gimbal_result_from_command_result(
               _parent->send_command_with_ack(
                   MAV_CMD_DO_SET_ROI_LOCATION,
                   MAVLinkCommands::Params {NAN, NAN, NAN, NAN, (float)latitude_deg, (float)longitude_deg,
                                            altitude_m},
                   MAVLinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT));
}

void GimbalImpl::set_roi_location_async(double latitude_deg, double longitude_deg, float altitude_m,
                                        Gimbal::result_callback_t callback)
{
    _parent->send_command_with_ack_async(
        MAV_CMD_DO_SET_ROI_LOCATION,
        MAVLinkCommands::Params {NAN, NAN, NAN, NAN, (float)latitude_deg, (float)longitude_deg,
                                 altitude_m},
        std::bind(&GimbalImpl::receive_command_result, std::placeholders::_1, callback),
        MAVLinkCommands::DEFAULT_COMPONENT_ID_AUTOPILOT);
}

void GimbalImpl::receive_command_result(MAVLinkCommands::Result command_result,
                                        const Gimbal::result_callback_t &callback)
{
    Gimbal::Result gimbal_result = gimbal_result_from_command_result(command_result);

    if (callback) {
        callback(gimbal_result);
    }
}


Gimbal::Result GimbalImpl::gimbal_result_from_command_result(MAVLinkCommands::Result
                                                             command_result)
{
    switch (command_result) {
        case MAVLinkCommands::Result::SUCCESS:
            return Gimbal::Result::SUCCESS;
        case MAVLinkCommands::Result::TIMEOUT:
            return Gimbal::Result::TIMEOUT;
        case MAVLinkCommands::Result::NO_SYSTEM:
        case MAVLinkCommands::Result::CONNECTION_ERROR:
        case MAVLinkCommands::Result::BUSY:
        case MAVLinkCommands::Result::COMMAND_DENIED:
        default:
            return Gimbal::Result::ERROR;
    }
}


} // namespace dronecore
