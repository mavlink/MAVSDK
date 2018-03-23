#include "camera_impl.h"
#include "device.h"
#include "global_include.h"
#include "mavlink_include.h"
#include <functional>

namespace dronecore {

CameraImpl::CameraImpl(Device &device) :
    PluginImplBase(device)
{
    _parent.register_plugin(this);
}

CameraImpl::~CameraImpl()
{
    _parent.unregister_plugin(this);
}

void CameraImpl::init() {}

void CameraImpl::deinit() {}

void CameraImpl::enable() {}

void CameraImpl::disable() {}

Camera::Result CameraImpl::take_photo()
{
    return camera_result_from_command_result(
               _parent.send_command_with_ack(
                   MAV_CMD_IMAGE_START_CAPTURE,
                   MavlinkCommands::Params {0.0f, // all camera IDs
                                            0.0f, // only picture, no interval
                                            1.0f, // take only one picture
                                            float(_capture_sequence++),
                                            NAN, NAN, NAN},
                   MAV_COMP_ID_CAMERA));
}

Camera::Result CameraImpl::start_photo_interval(float interval_s)
{
    if (!interval_valid(interval_s)) {
        return Camera::Result::WRONG_ARGUMENT;
    }

    return camera_result_from_command_result(
               _parent.send_command_with_ack(
                   MAV_CMD_IMAGE_START_CAPTURE,
                   MavlinkCommands::Params {0.0f, // all camera IDs
                                            interval_s,
                                            0, // unlimited photos
                                            float(_capture_sequence++),
                                            NAN, NAN, NAN},
                   MAV_COMP_ID_CAMERA));
}

Camera::Result CameraImpl::stop_photo_interval()
{
    return camera_result_from_command_result(
               _parent.send_command_with_ack(
                   MAV_CMD_IMAGE_STOP_CAPTURE,
                   MavlinkCommands::Params {0.0f, // all camera IDs
                                            NAN, NAN, NAN, NAN, NAN, NAN},
                   MAV_COMP_ID_CAMERA));
}

Camera::Result CameraImpl::start_video()
{
    return camera_result_from_command_result(
               _parent.send_command_with_ack(
                   MAV_CMD_VIDEO_START_CAPTURE,
                   MavlinkCommands::Params {0.0f, // all camera IDs
                                            NAN, // fps not set yet
                                            NAN, // resolution not set yet
                                            NAN, NAN, NAN, NAN},
                   MAV_COMP_ID_CAMERA));
}

Camera::Result CameraImpl::camera_result_from_command_result(MavlinkCommands::Result
                                                             command_result)
{
    switch (command_result) {
        case MavlinkCommands::Result::SUCCESS:
            return Camera::Result::SUCCESS;
        case MavlinkCommands::Result::NO_DEVICE:
        // FALLTHROUGH
        case MavlinkCommands::Result::CONNECTION_ERROR:
        // FALLTHROUGH
        case MavlinkCommands::Result::BUSY:
            return Camera::Result::ERROR;
        case MavlinkCommands::Result::COMMAND_DENIED:
            return Camera::Result::DENIED;
        case MavlinkCommands::Result::TIMEOUT:
            return Camera::Result::TIMEOUT;
        default:
            return Camera::Result::UNKNOWN;
    }
}

bool CameraImpl::interval_valid(float interval_s)
{
    // Reject everything faster than 1000 Hz, as well as negative inputs.
    if (interval_s < 0.001f) {
        LogWarn() << "Invalid interval input";
        return false;
    } else {
        return true;
    }
}

} // namespace dronecore
