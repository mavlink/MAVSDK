#include <fstream> // for `std::ifstream`
#include <sstream> // for `std::stringstream`
#include <functional>
#include "camera_impl.h"
#include "system.h"
#include "global_include.h"

namespace dronecore {

CameraImpl::CameraImpl(System &system, uint8_t camera_id) :
    PluginImplBase(system),
    _camera_id {camera_id}
{
    _parent->register_plugin(this);
}

CameraImpl::~CameraImpl()
{
    _parent->unregister_plugin(this);
}

void CameraImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_INFORMATION,
        std::bind(&CameraImpl::on_receive_camera_information, this, _1), this);

    _parent->send_command_with_ack(
        MAV_CMD_REQUEST_CAMERA_INFORMATION,
        MAVLinkCommands::Params {1.0f, NAN, NAN, NAN, NAN, NAN, NAN},
        MAV_COMP_ID_CAMERA + _camera_id - 1);
}

void CameraImpl::enable() {}

void CameraImpl::disable()
{
}

void CameraImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void CameraImpl::on_receive_camera_information(const mavlink_message_t &message)
{
    mavlink_camera_information_t camera_info;

    // decode camera info
    mavlink_msg_camera_information_decode(&message, &camera_info);

    LogDebug() << "Vendor: " << camera_info.vendor_name;
    LogDebug() << "Model: " << camera_info.model_name;
    LogDebug() << "FW ver: " << camera_info.firmware_version;
    LogDebug() << "Sensor H size: " << camera_info.sensor_size_h;
    LogDebug() << "Sensor V size: " << camera_info.sensor_size_v;
    LogDebug() << "Resolution H: " << camera_info.resolution_h;
    LogDebug() << "Resolution V: " << camera_info.resolution_v;
    LogDebug() << "Flags: " << camera_info.flags;
    LogDebug() << "Focal length: " << camera_info.focal_length;
    LogDebug() << "Camera def ver: " << camera_info.cam_definition_version;
    LogDebug() << "Camera def URI: " << camera_info.cam_definition_uri;
}

} // namespace dronecore
