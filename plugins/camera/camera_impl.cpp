#include "camera_impl.h"
#include "camera_definition.h"
#include "system.h"
#include "global_include.h"
#include "http_loader.h"
#if !defined(WINDOWS)
#include "camera_definition_files.h"
#endif
#include <functional>
#include <cmath>
#include <sstream>

namespace dronecode_sdk {

using namespace std::placeholders; // for `_1`

CameraImpl::CameraImpl(System &system) : PluginImplBase(system)
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

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_FLIGHT_INFORMATION,
        std::bind(&CameraImpl::process_flight_information, this, _1),
        this);
}

void CameraImpl::deinit()
{
    _parent->remove_call_every(_status.call_every_cookie);
    _parent->unregister_all_mavlink_message_handlers(this);

    {
        std::lock_guard<std::mutex> lock(_status.mutex);
        _status.callback = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(_get_mode.mutex);
        _get_mode.callback = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(_capture_info.mutex);
        _capture_info.callback = nullptr;
    }

    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        _video_stream_info.callback = nullptr;
    }
}

void CameraImpl::enable()
{
    refresh_params();
    request_flight_information();

    auto command_camera_info = make_command_request_camera_info();
    _parent->send_command_async(command_camera_info, nullptr);

    _parent->add_call_every(
        [this]() { request_flight_information(); }, 10.0, &_flight_information_call_every_cookie);
}

void CameraImpl::disable()
{
    invalidate_params();
    _parent->remove_call_every(_flight_information_call_every_cookie);
}

