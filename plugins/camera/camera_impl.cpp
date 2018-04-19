#include "camera_impl.h"
#include "camera_definition.h"
#include "system.h"
#include "global_include.h"
#include "mavlink_include.h"
#include "http_loader.h"
#include <functional>

namespace dronecore {

using namespace std::placeholders; // for `_1`

CameraImpl::CameraImpl(System &system) :
    PluginImplBase(system)
{
    _parent->register_plugin(this);
}

CameraImpl::~CameraImpl()
{
    _parent->unregister_plugin(this);
}

void CameraImpl::init()
{
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_CAPTURE_STATUS,
        std::bind(&CameraImpl::process_camera_capture_status, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_STORAGE_INFORMATION,
        std::bind(&CameraImpl::process_storage_information, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED,
        std::bind(&CameraImpl::process_camera_image_captured, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_SETTINGS,
        std::bind(&CameraImpl::process_camera_settings, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_INFORMATION,
        std::bind(&CameraImpl::process_camera_information, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_VIDEO_STREAM_INFORMATION,
        std::bind(&CameraImpl::process_video_information, this, _1),
        this);

    auto command_camera_info = make_command_request_camera_info();

    _parent->send_command_async(command_camera_info, nullptr);
}

void CameraImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void CameraImpl::enable()
{
    refresh_params();
}

void CameraImpl::disable()
{
    invalidate_params();
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_request_camera_info()
{
    MAVLinkCommands::CommandLong command_camera_info {};

    command_camera_info.command = MAV_CMD_REQUEST_CAMERA_INFORMATION;
    command_camera_info.params.param1 = 1.0f; // Request it
    command_camera_info.target_component_id = MAV_COMP_ID_CAMERA;

    return command_camera_info;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_take_photo(float interval_s, float no_of_photos)
{
    MAVLinkCommands::CommandLong cmd_take_photo {};

    cmd_take_photo.command = MAV_CMD_IMAGE_START_CAPTURE;
    cmd_take_photo.params.param1 = 0.0f; // Reserved, set to 0
    cmd_take_photo.params.param2 = interval_s;
    cmd_take_photo.params.param3 = no_of_photos;
    cmd_take_photo.params.param4 = float(_capture.sequence++);
    cmd_take_photo.target_component_id = MAV_COMP_ID_CAMERA;

    return cmd_take_photo;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_stop_photo()
{
    MAVLinkCommands::CommandLong cmd_stop_photo {};

    cmd_stop_photo.command = MAV_CMD_IMAGE_STOP_CAPTURE;
    cmd_stop_photo.target_component_id = MAV_COMP_ID_CAMERA;

    return cmd_stop_photo;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_start_video(float capture_status_rate_hz)
{
    MAVLinkCommands::CommandLong cmd_start_video {};

    cmd_start_video.command = MAV_CMD_VIDEO_START_CAPTURE;
    cmd_start_video.params.param1 = 0.f; // Reserved, set to 0
    cmd_start_video.params.param2 = capture_status_rate_hz;
    cmd_start_video.target_component_id = MAV_COMP_ID_CAMERA;

    return cmd_start_video;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_stop_video()
{
    MAVLinkCommands::CommandLong cmd_stop_video {};

    cmd_stop_video.command = MAV_CMD_VIDEO_STOP_CAPTURE;
    cmd_stop_video.params.param1 = 0.f; // Reserved, set to 0
    cmd_stop_video.target_component_id = MAV_COMP_ID_CAMERA;

    return cmd_stop_video;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_set_camera_mode(float mavlink_mode)
{
    MAVLinkCommands::CommandLong cmd_set_camera_mode {};

    cmd_set_camera_mode.command = MAV_CMD_SET_CAMERA_MODE;
    cmd_set_camera_mode.params.param1 = 0.0f; // Reserved, set to 0
    cmd_set_camera_mode.params.param2 = mavlink_mode;
    cmd_set_camera_mode.target_component_id = MAV_COMP_ID_CAMERA;

    return  cmd_set_camera_mode;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_request_camera_settings()
{
    MAVLinkCommands::CommandLong cmd_req_camera_settings {};

    cmd_req_camera_settings.command = MAV_CMD_REQUEST_CAMERA_SETTINGS;
    cmd_req_camera_settings.params.param1 = 1.f; // Request it
    cmd_req_camera_settings.target_component_id = MAV_COMP_ID_CAMERA;

    return cmd_req_camera_settings;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_request_camera_capture_status()
{
    MAVLinkCommands::CommandLong cmd_req_camera_cap_stat {};

    cmd_req_camera_cap_stat.command = MAV_CMD_REQUEST_CAMERA_CAPTURE_STATUS;
    cmd_req_camera_cap_stat.params.param1 = 1.0f; // Request it

    return cmd_req_camera_cap_stat;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_request_storage_info()
{
    MAVLinkCommands::CommandLong cmd_req_storage_info {};

    cmd_req_storage_info.command = MAV_CMD_REQUEST_STORAGE_INFORMATION;
    cmd_req_storage_info.params.param1 = 0.f; // Reserved, set to 0
    cmd_req_storage_info.params.param2 = 1.f; // Request it
    cmd_req_storage_info.target_component_id = MAV_COMP_ID_CAMERA;

    return cmd_req_storage_info;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_start_video_streaming()
{
    MAVLinkCommands::CommandLong cmd_start_video_streaming {};

    cmd_start_video_streaming.command = MAV_CMD_VIDEO_START_STREAMING;
    cmd_start_video_streaming.target_component_id = MAV_COMP_ID_CAMERA;

    return  cmd_start_video_streaming;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_stop_video_streaming()
{
    MAVLinkCommands::CommandLong cmd_stop_video_streaming {};

    cmd_stop_video_streaming.command = MAV_CMD_VIDEO_STOP_STREAMING;
    cmd_stop_video_streaming.target_component_id = MAV_COMP_ID_CAMERA;

    return  cmd_stop_video_streaming;

}

mavlink_message_t
CameraImpl::make_message_set_video_stream_settings(const Camera::VideoStreamSettings &settings)
{
    mavlink_message_t msg;

    mavlink_msg_set_video_stream_settings_pack(GCSClient::system_id,
                                               GCSClient::component_id,
                                               &msg,
                                               _parent->get_system_id(),
                                               MAV_COMP_ID_CAMERA,
                                               MAV_COMP_ID_CAMERA, // Is it right ?
                                               settings.frame_rate_hz,
                                               settings.resolution_h_pix,
                                               settings.resolution_v_pix,
                                               settings.bit_rate_b_s,
                                               settings.rotation_deg,
                                               settings.uri.c_str());

    return  msg;
}

MAVLinkCommands::CommandLong
CameraImpl::make_command_request_video_stream_info()
{
    MAVLinkCommands::CommandLong cmd_req_video_stream_info {};

    cmd_req_video_stream_info.command = MAV_CMD_REQUEST_VIDEO_STREAM_INFORMATION;
    cmd_req_video_stream_info.params.param2 = 1.0f;
    cmd_req_video_stream_info.target_component_id = MAV_COMP_ID_CAMERA;

    return cmd_req_video_stream_info;
}

Camera::Result CameraImpl::take_photo()
{
    // TODO: check whether we are in photo mode.

    std::lock_guard<std::mutex> lock(_capture.mutex);

    // Take 1 photo only with no interval
    auto cmd_take_photo = make_command_take_photo(0.f, 1.0f);

    return camera_result_from_command_result(_parent->send_command(cmd_take_photo));
}

Camera::Result CameraImpl::start_photo_interval(float interval_s)
{
    if (!interval_valid(interval_s)) {
        return Camera::Result::WRONG_ARGUMENT;
    }

    // TODO: check whether we are in photo mode.

    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd_take_photo_time_lapse = make_command_take_photo(interval_s, 0.f);

    return camera_result_from_command_result(
               _parent->send_command(cmd_take_photo_time_lapse));
}

Camera::Result CameraImpl::stop_photo_interval()
{
    auto cmd_stop_photo_interval = make_command_stop_photo();

    return camera_result_from_command_result(
               _parent->send_command(cmd_stop_photo_interval));
}

Camera::Result CameraImpl::start_video()
{
    // TODO: check whether video capture is already in progress.
    // TODO: check whether we are in video mode.

    // Capture status rate is not set
    auto cmd_start_video = make_command_start_video(0.f);

    return camera_result_from_command_result(_parent->send_command(cmd_start_video));
}

Camera::Result CameraImpl::stop_video()
{
    auto cmd_stop_video = make_command_stop_video();

    return camera_result_from_command_result(_parent->send_command(cmd_stop_video));
}

void CameraImpl::take_photo_async(const Camera::result_callback_t &callback)
{
    // TODO: check whether we are in photo mode.

    std::lock_guard<std::mutex> lock(_capture.mutex);

    // Take 1 photo only with no interval
    auto cmd_take_photo = make_command_take_photo(0.f, 1.0f);

    _parent->send_command_async(cmd_take_photo,
                                std::bind(&CameraImpl::receive_command_result, _1, callback));

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

    // TODO: check whether we are in photo mode.

    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd_take_photo_time_lapse = make_command_take_photo(interval_s, 0.f);

    _parent->send_command_async(cmd_take_photo_time_lapse,
                                std::bind(&CameraImpl::receive_command_result, _1, callback));
}

void CameraImpl::stop_photo_interval_async(const Camera::result_callback_t &callback)
{
    auto cmd_stop_photo_interval = make_command_stop_photo();

    _parent->send_command_async(cmd_stop_photo_interval,
                                std::bind(&CameraImpl::receive_command_result, _1, callback));
}

void CameraImpl::start_video_async(const Camera::result_callback_t &callback)
{
    // TODO: check whether video capture is already in progress.
    // TODO: check whether we are in video mode.

    // Capture status rate is not set
    auto cmd_start_video = make_command_start_video(0.f);

    _parent->send_command_async(cmd_start_video,
                                std::bind(&CameraImpl::receive_command_result, _1, callback));
}

void CameraImpl::stop_video_async(const Camera::result_callback_t &callback)
{
    auto cmd_stop_video = make_command_stop_video();

    _parent->send_command_async(cmd_stop_video,
                                std::bind(&CameraImpl::receive_command_result, _1, callback));
}

void CameraImpl::set_video_stream_settings(const Camera::VideoStreamSettings &settings)
{
    auto msg = make_message_set_video_stream_settings(settings);

    if (!_parent->send_message(msg)) {
        LogErr() << "Failed to set Video stream settings";
    }
}

Camera::Result CameraImpl::start_video_streaming()
{
    if (_video_stream_info.available &&
        _video_stream_info.info.status == Camera::VideoStreamInfo::Status::IN_PROGRESS) {
        return Camera::Result::IN_PROGRESS;
    }

    // TODO Check whether we're in video mode
    auto command = make_command_start_video_streaming();

    auto result = camera_result_from_command_result(_parent->send_command(command));
    if (result == Camera::Result::SUCCESS) {
        // Cache video stream info; app may query immediately next.
        _video_stream_info.reset();
        get_video_stream_info(_video_stream_info.info);
    }
    return result;
}

Camera::Result CameraImpl::stop_video_streaming()
{
    // TODO I think we need to maintain current state, whether we issued
    // video capture request or video streaming request, etc.We shouldn't
    // send stop video streaming if we've not started it!
    auto command = make_command_stop_video_streaming();

    auto result = camera_result_from_command_result(_parent->send_command(command));
    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        _video_stream_info.reset();
    }
    return result;
}

Camera::Result
CameraImpl::get_video_stream_info(Camera::VideoStreamInfo &info)
{
    Camera::Result result = Camera::Result::SUCCESS;
    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);

        do {
            if (!_video_stream_info.available) { // Request if not available
                auto command = make_command_request_video_stream_info();
                result = camera_result_from_command_result(_parent->send_command(command));
                if (result != Camera::Result::SUCCESS) {
                    LogErr() << "Failed to request video stream info";
                    break;
                }
                while (!_video_stream_info.available) { // Wait for video stream info
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }
            // Copy to application, once video stream info is available.
            info = _video_stream_info.info;
        } while (false);
    }

    return result;;
}

Camera::Result
CameraImpl::camera_result_from_command_result(MAVLinkCommands::Result command_result)
{
    switch (command_result) {
        case MAVLinkCommands::Result::SUCCESS:
            return Camera::Result::SUCCESS;
        case MAVLinkCommands::Result::NO_SYSTEM:
        // FALLTHROUGH
        case MAVLinkCommands::Result::CONNECTION_ERROR:
        // FALLTHROUGH
        case MAVLinkCommands::Result::BUSY:
            return Camera::Result::ERROR;
        case MAVLinkCommands::Result::COMMAND_DENIED:
            return Camera::Result::DENIED;
        case MAVLinkCommands::Result::TIMEOUT:
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
        default:
        // FALLTHROUGH
        case Camera::Mode::UNKNOWN:
            mavlink_mode = NAN;
            break;
    }

    auto cmd_set_camera_mode = make_command_set_camera_mode(mavlink_mode);

    _parent->send_command_async(cmd_set_camera_mode,
                                std::bind(&CameraImpl::receive_set_mode_command_result,
                                          this, _1, callback, mode));
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

    auto cmd_req_camera_settings = make_command_request_camera_settings();

    _parent->send_command_async(cmd_req_camera_settings,
                                std::bind(&CameraImpl::receive_get_mode_command_result,
                                          this, _1));
    _parent->register_timeout_handler(
        std::bind(&CameraImpl::get_mode_timeout_happened, this),
        1.0,
        &_get_mode.timeout_cookie);
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
        std::lock_guard<std::mutex> lock(_status.mutex);

        // If we're already trying to get the status, we need to wait.
        if (_status.callback != nullptr) {
            if (callback) {
                Camera::Status empty_status = {};
                callback(Camera::Result::IN_PROGRESS, empty_status);
                return;
            }
        }
        // Let's create a subscription for the (hopefully) incoming messages.
        _status.callback = callback;
    }

    auto cmd_req_camera_capture_stat = make_command_request_camera_capture_status();
    _parent->send_command_async(cmd_req_camera_capture_stat,
                                std::bind(&CameraImpl::receive_camera_capture_status_result, this, _1));

    auto cmd_req_storage_info = make_command_request_storage_info();
    _parent->send_command_async(cmd_req_storage_info,
                                std::bind(&CameraImpl::receive_storage_information_result, this, _1));

    _parent->register_timeout_handler(std::bind(&CameraImpl::status_timeout_happened, this),
                                      DEFAULT_TIMEOUT_S, &_status.timeout_cookie);
}

void CameraImpl::receive_camera_capture_status_result(MAVLinkCommands::Result result)
{
    std::lock_guard<std::mutex> lock(_status.mutex);

    if (_status.callback == nullptr) {
        // We're not expecting this message, let's ignore it.
        return;
    }

    if (result != MAVLinkCommands::Result::SUCCESS) {
        if (_status.callback) {
            Camera::Status empty_status = {};
            _status.callback(camera_result_from_command_result(result), empty_status);
        }
        // Something went wrong, we give up.
        _status.callback = nullptr;
    }

    _parent->refresh_timeout_handler(_status.timeout_cookie);
}

void CameraImpl::capture_info_async(Camera::capture_info_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_capture_info.mutex);

    _capture_info.callback = callback;
}

void CameraImpl::process_camera_capture_status(const mavlink_message_t &message)
{
    mavlink_camera_capture_status_t camera_capture_status;
    mavlink_msg_camera_capture_status_decode(&message, &camera_capture_status);

    {
        std::lock_guard<std::mutex> lock(_status.mutex);

        _status.data.video_on = (camera_capture_status.video_status == 1);
        _status.data.photo_interval_on = (camera_capture_status.image_status == 2 ||
                                          camera_capture_status.image_status == 3);
        _status.received_camera_capture_status = true;
    }

    check_status();
}

void CameraImpl::process_storage_information(const mavlink_message_t &message)
{
    mavlink_storage_information_t storage_information;
    mavlink_msg_storage_information_decode(&message, &storage_information);

    {
        std::lock_guard<std::mutex> lock(_status.mutex);
        if (storage_information.status == 0) {
            _status.data.storage_status = Camera::Status::StorageStatus::NOT_AVAILABLE;
        } else if (storage_information.status == 1) {
            _status.data.storage_status = Camera::Status::StorageStatus::UNFORMATTED;
        } else if (storage_information.status == 2) {
            _status.data.storage_status = Camera::Status::StorageStatus::FORMATTED;
        }
        _status.data.available_storage_mib = storage_information.available_capacity;
        _status.data.used_storage_mib = storage_information.used_capacity;
        _status.data.total_storage_mib = storage_information.total_capacity;
        _status.received_storage_information = true;
    }

    check_status();
}

void CameraImpl::process_camera_image_captured(const mavlink_message_t &message)
{
    mavlink_camera_image_captured_t image_captured;
    mavlink_msg_camera_image_captured_decode(&message, &image_captured);

    {
        std::lock_guard<std::mutex> lock(_capture_info.mutex);

        if (_capture_info.callback) {
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
            _capture_info.callback(capture_info);
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
        default:
            mode = Camera::Mode::UNKNOWN;
            break;
    }

    if (_camera_definition) {
        // This "parameter" needs to be manually set.
        MAVLinkParameters::ParamValue value;
        value.set_uint32(camera_settings.mode_id);
        _camera_definition->set_setting("CAM_MODE", value);
        refresh_params();
    }

    _get_mode.callback(Camera::Result::SUCCESS, mode);
    _get_mode.callback = nullptr;

    _parent->unregister_timeout_handler(_get_mode.timeout_cookie);
}

void CameraImpl::process_camera_information(const mavlink_message_t &message)
{
    mavlink_camera_information_t camera_information;
    mavlink_msg_camera_information_decode(&message, &camera_information);

    load_definition_file(camera_information.cam_definition_uri);
}

void CameraImpl::process_video_information(const mavlink_message_t &message)
{
    mavlink_video_stream_information_t received_video_info;
    mavlink_msg_video_stream_information_decode(&message, &received_video_info);

    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        _video_stream_info.info.status = \
                                         static_cast<Camera::VideoStreamInfo::Status>(received_video_info.status);
        auto &video_stream_info = _video_stream_info.info.settings;
        video_stream_info.frame_rate_hz = received_video_info.framerate;
        video_stream_info.resolution_h_pix = received_video_info.resolution_h;
        video_stream_info.resolution_v_pix = received_video_info.resolution_v;
        video_stream_info.bit_rate_b_s = received_video_info.bitrate;
        video_stream_info.rotation_deg = received_video_info.rotation;
        video_stream_info.uri = received_video_info.uri;

        _video_stream_info.available = true;
    }
}

void CameraImpl::check_status()
{
    std::lock_guard<std::mutex> lock(_status.mutex);

    if (_status.received_camera_capture_status &&
        _status.received_storage_information) {

        if (_status.callback) {
            _status.callback(Camera::Result::SUCCESS, _status.data);
            _status.callback = nullptr;
            _status.received_camera_capture_status = false;
            _status.received_storage_information = false;
            _parent->unregister_timeout_handler(_status.timeout_cookie);
        }
    }
}

void CameraImpl::status_timeout_happened()
{
    std::lock_guard<std::mutex> lock(_status.mutex);

    if (_status.callback) {
        // Send empty settings with timeout error.
        Camera::Status empty_status = {};
        _status.callback(Camera::Result::TIMEOUT, empty_status);
        // Then give up.
    }
    // Discard the subscription
    _status.callback = nullptr;
}

void CameraImpl::receive_command_result(MAVLinkCommands::Result command_result,
                                        const Camera::result_callback_t &callback)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    if (callback) {
        callback(camera_result);
    }
}


void CameraImpl::receive_storage_information_result(MAVLinkCommands::Result result)
{
    std::lock_guard<std::mutex> lock(_status.mutex);
    if (_status.callback == nullptr) {
        // We're not expecting this message, let's ignore it.
        return;
    }

    if (result != MAVLinkCommands::Result::SUCCESS) {
        if (_status.callback) {
            Camera::Status empty_status = {};
            _status.callback(camera_result_from_command_result(result), empty_status);
        }
        // Something went wrong, we give up.
        _status.callback = nullptr;
    }

    // TODO: decode and save values

    _parent->refresh_timeout_handler(_status.timeout_cookie);
}

void CameraImpl::receive_set_mode_command_result(MAVLinkCommands::Result command_result,
                                                 const Camera::mode_callback_t &callback,
                                                 Camera::Mode mode)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    if (callback) {
        callback(camera_result, mode);
    }

    if (command_result == MAVLinkCommands::Result::SUCCESS && _camera_definition) {
        // This "parameter" needs to be manually set.

        uint32_t mavlink_mode;
        switch (mode) {
            case Camera::Mode::PHOTO:
                mavlink_mode = CAMERA_MODE_IMAGE;
                break;
            case Camera::Mode::VIDEO:
                mavlink_mode = CAMERA_MODE_VIDEO;
                break;
            default:
            // FALLTHROUGH
            case Camera::Mode::UNKNOWN:
                LogWarn() << "Unknown camera mode";
                return;
        }

        MAVLinkParameters::ParamValue value;
        value.set_uint32(mavlink_mode);
        _camera_definition->set_setting("CAM_MODE", value);
        refresh_params();
    }
}

void CameraImpl::receive_get_mode_command_result(MAVLinkCommands::Result command_result)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    std::lock_guard<std::mutex> lock(_get_mode.mutex);

    if (camera_result == Camera::Result::SUCCESS) {
        // SUCESS is the normal case and means we keep waiting to receive the mode.
        _parent->refresh_timeout_handler(_get_mode.timeout_cookie);
        return;
    } else {
        if (_get_mode.callback) {
            Camera::Mode mode = Camera::Mode::UNKNOWN;
            _get_mode.callback(camera_result, mode);
        }
        _parent->unregister_timeout_handler(_get_mode.timeout_cookie);
    }
}

void CameraImpl::get_mode_timeout_happened()
{
    std::lock_guard<std::mutex> lock(_get_mode.mutex);

    if (_get_mode.callback) {
        _get_mode.callback(Camera::Result::TIMEOUT, Camera::Mode::UNKNOWN);
        _get_mode.callback = nullptr;
    }
}

void CameraImpl::load_definition_file(const std::string &uri)
{
    HttpLoader http_loader;
    std::string content;
    LogInfo() << "Downloading camera definition from: " << uri;
    if (!http_loader.download_text_sync(uri, content)) {
        LogErr() << "Failed to download camera definition.";
        return;
    }

    _camera_definition.reset(new CameraDefinition());
    _camera_definition->load_string(content);

    refresh_params();
}

bool CameraImpl::get_possible_settings(std::vector<std::string> &settings)
{
    settings.clear();

    if (!_camera_definition) {
        LogWarn() << "Error: no camera definition available yet";
        return false;
    }

    std::map<std::string, MAVLinkParameters::ParamValue> cd_settings {};
    _camera_definition->get_possible_settings(cd_settings);

    for (const auto &cd_setting : cd_settings) {
        if (cd_setting.first.compare("CAM_MODE") == 0) {
            // We ignore the mode param.
            continue;
        }

        settings.push_back(cd_setting.first);
    }

    return settings.size() > 0;
}

bool CameraImpl::get_possible_options(const std::string &setting_name,
                                      std::vector<std::string> &options)
{
    options.clear();

    if (!_camera_definition) {
        LogWarn() << "Error: no camera definition available yet";
        return false;
    }

    std::vector<MAVLinkParameters::ParamValue> values;
    if (!_camera_definition->get_possible_options(setting_name, values)) {
        return false;
    }

    for (const auto &value : values) {
        std::stringstream ss {};
        ss << value;
        options.push_back(ss.str());
    }

    return options.size() > 0;
}

void CameraImpl::set_option_async(const std::string &setting,
                                  const std::string &option,
                                  const Camera::result_callback_t &callback)
{
    if (!_camera_definition) {
        LogWarn() << "Error: no camera defnition available yet.";
        if (callback) {
            callback(Camera::Result::ERROR);
        }
        return;
    }

    // We get it first so that we have the type of the param value.
    MAVLinkParameters::ParamValue value;
    if (!_camera_definition->get_option_value(setting, option, value)) {
        if (callback) {
            callback(Camera::Result::ERROR);
        }
        return;
    }

    std::vector<MAVLinkParameters::ParamValue> possible_values;
    _camera_definition->get_possible_options(setting, possible_values);
    bool allowed = false;
    for (const auto &possible_value : possible_values) {
        if (value == possible_value) {
            allowed = true;
        }
    }
    if (!allowed) {
        LogErr() << "Setting " << setting << "(" << option << ") not allowed";
        if (callback) {
            callback(Camera::Result::ERROR);
        }
        return;
    }

    _parent->set_param_async(setting, value,
    [this, callback, setting, value](bool success) {
        if (success) {
            if (this->_camera_definition) {

                _camera_definition->set_setting(setting, value);
                refresh_params();
            }
            if (callback) {
                callback(Camera::Result::SUCCESS);
            }
        } else {
            if (callback) {
                callback(Camera::Result::ERROR);
            }
        }
    },
    true);

}

void CameraImpl::get_option_async(const std::string &setting,
                                  const Camera::get_option_callback_t &callback)
{
    if (!_camera_definition) {
        LogWarn() << "Error: no camera defnition available yet.";
        if (callback) {
            callback(Camera::Result::ERROR, "");
        }
        return;
    }

    MAVLinkParameters::ParamValue value;
    // We should have this cached and don't need to get the param.
    if (_camera_definition->get_setting(setting, value)) {
        if (callback) {
            callback(Camera::Result::SUCCESS, value.get_string());
        }
    } else {
        // If this still happens, we request the param, but also complain.
        LogWarn() << "The param was probably outdated, trying to fetch it";
        _parent->get_param_async(setting,
        [setting, this](bool success, MAVLinkParameters::ParamValue value) {
            if (!success) {
                LogWarn() << "Fetching the param failed";
                return;
            }
            // We need to check again by the time this callback runs
            if (!this->_camera_definition) {
                return;
            }
            this->_camera_definition->set_setting(setting, value);
        }, true);

        // At this point it might be a good idea to refresh but it's a bit scary
        // as the stack keeps growing at this point.
        //refresh_params();
    }
}

void CameraImpl::refresh_params()
{
    if (!_camera_definition) {
        return;
    }

    std::vector<std::string> params {};
    if (!_camera_definition->get_unknown_params(params)) {
        return;
    }

    for (const auto &param : params) {
        std::string param_name = param;
        _parent->get_param_async(param_name,
        [param_name, this](bool success, MAVLinkParameters::ParamValue value) {
            if (!success) {
                return;
            }
            // We need to check again by the time this callback runs
            if (!this->_camera_definition) {
                return;
            }
            this->_camera_definition->set_setting(param_name, value);
        }, true);
    }
}

void CameraImpl::invalidate_params()
{
    if (!_camera_definition) {
        return;
    }

    _camera_definition->set_all_params_unknown();
}

} // namespace dronecore
