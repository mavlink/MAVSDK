#include "camera_impl.h"
#include "device.h"
#include "global_include.h"
#include "mavlink_include.h"
#include <functional>

namespace dronecore {

using namespace std::placeholders; // for `_1`

CameraImpl::CameraImpl(Device &device) :
    PluginImplBase(device)
{
    _parent.register_plugin(this);
}

CameraImpl::~CameraImpl()
{
    _parent.unregister_plugin(this);
}

void CameraImpl::init()
{
    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_CAPTURE_STATUS,
        std::bind(&CameraImpl::process_camera_capture_status, this, _1),
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_STORAGE_INFORMATION,
        std::bind(&CameraImpl::process_storage_information, this, _1),
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED,
        std::bind(&CameraImpl::process_camera_image_captured, this, _1),
        this);

    _parent.register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_SETTINGS,
        std::bind(&CameraImpl::process_camera_settings, this, _1),
        this);
}

void CameraImpl::deinit() {}

void CameraImpl::enable() {}

void CameraImpl::disable() {}

Camera::Result CameraImpl::take_photo()
{
    return camera_result_from_command_result(
               _parent.send_command_with_ack(
                   MAV_CMD_IMAGE_START_CAPTURE,
                   MavlinkCommands::Params {0.0f, // Reserved, set to 0
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
                   MavlinkCommands::Params {0.0f, // Reserved, set to 0
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
                   MavlinkCommands::Params {0.0f, // Reserved, set to 0
                                            NAN, NAN, NAN, NAN, NAN, NAN},
                   MAV_COMP_ID_CAMERA));
}

Camera::Result CameraImpl::start_video()
{
    return camera_result_from_command_result(
               _parent.send_command_with_ack(
                   MAV_CMD_VIDEO_START_CAPTURE,
                   MavlinkCommands::Params {0.0f, // Reserved, set to 0
                                            NAN, // fps not set yet
                                            NAN, // resolution not set yet
                                            NAN, NAN, NAN, NAN},
                   MAV_COMP_ID_CAMERA));
}

Camera::Result CameraImpl::stop_video()
{
    return camera_result_from_command_result(
               _parent.send_command_with_ack(
                   MAV_CMD_VIDEO_STOP_CAPTURE,
                   MavlinkCommands::Params {0.0f, // Reserved, set to 0
                                            NAN, NAN, NAN, NAN, NAN, NAN},
                   MAV_COMP_ID_CAMERA));
}

void CameraImpl::take_photo_async(const Camera::result_callback_t &callback)
{
    _parent.send_command_with_ack_async(
        MAV_CMD_IMAGE_START_CAPTURE,
        MavlinkCommands::Params {0.0f, // Reserved, set to 0
                                 0.0f, // no interval for one picture
                                 1.0f, // take only one picture
                                 float(_capture_sequence++),
                                 NAN, NAN, NAN},
        std::bind(&CameraImpl::receive_command_result, _1, callback),
        MAV_COMP_ID_CAMERA);

}

void CameraImpl::start_photo_interval_async(float interval_s,
                                            const Camera::result_callback_t &callback)
{
    if (!interval_valid(interval_s)) {
        if (callback) {
            callback(Camera::Result::WRONG_ARGUMENT);
        }
        return;
    }

    _parent.send_command_with_ack_async(
        MAV_CMD_IMAGE_START_CAPTURE,
        MavlinkCommands::Params {0.0f, // Reserved, set to 0
                                 interval_s,
                                 0.0f, // unlimited photos
                                 float(_capture_sequence++),
                                 NAN, NAN, NAN},
        std::bind(&CameraImpl::receive_command_result, _1, callback),
        MAV_COMP_ID_CAMERA);
}

void CameraImpl::stop_photo_interval_async(const Camera::result_callback_t &callback)
{
    _parent.send_command_with_ack_async(
        MAV_CMD_IMAGE_STOP_CAPTURE,
        MavlinkCommands::Params {0.0f, // Reserved, set to 0
                                 NAN, NAN, NAN, NAN, NAN, NAN},
        std::bind(&CameraImpl::receive_command_result, _1, callback),
        MAV_COMP_ID_CAMERA);
}

void CameraImpl::start_video_async(const Camera::result_callback_t &callback)
{
    _parent.send_command_with_ack_async(
        MAV_CMD_VIDEO_START_CAPTURE,
        MavlinkCommands::Params {0.0f, // Reserved, set to 0
                                 NAN, // fps not set yet
                                 NAN, // resolution not set yet
                                 NAN, NAN, NAN, NAN},
        std::bind(&CameraImpl::receive_command_result, _1, callback),
        MAV_COMP_ID_CAMERA);
}

void CameraImpl::stop_video_async(const Camera::result_callback_t &callback)
{
    _parent.send_command_with_ack_async(
        MAV_CMD_VIDEO_STOP_CAPTURE,
        MavlinkCommands::Params {0.0f, // Reserved, set to 0
                                 NAN, NAN, NAN, NAN, NAN, NAN},
        std::bind(&CameraImpl::receive_command_result, _1, callback),
        MAV_COMP_ID_CAMERA);
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

void CameraImpl::set_mode_async(Camera::Mode mode, const Camera::mode_callback_t &callback)
{
    float mavlink_mode;
    switch (mode) {
        case Camera::Mode::PHOTO:
            mavlink_mode = CAMERA_MODE_IMAGE;
            break;
        case Camera::Mode::VIDEO:
            mavlink_mode = CAMERA_MODE_VIDEO;
            break;
        case Camera::Mode::PHOTO_SURVEY:
            mavlink_mode = CAMERA_MODE_IMAGE_SURVEY;
            break;
        default:
        // FALLTHROUGH
        case Camera::Mode::UNKNOWN:
            mavlink_mode = NAN;
            break;
    }

    _parent.send_command_with_ack_async(
        MAV_CMD_SET_CAMERA_MODE,
        MavlinkCommands::Params {0.0f, // Reserved, set to 0
                                 mavlink_mode,
                                 NAN, NAN, NAN, NAN, NAN // Reserved
                                },
        std::bind(&CameraImpl::receive_set_mode_command_result, this, _1, callback,
                  mode),
        MAV_COMP_ID_CAMERA);
}

void CameraImpl::get_mode_async(Camera::mode_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_get_mode.mutex);
    if (_get_mode.callback != nullptr) {
        if (callback) {
            callback(Camera::Result::BUSY, Camera::Mode::UNKNOWN);
        }
    }
    _get_mode.callback = callback;

    // TODO: we should probably listen for the command result instead of
    // just the camera_mode.
    _parent.send_command_with_ack_async(
        MAV_CMD_REQUEST_CAMERA_SETTINGS,
        MavlinkCommands::Params {1.0, // Request it
                                 NAN, NAN, NAN, NAN, NAN, NAN // Reserved
                                },
        nullptr,
        MAV_COMP_ID_CAMERA);

    _parent.register_timeout_handler(
        std::bind(&CameraImpl::receive_get_mode_timeout, this), 1.0, &_timeout_cookie);
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

void CameraImpl::get_status_async(Camera::get_status_callback_t callback)
{
    if (callback == nullptr) {
        LogDebug() << "get_status_async called with empty callback";
        return;
    }

    {
        std::lock_guard<std::mutex> lock(_last_status.mutex);
        // If we're already trying to get the status, we need to wait.
        if (_last_status.callback != nullptr) {
            if (callback) {
                Camera::Status empty_status = {};
                callback(Camera::Result::IN_PROGRESS, empty_status);
                return;
            }
        }
        // Let's create a subscription for the (hopefully) incoming messages.
        _last_status.callback = callback;
    }

    _parent.send_command_with_ack_async(
        MAV_CMD_REQUEST_CAMERA_CAPTURE_STATUS,
        MavlinkCommands::Params {1.0f, // request it
                                 NAN, NAN, NAN, NAN, NAN, NAN},
        std::bind(&CameraImpl::receive_camera_capture_status_result, this, _1),
        MAV_COMP_ID_CAMERA);

    _parent.send_command_with_ack_async(
        MAV_CMD_REQUEST_STORAGE_INFORMATION,
        MavlinkCommands::Params {0.0f, // Reserved
                                 1.0f, // request it
                                 NAN, NAN, NAN, NAN, NAN},
        std::bind(&CameraImpl::receive_storage_information_result, this, _1),
        MAV_COMP_ID_CAMERA);

    // We would like to complete this status within 3s, TODO: remove this long timeout.
    _parent.register_timeout_handler(std::bind(&CameraImpl::status_timeout_happened, this),
                                     DEFAULT_TIMEOUT_S * 3.0, &_timeout_cookie);
}

void CameraImpl::receive_camera_capture_status_result(MavlinkCommands::Result result)
{
    std::lock_guard<std::mutex> lock(_last_status.mutex);
    if (_last_status.callback == nullptr) {
        // We're not expecting this message, let's ignore it.
        return;
    }

    if (result != MavlinkCommands::Result::SUCCESS) {
        if (_last_status.callback) {
            Camera::Status empty_status = {};
            _last_status.callback(camera_result_from_command_result(result), empty_status);
        }
        // Something went wrong, we give up.
        _last_status.callback = nullptr;
    }

    // FIXME: doesn't this interfere with other timeout handlers?
    _parent.refresh_timeout_handler(this);
}

void CameraImpl::capture_info_async(Camera::capture_info_callback_t callback)
{
    _capture_info_callback = callback;
}

void CameraImpl::process_camera_capture_status(const mavlink_message_t &message)
{
    mavlink_camera_capture_status_t camera_capture_status;
    mavlink_msg_camera_capture_status_decode(&message, &camera_capture_status);

    {
        std::lock_guard<std::mutex> lock(_last_status.mutex);
        _last_status.data.video_on = (camera_capture_status.video_status == 1);
        _last_status.data.photo_interval_on = (camera_capture_status.image_status == 2 ||
                                               camera_capture_status.image_status == 3);
        _last_status.received_camera_capture_status = true;
    }

    check_last_status();
}

void CameraImpl::process_storage_information(const mavlink_message_t &message)
{
    mavlink_storage_information_t storage_information;
    mavlink_msg_storage_information_decode(&message, &storage_information);

    {
        std::lock_guard<std::mutex> lock(_last_status.mutex);
        if (storage_information.status == 0) {
            _last_status.data.storage_status = Camera::Status::StorageStatus::NOT_AVAILABLE;
        } else if (storage_information.status == 1) {
            _last_status.data.storage_status = Camera::Status::StorageStatus::UNFORMATTED;
        } else if (storage_information.status == 2) {
            _last_status.data.storage_status = Camera::Status::StorageStatus::FORMATTED;
        }
        _last_status.data.available_storage_mib = storage_information.available_capacity;
        _last_status.data.used_storage_mib = storage_information.used_capacity;
        _last_status.data.total_storage_mib = storage_information.total_capacity;
        _last_status.received_storage_information = true;
    }

    check_last_status();
}

void CameraImpl::process_camera_image_captured(const mavlink_message_t &message)
{
    mavlink_camera_image_captured_t image_captured;
    mavlink_msg_camera_image_captured_decode(&message, &image_captured);

    {
        if (_capture_info_callback) {
            Camera::CaptureInfo capture_info = {};
            capture_info.position.latitude_deg = image_captured.lat / 1e7;
            capture_info.position.longitude_deg = image_captured.lon / 1e7;
            capture_info.position.absolute_altitude_m = image_captured.alt / 1e3f;
            capture_info.position.relative_altitude_m = image_captured.relative_alt / 1e3f;
            capture_info.time_utc_us = image_captured.time_utc;
            capture_info.quaternion.w = image_captured.q[0];
            capture_info.quaternion.x = image_captured.q[1];
            capture_info.quaternion.y = image_captured.q[2];
            capture_info.quaternion.z = image_captured.q[3];
            capture_info.file_url = std::string(image_captured.file_url);
            capture_info.success = (image_captured.capture_result == 1);
            capture_info.index = image_captured.image_index;
            _capture_info_callback(capture_info);
        }
    }
}

void CameraImpl::process_camera_settings(const mavlink_message_t &message)
{
    std::lock_guard<std::mutex> lock(_get_mode.mutex);
    if (_get_mode.callback == nullptr) {
        // It seems that we are not interested.
        return;
    }

    mavlink_camera_settings_t camera_settings;
    mavlink_msg_camera_settings_decode(&message, &camera_settings);

    Camera::Mode mode;
    switch (camera_settings.mode_id) {
        case CAMERA_MODE_IMAGE:
            mode = Camera::Mode::PHOTO;
            break;
        case CAMERA_MODE_VIDEO:
            mode = Camera::Mode::VIDEO;
            break;
        case CAMERA_MODE_IMAGE_SURVEY:
            mode = Camera::Mode::PHOTO_SURVEY;
            break;
        default:
            mode = Camera::Mode::UNKNOWN;
            break;
    }


    //if (_camera_definition) {
    //    // This "parameter" needs to be manually set.

    //    CameraDefinition::ParameterValue value;
    //    value.value.as_uint32 = camera_settings.mode_id;
    //    _camera_definition->update_setting("CAM_MODE", value);
    //}

    _get_mode.callback(Camera::Result::SUCCESS, mode);
    _get_mode.callback = nullptr;

    // FIXME: this will interfere with others.
    _parent.unregister_timeout_handler(_timeout_cookie);
}

void CameraImpl::check_last_status()
{
    std::lock_guard<std::mutex> lock(_last_status.mutex);
    if (_last_status.received_camera_capture_status &&
        _last_status.received_storage_information) {

        if (_last_status.callback) {
            _last_status.callback(Camera::Result::SUCCESS, _last_status.data);
            _last_status.callback = nullptr;
            _last_status.received_camera_capture_status = false;
            _last_status.received_storage_information = false;
            // TODO: we need to make sure not to interfere with other timeouts
            _parent.unregister_timeout_handler(_timeout_cookie);
        }
    }
}

void CameraImpl::status_timeout_happened()
{
    std::lock_guard<std::mutex> lock(_last_status.mutex);

    if (_last_status.callback) {
        // Send empty settings with timeout error.
        Camera::Status empty_status = {};
        _last_status.callback(Camera::Result::TIMEOUT, empty_status);
        // Then give up.
    }
    // Discard the subscription
    _last_status.callback = nullptr;
}

void CameraImpl::receive_command_result(MavlinkCommands::Result command_result,
                                        const Camera::result_callback_t &callback)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    if (callback) {
        callback(camera_result);
    }
}


void CameraImpl::receive_storage_information_result(MavlinkCommands::Result result)
{
    std::lock_guard<std::mutex> lock(_last_status.mutex);
    if (_last_status.callback == nullptr) {
        // We're not expecting this message, let's ignore it.
        return;
    }

    if (result != MavlinkCommands::Result::SUCCESS) {
        if (_last_status.callback) {
            Camera::Status empty_status = {};
            _last_status.callback(camera_result_from_command_result(result), empty_status);
        }
        // Something went wrong, we give up.
        _last_status.callback = nullptr;
    }

    // TODO: decode and save values
    _parent.refresh_timeout_handler(this);
}

void CameraImpl::receive_set_mode_command_result(MavlinkCommands::Result command_result,
                                                 const Camera::mode_callback_t &callback,
                                                 Camera::Mode mode)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    if (callback) {
        callback(camera_result, mode);
    }
}

void CameraImpl::receive_get_mode_timeout()
{
    std::lock_guard<std::mutex> lock(_get_mode.mutex);
    if (_get_mode.callback) {
        _get_mode.callback(Camera::Result::TIMEOUT, Camera::Mode::UNKNOWN);
        _get_mode.callback = nullptr;
    }
}


} // namespace dronecore