Camera::Result CameraImpl::select_camera(unsigned id)
{
    if (id > 6) {
        return Camera::Result::WRONG_ARGUMENT;
    }

    // camera component IDs go from 100 to 105.
    _component_id = MAV_COMP_ID_CAMERA + id;

    // We should probably reload everything to make sure the
    // correct  camera is initialized.
    disable();
    enable();

    return Camera::Result::SUCCESS;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_request_camera_info()
{
    MAVLinkCommands::CommandLong command_camera_info{};

    command_camera_info.command = MAV_CMD_REQUEST_CAMERA_INFORMATION;
    command_camera_info.params.param1 = 1.0f; // Request it
    command_camera_info.target_component_id = _component_id;
    ;

    return command_camera_info;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_take_photo(float interval_s,
                                                                 float no_of_photos)
{
    MAVLinkCommands::CommandLong cmd_take_photo{};

    cmd_take_photo.command = MAV_CMD_IMAGE_START_CAPTURE;
    cmd_take_photo.params.param1 = 0.0f; // Reserved, set to 0
    cmd_take_photo.params.param2 = interval_s;
    cmd_take_photo.params.param3 = no_of_photos;
    cmd_take_photo.params.param4 = float(_capture.sequence++);
    cmd_take_photo.target_component_id = _component_id;

    return cmd_take_photo;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_stop_photo()
{
    MAVLinkCommands::CommandLong cmd_stop_photo{};

    cmd_stop_photo.command = MAV_CMD_IMAGE_STOP_CAPTURE;
    cmd_stop_photo.target_component_id = _component_id;

    return cmd_stop_photo;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_start_video(float capture_status_rate_hz)
{
    MAVLinkCommands::CommandLong cmd_start_video{};

    cmd_start_video.command = MAV_CMD_VIDEO_START_CAPTURE;
    cmd_start_video.params.param1 = 0.f; // Reserved, set to 0
    cmd_start_video.params.param2 = capture_status_rate_hz;
    cmd_start_video.target_component_id = _component_id;

    return cmd_start_video;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_stop_video()
{
    MAVLinkCommands::CommandLong cmd_stop_video{};

    cmd_stop_video.command = MAV_CMD_VIDEO_STOP_CAPTURE;
    cmd_stop_video.params.param1 = 0.f; // Reserved, set to 0
    cmd_stop_video.target_component_id = _component_id;

    return cmd_stop_video;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_set_camera_mode(float mavlink_mode)
{
    MAVLinkCommands::CommandLong cmd_set_camera_mode{};

    cmd_set_camera_mode.command = MAV_CMD_SET_CAMERA_MODE;
    cmd_set_camera_mode.params.param1 = 0.0f; // Reserved, set to 0
    cmd_set_camera_mode.params.param2 = mavlink_mode;
    cmd_set_camera_mode.target_component_id = _component_id;

    return cmd_set_camera_mode;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_request_camera_settings()
{
    MAVLinkCommands::CommandLong cmd_req_camera_settings{};

    cmd_req_camera_settings.command = MAV_CMD_REQUEST_CAMERA_SETTINGS;
    cmd_req_camera_settings.params.param1 = 1.f; // Request it
    cmd_req_camera_settings.target_component_id = _component_id;

    return cmd_req_camera_settings;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_request_camera_capture_status()
{
    MAVLinkCommands::CommandLong cmd_req_camera_cap_stat{};

    cmd_req_camera_cap_stat.command = MAV_CMD_REQUEST_CAMERA_CAPTURE_STATUS;
    cmd_req_camera_cap_stat.params.param1 = 1.0f; // Request it
    cmd_req_camera_cap_stat.target_component_id = _component_id;

    return cmd_req_camera_cap_stat;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_request_storage_info()
{
    MAVLinkCommands::CommandLong cmd_req_storage_info{};

    cmd_req_storage_info.command = MAV_CMD_REQUEST_STORAGE_INFORMATION;
    cmd_req_storage_info.params.param1 = 0.f; // Reserved, set to 0
    cmd_req_storage_info.params.param2 = 1.f; // Request it
    cmd_req_storage_info.target_component_id = _component_id;

    return cmd_req_storage_info;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_start_video_streaming()
{
    MAVLinkCommands::CommandLong cmd_start_video_streaming{};

    cmd_start_video_streaming.command = MAV_CMD_VIDEO_START_STREAMING;
    cmd_start_video_streaming.target_component_id = _component_id;

    return cmd_start_video_streaming;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_stop_video_streaming()
{
    MAVLinkCommands::CommandLong cmd_stop_video_streaming{};

    cmd_stop_video_streaming.command = MAV_CMD_VIDEO_STOP_STREAMING;
    cmd_stop_video_streaming.target_component_id = _component_id;

    return cmd_stop_video_streaming;
}

mavlink_message_t
CameraImpl::make_message_set_video_stream_settings(const Camera::VideoStreamSettings &settings)
{
    mavlink_message_t msg;

    mavlink_msg_set_video_stream_settings_pack(GCSClient::system_id,
                                               GCSClient::component_id,
                                               &msg,
                                               _parent->get_system_id(),
                                               _component_id,
                                               _component_id, // TODO: Is it right ?
                                               settings.frame_rate_hz,
                                               settings.horizontal_resolution_pix,
                                               settings.vertical_resolution_pix,
                                               settings.bit_rate_b_s,
                                               settings.rotation_deg,
                                               settings.uri.c_str());

    return msg;
}

MAVLinkCommands::CommandLong CameraImpl::make_command_request_video_stream_info()
{
    MAVLinkCommands::CommandLong cmd_req_video_stream_info{};

    cmd_req_video_stream_info.command = MAV_CMD_REQUEST_VIDEO_STREAM_INFORMATION;
    cmd_req_video_stream_info.params.param2 = 1.0f;
    cmd_req_video_stream_info.target_component_id = _component_id;

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

    return camera_result_from_command_result(_parent->send_command(cmd_take_photo_time_lapse));
}

Camera::Result CameraImpl::stop_photo_interval()
{
    auto cmd_stop_photo_interval = make_command_stop_photo();

    return camera_result_from_command_result(_parent->send_command(cmd_stop_photo_interval));
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

    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        _video_stream_info.info.status = Camera::VideoStreamInfo::Status::NOT_RUNNING;
    }

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

Camera::Information CameraImpl::get_information()
{
    std::lock_guard<std::mutex> lock(_information.mutex);

    return _information.data;
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
        // TODO: check if we can/should do that.
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
        // TODO: check if we can/should do that.
        _video_stream_info.info.status = Camera::VideoStreamInfo::Status::NOT_RUNNING;
    }
    return result;
}

Camera::Result CameraImpl::get_video_stream_info(Camera::VideoStreamInfo &info)
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    get_video_stream_info_async([prom](Camera::Result result, Camera::VideoStreamInfo info_gotten) {
        UNUSED(info_gotten);
        prom->set_value(result);
    });

    auto status = ret.wait_for(std::chrono::seconds(5));

    if (status == std::future_status::ready) {
        info = _video_stream_info.info;
        return Camera::Result::SUCCESS;
    } else {
        return Camera::Result::TIMEOUT;
    }
}

void CameraImpl::get_video_stream_info_async(
    const Camera::get_video_stream_info_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
    if (_video_stream_info.callback) {
        if (callback) {
            callback(Camera::Result::IN_PROGRESS, _video_stream_info.info);
        }
        return;
    }

    _video_stream_info.callback = callback;

    auto command = make_command_request_video_stream_info();
    _parent->send_command_async(command, [this](MAVLinkCommands::Result result, float progress) {
        UNUSED(progress);
        Camera::Result camera_result = camera_result_from_command_result(result);
        if (camera_result != Camera::Result::SUCCESS) {
            if (_video_stream_info.callback) {
                _video_stream_info.callback(camera_result, _video_stream_info.info);
            }
            return;
        }
        _video_stream_info.callback = nullptr;
        _parent->register_timeout_handler(
            std::bind(&CameraImpl::get_video_stream_info_timeout, this),
            1.0,
            &_video_stream_info.timeout_cookie);
    });
}

void CameraImpl::get_video_stream_info_timeout()
{
    std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
    LogErr() << "Getting video stream info timed out.";

    _video_stream_info.available = false;
    if (_video_stream_info.callback) {
        _video_stream_info.callback(Camera::Result::TIMEOUT, _video_stream_info.info);
        _video_stream_info.callback = nullptr;
    }
}

void CameraImpl::subscribe_video_stream_info(
    const Camera::subscribe_video_stream_info_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_video_stream_info.mutex);

    _subscribe_video_stream_info_callback = callback;
    if (callback) {
        _parent->add_call_every([this]() { get_video_stream_info_async(nullptr); },
                                1.0,
                                &_video_stream_info.call_every_cookie);
    } else {
        _parent->remove_call_every(_video_stream_info.call_every_cookie);
    }
}

Camera::Result
CameraImpl::camera_result_from_command_result(const MAVLinkCommands::Result command_result)
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

Camera::Result CameraImpl::set_mode(const Camera::Mode mode)
{
    const float mavlink_mode = to_mavlink_camera_mode(mode);
    auto cmd_set_camera_mode = make_command_set_camera_mode(mavlink_mode);
    const auto command_result = _parent->send_command(cmd_set_camera_mode);
    const auto camera_result = camera_result_from_command_result(command_result);

    if (camera_result == Camera::Result::SUCCESS) {
        notify_mode(mode);
        if (_camera_definition != nullptr) {
            save_camera_mode(mavlink_mode);
        }
    }

    return camera_result;
}

void CameraImpl::save_camera_mode(const float mavlink_camera_mode)
{
    if (!std::isfinite(mavlink_camera_mode)) {
        LogWarn() << "Can't save NAN as camera mode";
        return;
    }

    MAVLinkParameters::ParamValue value;
    value.set_uint32(uint32_t(mavlink_camera_mode));
    _camera_definition->set_setting("CAM_MODE", value);
    refresh_params();
}

float CameraImpl::to_mavlink_camera_mode(const Camera::Mode mode) const
{
    switch (mode) {
        case Camera::Mode::PHOTO:
            return CAMERA_MODE_IMAGE;
        case Camera::Mode::VIDEO:
            return CAMERA_MODE_VIDEO;
        default:
        case Camera::Mode::UNKNOWN:
            return NAN;
    }
}

void CameraImpl::set_mode_async(const Camera::Mode mode, const Camera::mode_callback_t &callback)
{
    const auto mavlink_mode = to_mavlink_camera_mode(mode);
    auto cmd_set_camera_mode = make_command_set_camera_mode(mavlink_mode);

    _parent->send_command_async(
        cmd_set_camera_mode,
        std::bind(&CameraImpl::receive_set_mode_command_result, this, _1, callback, mode));
}

void CameraImpl::get_mode_async(Camera::mode_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_get_mode.mutex);

    if (_get_mode.callback != nullptr) {
        if (callback) {
            callback(Camera::Result::BUSY, Camera::Mode::UNKNOWN);
            return;
        }
    }
    _get_mode.callback = callback;

    auto cmd_req_camera_settings = make_command_request_camera_settings();

    _parent->send_command_async(cmd_req_camera_settings,
                                std::bind(&CameraImpl::receive_get_mode_command_result, this, _1));
    _parent->register_timeout_handler(
        std::bind(&CameraImpl::get_mode_timeout_happened, this), 1.0, &_get_mode.timeout_cookie);
}

void CameraImpl::subscribe_mode(const Camera::subscribe_mode_callback_t callback)
{
    _subscribe_mode_callback = callback;
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
    {
        std::lock_guard<std::mutex> lock(_status.mutex);

        // If we're already trying to get the status, we need to wait.
        if (_status.callback != nullptr) {
            if (callback) {
                Camera::Status empty_status = {};
                callback(Camera::Result::BUSY, empty_status);
                return;
            }
        }
        // Let's create a subscription for the (hopefully) incoming messages.
        _status.callback = callback;
    }

    auto cmd_req_camera_capture_stat = make_command_request_camera_capture_status();
    _parent->send_command_async(
        cmd_req_camera_capture_stat,
        std::bind(&CameraImpl::receive_camera_capture_status_result, this, _1));

    auto cmd_req_storage_info = make_command_request_storage_info();
    _parent->send_command_async(
        cmd_req_storage_info, std::bind(&CameraImpl::receive_storage_information_result, this, _1));

    if (callback) {
        _parent->register_timeout_handler(std::bind(&CameraImpl::status_timeout_happened, this),
                                          DEFAULT_TIMEOUT_S,
                                          &_status.timeout_cookie);
    }
}

void CameraImpl::subscribe_status(const Camera::subscribe_status_callback_t callback)
{
    std::lock_guard<std::mutex> lock(_status.mutex);

    _subscribe_status_callback = callback;
    if (callback) {
        _parent->add_call_every(
            [this]() { get_status_async(nullptr); }, 1.0, &_status.call_every_cookie);
    } else {
        _parent->remove_call_every(_status.call_every_cookie);
    }
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

void CameraImpl::subscribe_capture_info(Camera::capture_info_callback_t callback)
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
        _status.data.photo_interval_on =
            (camera_capture_status.image_status == 2 || camera_capture_status.image_status == 3);
        _status.received_camera_capture_status = true;
        _status.data.recording_time_s = float(camera_capture_status.recording_time_ms) / 1e3f;
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
            capture_info.attitude_quaternion.w = image_captured.q[0];
            capture_info.attitude_quaternion.x = image_captured.q[1];
            capture_info.attitude_quaternion.y = image_captured.q[2];
            capture_info.attitude_quaternion.z = image_captured.q[3];
            capture_info.attitude_euler_angle =
                to_euler_angle_from_quaternion(capture_info.attitude_quaternion);
            capture_info.file_url = std::string(image_captured.file_url);
            capture_info.success = (image_captured.capture_result == 1);
            capture_info.index = image_captured.image_index;
            notify_capture_info(capture_info);
        }
    }
}

Camera::CaptureInfo::EulerAngle
CameraImpl::to_euler_angle_from_quaternion(Camera::CaptureInfo::Quaternion quaternion)
{
    auto &q = quaternion;

    // FIXME: This is duplicated from telemetry/math_conversions.cpp.
    Camera::CaptureInfo::EulerAngle euler_angle{
        to_deg_from_rad(
            atan2f(2.0f * (q.w * q.x + q.y * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y))),
        to_deg_from_rad(asinf(2.0f * (q.w * q.y - q.z * q.x))),
        to_deg_from_rad(
            atan2f(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.y * q.y + q.z * q.z)))};
    return euler_angle;
}

void CameraImpl::notify_capture_info(Camera::CaptureInfo capture_info)
{
    // Make a copy because it is passed to the thread pool
    const auto capture_info_callback = _capture_info.callback;

    if (capture_info_callback == nullptr) {
        return;
    }

    _parent->call_user_callback(
        [capture_info, capture_info_callback]() { capture_info_callback(capture_info); });
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

    const auto mode = to_camera_mode(camera_settings.mode_id);

    if (_camera_definition) {
        // This "parameter" needs to be manually set.
        save_camera_mode(camera_settings.mode_id);
    }

    _get_mode.callback(Camera::Result::SUCCESS, mode);
    _get_mode.callback = nullptr;

    _parent->unregister_timeout_handler(_get_mode.timeout_cookie);
}

Camera::Mode CameraImpl::to_camera_mode(const uint8_t mavlink_camera_mode) const
{
    switch (mavlink_camera_mode) {
        case CAMERA_MODE_IMAGE:
            return Camera::Mode::PHOTO;
        case CAMERA_MODE_VIDEO:
            return Camera::Mode::VIDEO;
        default:
            return Camera::Mode::UNKNOWN;
    }
}

void CameraImpl::process_camera_information(const mavlink_message_t &message)
{
    mavlink_camera_information_t camera_information;
    mavlink_msg_camera_information_decode(&message, &camera_information);

    {
        std::lock_guard<std::mutex> lock(_information.mutex);
        _information.data.vendor_name = (char *)(camera_information.vendor_name);
        _information.data.model_name = (char *)(camera_information.model_name);
    }

    std::string content{};
    bool found_content = false;

#if !defined(WINDOWS)
    // TODO: we might also try to support the correct version of the xml files.
    if (strcmp((const char *)(camera_information.vendor_name), "Yuneec") == 0) {
        if (strcmp((const char *)(camera_information.model_name), "E90") == 0) {
            LogInfo() << "Using cached file for Yuneec E90.";
            content = e90xml;
            found_content = true;
        } else if (strcmp((const char *)(camera_information.model_name), "E50") == 0) {
            LogInfo() << "Using cached file for Yuneec E50.";
            content = e50xml;
            found_content = true;
        } else if (strcmp((const char *)(camera_information.model_name), "CGOET") == 0) {
            LogInfo() << "Using cached file for Yuneec ET.";
            content = cgoetxml;
            found_content = true;
        }
    } else {
#endif
        found_content = load_definition_file(camera_information.cam_definition_uri, content);
#if !defined(WINDOWS)
    }
#endif

    if (found_content) {
        _camera_definition.reset(new CameraDefinition());
        _camera_definition->load_string(content);
        refresh_params();
    }
}

void CameraImpl::process_video_information(const mavlink_message_t &message)
{
    mavlink_video_stream_information_t received_video_info;
    mavlink_msg_video_stream_information_decode(&message, &received_video_info);

    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        _video_stream_info.info.status =
            static_cast<Camera::VideoStreamInfo::Status>(received_video_info.status);
        auto &video_stream_info = _video_stream_info.info.settings;
        video_stream_info.frame_rate_hz = received_video_info.framerate;
        video_stream_info.horizontal_resolution_pix = received_video_info.resolution_h;
        video_stream_info.vertical_resolution_pix = received_video_info.resolution_v;
        video_stream_info.bit_rate_b_s = received_video_info.bitrate;
        video_stream_info.rotation_deg = received_video_info.rotation;
        video_stream_info.uri = received_video_info.uri;
        _video_stream_info.available = true;

        if (_video_stream_info.callback) {
            _parent->unregister_timeout_handler(_video_stream_info.timeout_cookie);
            _video_stream_info.callback(Camera::Result::SUCCESS, _video_stream_info.info);
            _video_stream_info.callback = nullptr;
        }
        notify_video_stream_info();
    }
}

void CameraImpl::process_flight_information(const mavlink_message_t &message)
{
    mavlink_flight_information_t flight_information;
    mavlink_msg_flight_information_decode(&message, &flight_information);

    std::stringstream folder_name_stream;
    {
        std::lock_guard<std::mutex> information_lock(_information.mutex);

        // For Yuneec cameras, the folder names can be derived from the flight ID,
        // starting at 100 up to 999.
        if (_information.data.vendor_name == "Yuneec" && _information.data.model_name == "E90") {
            folder_name_stream << (101 + flight_information.flight_uuid % 899) << "E90HD";
        } else if (_information.data.vendor_name == "Yuneec" &&
                   _information.data.model_name == "E50") {
            folder_name_stream << (101 + flight_information.flight_uuid % 899) << "E50HD";
        } else if (_information.data.vendor_name == "Yuneec" &&
                   _information.data.model_name == "CGOET") {
            folder_name_stream << (101 + flight_information.flight_uuid % 899) << "CGOET";
        } else {
            // Folder name unknown
        }
    }

    {
        std::lock_guard<std::mutex> lock(_status.mutex);
        _status.data.media_folder_name = folder_name_stream.str();
    }
}

void CameraImpl::notify_video_stream_info()
{
    if (_subscribe_video_stream_info_callback) {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        _parent->call_user_callback(
            [this]() { _subscribe_video_stream_info_callback(_video_stream_info.info); });
    }
}

void CameraImpl::check_status()
{
    std::lock_guard<std::mutex> lock(_status.mutex);

    if (_status.received_camera_capture_status && _status.received_storage_information) {
        if (_status.callback) {
            _status.callback(Camera::Result::SUCCESS, _status.data);
            _status.callback = nullptr;
            _status.received_camera_capture_status = false;
            _status.received_storage_information = false;
            _parent->unregister_timeout_handler(_status.timeout_cookie);
        }
        notify_status(_status.data);
    }
}

void CameraImpl::notify_status(Camera::Status status)
{
    // Make a copy because it is passed to the thread pool
    const auto status_callback = _subscribe_status_callback;

    if (status_callback == nullptr) {
        return;
    }

    _parent->call_user_callback([status, status_callback]() { status_callback(status); });
}

void CameraImpl::status_timeout_happened()
{
    std::lock_guard<std::mutex> lock(_status.mutex);

    if (_status.callback) {
        // Send empty settings with timeout error.
        Camera::Status empty_status = {};
        _status.callback(Camera::Result::TIMEOUT, empty_status);
        // Discard the subscription
        _status.callback = nullptr;
    }
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

void CameraImpl::receive_set_mode_command_result(const MAVLinkCommands::Result command_result,
                                                 const Camera::mode_callback_t &callback,
                                                 const Camera::Mode mode)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    if (callback) {
        callback(camera_result, mode);
    }

    if (command_result == MAVLinkCommands::Result::SUCCESS && _camera_definition) {
        // This "parameter" needs to be manually set.

        const auto mavlink_mode = to_mavlink_camera_mode(mode);

        if (std::isnan(mavlink_mode)) {
            LogWarn() << "Unknown camera mode";
            return;
        }

        notify_mode(mode);
        save_camera_mode(mavlink_mode);
    }
}

void CameraImpl::notify_mode(const Camera::Mode mode)
{
    // Make a copy because it is passed to the thread pool
    const auto mode_callback = _subscribe_mode_callback;

    if (mode_callback == nullptr) {
        return;
    }

    _parent->call_user_callback([mode, mode_callback]() { mode_callback(mode); });
}

void CameraImpl::receive_get_mode_command_result(MAVLinkCommands::Result command_result)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    std::lock_guard<std::mutex> lock(_get_mode.mutex);

    if (camera_result == Camera::Result::SUCCESS) {
        // SUCCESS is the normal case and means we keep waiting to receive the mode.
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

bool CameraImpl::load_definition_file(const std::string &uri, std::string &content)
{
    HttpLoader http_loader;
    LogInfo() << "Downloading camera definition from: " << uri;
    if (!http_loader.download_text_sync(uri, content)) {
        LogErr() << "Failed to download camera definition.";
        return false;
    }

    return true;
}

bool CameraImpl::get_possible_setting_options(std::vector<std::string> &settings)
{
    settings.clear();

    if (!_camera_definition) {
        LogWarn() << "Error: no camera definition available yet";
        return false;
    }

    std::map<std::string, MAVLinkParameters::ParamValue> cd_settings{};
    _camera_definition->get_possible_settings(cd_settings);

    for (const auto &cd_setting : cd_settings) {
        if (cd_setting.first == "CAM_MODE") {
            // We ignore the mode param.
            continue;
        }

        settings.push_back(cd_setting.first);
    }

    return settings.size() > 0;
}

bool CameraImpl::get_possible_options(const std::string &setting_id,
                                      std::vector<Camera::Option> &options)
{
    options.clear();

    if (!_camera_definition) {
        LogWarn() << "Error: no camera definition available yet";
        return false;
    }

    std::vector<MAVLinkParameters::ParamValue> values;
    if (!_camera_definition->get_possible_options(setting_id, values)) {
        return false;
    }

    for (const auto &value : values) {
        std::stringstream ss{};
        ss << value;
        Camera::Option option{};
        option.option_id = ss.str();
        get_option_str(setting_id, option.option_id, option.option_description);
        options.push_back(option);
    }

    return options.size() > 0;
}

void CameraImpl::set_option_async(const std::string &setting_id,
                                  const Camera::Option &option,
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
    if (!_camera_definition->get_option_value(setting_id, option.option_id, value)) {
        if (callback) {
            callback(Camera::Result::ERROR);
        }
        return;
    }

    std::vector<MAVLinkParameters::ParamValue> possible_values;
    _camera_definition->get_possible_options(setting_id, possible_values);
    bool allowed = false;
    for (const auto &possible_value : possible_values) {
        if (value == possible_value) {
            allowed = true;
        }
    }
    if (!allowed) {
        LogErr() << "Setting " << setting_id << "(" << option.option_id << ") not allowed";
        if (callback) {
            callback(Camera::Result::ERROR);
        }
        return;
    }

    _parent->set_param_async(setting_id,
                             value,
                             [this, callback, setting_id, value](MAVLinkParameters::Result result) {
                                 if (result == MAVLinkParameters::Result::SUCCESS) {
                                     if (this->_camera_definition) {
                                         _camera_definition->set_setting(setting_id, value);
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

Camera::Result CameraImpl::get_option(const std::string &setting_id, Camera::Option &option)
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    get_option_async(setting_id,
                     [prom, &option](Camera::Result result, const Camera::Option &option_gotten) {
                         prom->set_value(result);
                         if (result == Camera::Result::SUCCESS) {
                             option = option_gotten;
                         }
                     });

    auto status = ret.wait_for(std::chrono::seconds(1));

    if (status == std::future_status::ready) {
        return Camera::Result::SUCCESS;
    } else {
        return Camera::Result::TIMEOUT;
    }
}

void CameraImpl::get_option_async(const std::string &setting_id,
                                  const Camera::get_option_callback_t &callback)
{
    if (!_camera_definition) {
        LogWarn() << "Error: no camera defnition available yet.";
        if (callback) {
            Camera::Option empty_option{};
            callback(Camera::Result::ERROR, empty_option);
        }
        return;
    }

    MAVLinkParameters::ParamValue value;
    // We should have this cached and don't need to get the param.
    if (_camera_definition->get_setting(setting_id, value)) {
        if (callback) {
            Camera::Option new_option{};
            new_option.option_id = value.get_string();
            get_option_str(setting_id, new_option.option_id, new_option.option_description);
            callback(Camera::Result::SUCCESS, new_option);
        }
    } else {
        // If this still happens, we request the param, but also complain.
        LogWarn() << "Setting '" << setting_id << "' not found.";
        if (callback) {
            Camera::Option no_option{};
            callback(Camera::Result::ERROR, no_option);
        }
    }
}

void CameraImpl::subscribe_current_settings(
    const Camera::subscribe_current_settings_callback_t &callback)
{
    _subscribe_current_settings_callback = callback;
    notify_current_settings();
}

void CameraImpl::subscribe_possible_setting_options(
    const Camera::subscribe_possible_setting_options_callback_t &callback)
{
    _subscribe_possible_setting_options_callback = callback;
    notify_possible_setting_options();
}

void CameraImpl::notify_current_settings()
{
    if (!_subscribe_current_settings_callback) {
        return;
    }

    if (!_camera_definition) {
        LogErr() << "notify_current_settings has no camera definition";
        return;
    }

    std::vector<Camera::Setting> current_settings{};
    std::vector<std::string> possible_setting_options{};
    if (!get_possible_setting_options(possible_setting_options)) {
        LogErr() << "Could not get possible settings in current options subscription.";
        return;
    }

    for (auto &possible_setting : possible_setting_options) {
        // use the cache for this, presumably we updated it right before.
        MAVLinkParameters::ParamValue value;
        if (_camera_definition->get_setting(possible_setting, value)) {
            Camera::Setting setting{};
            setting.setting_id = possible_setting;
            get_setting_str(setting.setting_id, setting.setting_description);
            setting.option.option_id = value.get_string();
            get_option_str(
                setting.setting_id, setting.option.option_id, setting.option.option_description);
            current_settings.push_back(setting);
        }
    }
    _subscribe_current_settings_callback(current_settings);
}

void CameraImpl::notify_possible_setting_options()
{
    if (!_subscribe_possible_setting_options_callback) {
        return;
    }

    if (!_camera_definition) {
        LogErr() << "notify_possible_setting_options has no camera definition";
        return;
    }

    std::vector<Camera::SettingOptions> possible_setting_options{};

    std::vector<std::string> possible_settings{};
    if (!get_possible_setting_options(possible_settings)) {
        LogErr() << "Could not get possible settings in possible options subscription.";
        return;
    }

    for (auto &possible_setting : possible_settings) {
        Camera::SettingOptions setting_options{};
        setting_options.setting_id = possible_setting;
        get_setting_str(setting_options.setting_id, setting_options.setting_description);
        get_possible_options(possible_setting, setting_options.options);
        possible_setting_options.push_back(setting_options);
    }

    _subscribe_possible_setting_options_callback(possible_setting_options);
}

void CameraImpl::refresh_params()
{
    if (!_camera_definition) {
        return;
    }

    std::vector<std::pair<std::string, MAVLinkParameters::ParamValue>> params;
    _camera_definition->get_unknown_params(params);
    if (params.size() == 0) {
        // We're assuming that we changed one option and this did not cause
        // any other possible settings to change. However, we still would
        // like to notify the current settings with this one change.
        notify_current_settings();
        return;
    }

    unsigned count = 0;
    for (const auto &param : params) {
        const std::string &param_name = param.first;
        const MAVLinkParameters::ParamValue &param_value_type = param.second;
        const bool is_last = (count + 1 == params.size());
        _parent->get_param_async(param_name,
                                 param_value_type,
                                 [param_name, is_last, this](MAVLinkParameters::Result result,
                                                             MAVLinkParameters::ParamValue value) {
                                     if (result != MAVLinkParameters::Result::SUCCESS) {
                                         return;
                                     }
                                     // We need to check again by the time this callback runs
                                     if (!this->_camera_definition) {
                                         return;
                                     }
                                     this->_camera_definition->set_setting(param_name, value);

                                     if (is_last) {
                                         notify_current_settings();
                                         notify_possible_setting_options();
                                     }
                                 },
                                 true);
        ++count;
    }
}

void CameraImpl::invalidate_params()
{
    if (!_camera_definition) {
        return;
    }

    _camera_definition->set_all_params_unknown();
}

bool CameraImpl::get_setting_str(const std::string &setting_id, std::string &description)
{
    if (!_camera_definition) {
        return false;
    }

    return _camera_definition->get_setting_str(setting_id, description);
}

bool CameraImpl::get_option_str(const std::string &setting_id,
                                const std::string &option_id,
                                std::string &description)
{
    if (!_camera_definition) {
        return false;
    }

    return _camera_definition->get_option_str(setting_id, option_id, description);
}

void CameraImpl::request_flight_information()
{
    MAVLinkCommands::CommandLong command_flight_information{};

    command_flight_information.command = MAV_CMD_REQUEST_FLIGHT_INFORMATION;
    command_flight_information.params.param1 = 1.0f; // Request it
    command_flight_information.target_component_id = MAV_COMP_ID_AUTOPILOT1;

    _parent->send_command_async(command_flight_information, nullptr);
}

Camera::Result CameraImpl::format_storage()
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    format_storage_async([prom](Camera::Result result) { prom->set_value(result); });

    return ret.get();
}

void CameraImpl::format_storage_async(Camera::result_callback_t callback)
{
    MAVLinkCommands::CommandLong cmd_format{};

    cmd_format.command = MAV_CMD_STORAGE_FORMAT;
    cmd_format.params.param1 = 1.0f; // storage ID
    cmd_format.params.param2 = 1.0f; // format
    cmd_format.target_component_id = _component_id;

    _parent->send_command_async(cmd_format,
                                std::bind(&CameraImpl::receive_command_result, _1, callback));
}

} // namespace dronecode_sdk
