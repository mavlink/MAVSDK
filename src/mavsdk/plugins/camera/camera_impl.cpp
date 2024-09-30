#include "camera_impl.h"
#include "camera_definition.h"
#include "system.h"
#include "http_loader.h"
#include "unused.h"
#include "callback_list.tpp"
#include "fs_utils.h"
#include "string_utils.h"
#include "math_utils.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iterator>
#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <sstream>

namespace mavsdk {

template class CallbackList<Camera::Mode>;
template class CallbackList<std::vector<Camera::Setting>>;
template class CallbackList<std::vector<Camera::SettingOptions>>;
template class CallbackList<Camera::CaptureInfo>;
template class CallbackList<Camera::VideoStreamInfo>;
template class CallbackList<Camera::Status>;

CameraImpl::CameraImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

CameraImpl::CameraImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

CameraImpl::~CameraImpl()
{
    _system_impl->unregister_plugin(this);
}

void CameraImpl::init()
{
    const auto cache_dir_option = get_cache_directory();
    if (cache_dir_option) {
        _file_cache.emplace(cache_dir_option.value() / "camera", 50, true);
    } else {
        LogErr() << "Failed to get cache directory";
    }

    const auto tmp_option = create_tmp_directory("mavsdk-component-metadata");
    if (tmp_option) {
        _tmp_download_path = tmp_option.value();
    } else {
        _tmp_download_path = "./mavsdk-component-metadata";
        std::error_code err;
        std::filesystem::create_directory(_tmp_download_path, err);
    }

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT,
        [this](const mavlink_message_t& message) { process_heartbeat(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_CAPTURE_STATUS,
        [this](const mavlink_message_t& message) { process_camera_capture_status(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_STORAGE_INFORMATION,
        [this](const mavlink_message_t& message) { process_storage_information(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED,
        [this](const mavlink_message_t& message) { process_camera_image_captured(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_SETTINGS,
        [this](const mavlink_message_t& message) { process_camera_settings(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_CAMERA_INFORMATION,
        [this](const mavlink_message_t& message) { process_camera_information(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_VIDEO_STREAM_INFORMATION,
        [this](const mavlink_message_t& message) { process_video_information(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_VIDEO_STREAM_STATUS,
        [this](const mavlink_message_t& message) { process_video_stream_status(message); },
        this);

    //_request_missing_capture_info_cookie =
    //    _system_impl->add_call_every([this]() { request_missing_capture_info(); }, 0.5);
}

void CameraImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers(this);

    //_system_impl->remove_call_every(_request_missing_capture_info_cookie);
    //_system_impl->remove_call_every(_status.call_every_cookie);
    //_system_impl->remove_call_every(_camera_information_call_every_cookie);
    //_system_impl->remove_call_every(_mode.call_every_cookie);
    //_system_impl->remove_call_every(_video_stream_info.call_every_cookie);
    //_system_impl->cancel_all_param(this);

    //{
    //    std::lock_guard<std::mutex> lock(_status.mutex);
    //    _status.subscription_callbacks.clear();
    //}

    //{
    //    std::lock_guard<std::mutex> lock(_mode.mutex);
    //    _mode.subscription_callbacks.clear();
    //}

    //{
    //    std::lock_guard<std::mutex> lock(_capture_info.mutex);
    //    _capture_info.callbacks.clear();
    //}

    //{
    //    std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
    //    _video_stream_info.subscription_callbacks.clear();
    //}

    //{
    //    std::lock_guard<std::mutex> lock(_camera_list.mutex);
    //    _camera_list.subscription_callbacks.clear();
    //}

    //{
    //    std::lock_guard<std::mutex> lock(_subscribe_current_settings.mutex);
    //    _subscribe_current_settings.callbacks.clear();
    //}

    //{
    //    std::lock_guard<std::mutex> lock(_subscribe_possible_setting_options.mutex);
    //    _subscribe_possible_setting_options.callbacks.clear();
    //}

    //_camera_found = false;
}

void CameraImpl::enable() {}
void CameraImpl::disable() {}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_take_photo(int32_t camera_id, float interval_s, float no_of_photos)
{
    MavlinkCommandSender::CommandLong cmd{};

    cmd.command = MAV_CMD_IMAGE_START_CAPTURE;
    cmd.params.maybe_param1 = 0.0f; // Reserved, set to 0
    cmd.params.maybe_param2 = interval_s;
    cmd.params.maybe_param3 = no_of_photos;
    cmd.params.maybe_param4 = static_cast<float>(_capture.sequence++);
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_zoom_out(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_ZOOM;
    cmd.params.maybe_param1 = (float)ZOOM_TYPE_CONTINUOUS;
    cmd.params.maybe_param2 = -1.f;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_zoom_in(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_ZOOM;
    cmd.params.maybe_param1 = (float)ZOOM_TYPE_CONTINUOUS;
    cmd.params.maybe_param2 = 1.f;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_zoom_stop(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_ZOOM;
    cmd.params.maybe_param1 = (float)ZOOM_TYPE_CONTINUOUS;
    cmd.params.maybe_param2 = 0.f;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_zoom_range(int32_t camera_id, float range)
{
    // Clip to safe range.
    range = std::max(0.f, std::min(range, 100.f));

    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_ZOOM;
    cmd.params.maybe_param1 = (float)ZOOM_TYPE_RANGE;
    cmd.params.maybe_param2 = range;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_track_point(int32_t camera_id, float point_x, float point_y, float radius)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_CAMERA_TRACK_POINT;
    cmd.params.maybe_param1 = (float)point_x;
    cmd.params.maybe_param2 = (float)point_y;
    cmd.params.maybe_param3 = (float)radius;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_track_rectangle(
    int32_t camera_id,
    float top_left_x,
    float top_left_y,
    float bottom_right_x,
    float bottom_right_y)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_CAMERA_TRACK_RECTANGLE;
    cmd.params.maybe_param1 = top_left_x;
    cmd.params.maybe_param2 = top_left_y;
    cmd.params.maybe_param3 = bottom_right_x;
    cmd.params.maybe_param4 = bottom_right_y;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}
MavlinkCommandSender::CommandLong CameraImpl::make_command_track_stop(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_CAMERA_STOP_TRACKING;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}
MavlinkCommandSender::CommandLong CameraImpl::make_command_focus_in(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_FOCUS;
    cmd.params.maybe_param1 = (float)FOCUS_TYPE_CONTINUOUS;
    cmd.params.maybe_param2 = -1.f;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}
MavlinkCommandSender::CommandLong CameraImpl::make_command_focus_out(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_FOCUS;
    cmd.params.maybe_param1 = (float)FOCUS_TYPE_CONTINUOUS;
    cmd.params.maybe_param2 = 1.f;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}
MavlinkCommandSender::CommandLong CameraImpl::make_command_focus_stop(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_FOCUS;
    cmd.params.maybe_param1 = (float)FOCUS_TYPE_CONTINUOUS;
    cmd.params.maybe_param2 = 0.f;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}
MavlinkCommandSender::CommandLong
CameraImpl::make_command_focus_range(int32_t camera_id, float range)
{
    // Clip to safe range.
    range = std::max(0.f, std::min(range, 100.f));

    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_FOCUS;
    cmd.params.maybe_param1 = (float)FOCUS_TYPE_RANGE;
    cmd.params.maybe_param2 = range;
    cmd.target_component_id = component_id_for_camera_id(camera_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_stop_photo(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd_stop_photo{};

    cmd_stop_photo.command = MAV_CMD_IMAGE_STOP_CAPTURE;
    cmd_stop_photo.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_stop_photo;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_start_video(int32_t camera_id, float capture_status_rate_hz)
{
    MavlinkCommandSender::CommandLong cmd_start_video{};

    cmd_start_video.command = MAV_CMD_VIDEO_START_CAPTURE;
    cmd_start_video.params.maybe_param1 = 0.f; // Reserved, set to 0
    cmd_start_video.params.maybe_param2 = capture_status_rate_hz;
    cmd_start_video.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_start_video;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_stop_video(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd_stop_video{};

    cmd_stop_video.command = MAV_CMD_VIDEO_STOP_CAPTURE;
    cmd_stop_video.params.maybe_param1 = 0.f; // Reserved, set to 0
    cmd_stop_video.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_stop_video;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_set_camera_mode(int32_t camera_id, float mavlink_mode)
{
    MavlinkCommandSender::CommandLong cmd_set_camera_mode{};

    cmd_set_camera_mode.command = MAV_CMD_SET_CAMERA_MODE;
    cmd_set_camera_mode.params.maybe_param1 = 0.0f; // Reserved, set to 0
    cmd_set_camera_mode.params.maybe_param2 = mavlink_mode;
    cmd_set_camera_mode.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_set_camera_mode;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_request_camera_settings(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd_req_camera_settings{};

    cmd_req_camera_settings.command = MAV_CMD_REQUEST_CAMERA_SETTINGS;
    cmd_req_camera_settings.params.maybe_param1 = 1.f; // Request it
    cmd_req_camera_settings.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_req_camera_settings;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_request_camera_capture_status(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd_req_camera_cap_stat{};

    cmd_req_camera_cap_stat.command = MAV_CMD_REQUEST_CAMERA_CAPTURE_STATUS;
    cmd_req_camera_cap_stat.params.maybe_param1 = 1.0f; // Request it
    cmd_req_camera_cap_stat.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_req_camera_cap_stat;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_request_camera_image_captured(int32_t camera_id, const size_t photo_id)
{
    MavlinkCommandSender::CommandLong cmd_req_camera_image_captured{};

    cmd_req_camera_image_captured.command = MAV_CMD_REQUEST_MESSAGE;
    cmd_req_camera_image_captured.params.maybe_param1 =
        static_cast<float>(MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED);
    cmd_req_camera_image_captured.params.maybe_param2 = static_cast<float>(photo_id);
    cmd_req_camera_image_captured.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_req_camera_image_captured;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_request_storage_info(int32_t camera_id)
{
    MavlinkCommandSender::CommandLong cmd_req_storage_info{};

    cmd_req_storage_info.command = MAV_CMD_REQUEST_STORAGE_INFORMATION;
    cmd_req_storage_info.params.maybe_param1 = 0.f; // Reserved, set to 0
    cmd_req_storage_info.params.maybe_param2 = 1.f; // Request it
    cmd_req_storage_info.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_req_storage_info;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_start_video_streaming(int32_t camera_id, int32_t stream_id)
{
    MavlinkCommandSender::CommandLong cmd_start_video_streaming{};

    cmd_start_video_streaming.command = MAV_CMD_VIDEO_START_STREAMING;
    cmd_start_video_streaming.params.maybe_param1 = static_cast<float>(stream_id);
    cmd_start_video_streaming.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_start_video_streaming;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_stop_video_streaming(int32_t camera_id, int32_t stream_id)
{
    MavlinkCommandSender::CommandLong cmd_stop_video_streaming{};

    cmd_stop_video_streaming.command = MAV_CMD_VIDEO_STOP_STREAMING;
    cmd_stop_video_streaming.params.maybe_param1 = static_cast<float>(stream_id);
    cmd_stop_video_streaming.target_component_id = component_id_for_camera_id(camera_id);

    return cmd_stop_video_streaming;
}

Camera::Result CameraImpl::take_photo(int32_t camera_id)
{
    // TODO: check whether we are in photo mode.

    std::lock_guard<std::mutex> lock(_capture.mutex);

    // Take 1 photo only with no interval
    auto cmd_take_photo = make_command_take_photo(camera_id, 0.f, 1.0f);

    return camera_result_from_command_result(_system_impl->send_command(cmd_take_photo));
}

Camera::Result CameraImpl::zoom_out_start(int32_t camera_id)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_zoom_out(camera_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::zoom_in_start(int32_t camera_id)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_zoom_in(camera_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::zoom_stop(int32_t camera_id)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_zoom_stop(camera_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::zoom_range(int32_t camera_id, float range)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_zoom_range(camera_id, range);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result
CameraImpl::track_point(int32_t camera_id, float point_x, float point_y, float radius)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_track_point(camera_id, point_x, point_y, radius);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::track_rectangle(
    int32_t camera_id,
    float top_left_x,
    float top_left_y,
    float bottom_right_x,
    float bottom_right_y)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_track_rectangle(
        camera_id, top_left_x, top_left_y, bottom_right_x, bottom_right_y);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::track_stop(int32_t camera_id)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_track_stop(camera_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::focus_in_start(int32_t camera_id)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_focus_in(camera_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::focus_out_start(int32_t camera_id)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_focus_out(camera_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::focus_stop(int32_t camera_id)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_focus_stop(camera_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::focus_range(int32_t camera_id, float range)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_focus_range(camera_id, range);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::start_photo_interval(int32_t camera_id, float interval_s)
{
    // TODO: check whether we are in photo mode.

    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd_take_photo_time_lapse = make_command_take_photo(camera_id, interval_s, 0.f);

    return camera_result_from_command_result(_system_impl->send_command(cmd_take_photo_time_lapse));
}

Camera::Result CameraImpl::stop_photo_interval(int32_t camera_id)
{
    auto cmd_stop_photo_interval = make_command_stop_photo(camera_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd_stop_photo_interval));
}

Camera::Result CameraImpl::start_video(int32_t camera_id)
{
    // TODO: check whether video capture is already in progress.
    // TODO: check whether we are in video mode.

    // Capture status rate is not set
    auto cmd_start_video = make_command_start_video(camera_id, 0.f);

    return camera_result_from_command_result(_system_impl->send_command(cmd_start_video));
}

Camera::Result CameraImpl::stop_video(int32_t camera_id)
{
    auto cmd_stop_video = make_command_stop_video(camera_id);

    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        _video_stream_info.data.status = Camera::VideoStreamInfo::VideoStreamStatus::NotRunning;
    }

    return camera_result_from_command_result(_system_impl->send_command(cmd_stop_video));
}

void CameraImpl::zoom_in_start_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_zoom_in(camera_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::zoom_out_start_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_zoom_out(camera_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::zoom_stop_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_zoom_stop(camera_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::zoom_range_async(
    int32_t camera_id, float range, const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_zoom_range(camera_id, range);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::track_point_async(
    int32_t camera_id,
    float point_x,
    float point_y,
    float radius,
    const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_track_point(camera_id, point_x, point_y, radius);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::track_rectangle_async(
    int32_t camera_id,
    float top_left_x,
    float top_left_y,
    float bottom_right_x,
    float bottom_right_y,
    const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_track_rectangle(
        camera_id, top_left_x, top_left_y, bottom_right_x, bottom_right_y);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::track_stop_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_track_stop(camera_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::focus_in_start_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_focus_in(camera_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::focus_out_start_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_focus_out(camera_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::focus_stop_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_focus_stop(camera_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::focus_range_async(
    int32_t camera_id, float range, const Camera::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd = make_command_focus_range(camera_id, range);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::take_photo_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    // TODO: check whether we are in photo mode.

    std::lock_guard<std::mutex> lock(_capture.mutex);

    // Take 1 photo only with no interval
    auto cmd_take_photo = make_command_take_photo(camera_id, 0.f, 1.0f);

    _system_impl->send_command_async(
        cmd_take_photo, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::start_photo_interval_async(
    int32_t camera_id, float interval_s, const Camera::ResultCallback& callback)
{
    // TODO: check whether we are in photo mode.

    std::lock_guard<std::mutex> lock(_capture.mutex);

    auto cmd_take_photo_time_lapse = make_command_take_photo(camera_id, interval_s, 0.f);

    _system_impl->send_command_async(
        cmd_take_photo_time_lapse, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::stop_photo_interval_async(
    int32_t camera_id, const Camera::ResultCallback& callback)
{
    auto cmd_stop_photo_interval = make_command_stop_photo(camera_id);

    _system_impl->send_command_async(
        cmd_stop_photo_interval, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::start_video_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    // TODO: check whether video capture is already in progress.
    // TODO: check whether we are in video mode.

    // Capture status rate is not set
    auto cmd_start_video = make_command_start_video(camera_id, 0.f);

    _system_impl->send_command_async(
        cmd_start_video, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::stop_video_async(int32_t camera_id, const Camera::ResultCallback& callback)
{
    auto cmd_stop_video = make_command_stop_video(camera_id);

    _system_impl->send_command_async(
        cmd_stop_video, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

Camera::CameraList CameraImpl::camera_list() const
{
    std::lock_guard<std::mutex> lock(_camera_list.mutex);

    return _camera_list.data;
}

Camera::CameraListHandle
CameraImpl::subscribe_camera_list(const Camera::CameraListCallback& callback)
{
    std::lock_guard<std::mutex> lock(_camera_list.mutex);
    auto handle = _camera_list.subscription_callbacks.subscribe(callback);

    // TODO: what should we do?

    return handle;
}

void CameraImpl::unsubscribe_camera_list(Camera::CameraListHandle handle)
{
    std::lock_guard<std::mutex> lock(_camera_list.mutex);
    _camera_list.subscription_callbacks.unsubscribe(handle);
}

Camera::Result CameraImpl::start_video_streaming(int32_t camera_id, int32_t stream_id)
{
    std::lock_guard<std::mutex> lock(_video_stream_info.mutex);

    if (_video_stream_info.available &&
        _video_stream_info.data.status == Camera::VideoStreamInfo::VideoStreamStatus::InProgress) {
        return Camera::Result::InProgress;
    }

    // TODO Check whether we're in video mode
    auto command = make_command_start_video_streaming(camera_id, stream_id);

    auto result = camera_result_from_command_result(_system_impl->send_command(command));
    // if (result == Camera::Result::Success) {
    // Cache video stream info; app may query immediately next.
    // TODO: check if we can/should do that.
    // auto info = get_video_stream_info();
    //}
    return result;
}

Camera::Result CameraImpl::stop_video_streaming(int32_t camera_id, int32_t stream_id)
{
    // TODO I think we need to maintain current state, whether we issued
    // video capture request or video streaming request, etc.We shouldn't
    // send stop video streaming if we've not started it!
    auto command = make_command_stop_video_streaming(camera_id, stream_id);

    auto result = camera_result_from_command_result(_system_impl->send_command(command));
    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        // TODO: check if we can/should do that.
        _video_stream_info.data.status = Camera::VideoStreamInfo::VideoStreamStatus::NotRunning;
    }
    return result;
}

void CameraImpl::request_video_stream_info(int32_t camera_id)
{
    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_VIDEO_STREAM_INFORMATION, component_id_for_camera_id(camera_id), nullptr);
    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_VIDEO_STREAM_STATUS, component_id_for_camera_id(camera_id), nullptr);
}

Camera::VideoStreamInfo CameraImpl::video_stream_info()
{
    std::lock_guard<std::mutex> lock(_video_stream_info.mutex);

    return _video_stream_info.data;
}

Camera::VideoStreamInfoHandle
CameraImpl::subscribe_video_stream_info(const Camera::VideoStreamInfoCallback& callback)
{
    std::lock_guard<std::mutex> lock(_video_stream_info.mutex);

    auto handle = _video_stream_info.subscription_callbacks.subscribe(callback);

    /* TODO: request for all
    if (callback) {
        _system_impl->remove_call_every(_video_stream_info.call_every_cookie);
        _video_stream_info.call_every_cookie =
            _system_impl->add_call_every([this]() { request_video_stream_info(camera_id); }, 1.0);
    } else {
        _system_impl->remove_call_every(_video_stream_info.call_every_cookie);
    }
    */

    return handle;
}

void CameraImpl::unsubscribe_video_stream_info(Camera::VideoStreamInfoHandle handle)
{
    std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
    _video_stream_info.subscription_callbacks.unsubscribe(handle);
}

Camera::Result
CameraImpl::camera_result_from_command_result(const MavlinkCommandSender::Result command_result)
{
    switch (command_result) {
        case MavlinkCommandSender::Result::Success:
            return Camera::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::ConnectionError:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::Busy:
            return Camera::Result::Error;
        case MavlinkCommandSender::Result::Failed:
            return Camera::Result::Error;
        case MavlinkCommandSender::Result::Denied:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::TemporarilyRejected:
            return Camera::Result::Denied;
        case MavlinkCommandSender::Result::Timeout:
            return Camera::Result::Timeout;
        default:
            return Camera::Result::Unknown;
    }
}

Camera::Result CameraImpl::camera_result_from_parameter_result(
    const MavlinkParameterClient::Result parameter_result)
{
    switch (parameter_result) {
        case MavlinkParameterClient::Result::Success:
            return Camera::Result::Success;
        case MavlinkParameterClient::Result::Timeout:
            return Camera::Result::Timeout;
        case MavlinkParameterClient::Result::ConnectionError:
            return Camera::Result::Error;
        case MavlinkParameterClient::Result::WrongType:
            return Camera::Result::WrongArgument;
        case MavlinkParameterClient::Result::ParamNameTooLong:
            return Camera::Result::WrongArgument;
        case MavlinkParameterClient::Result::NotFound:
            return Camera::Result::WrongArgument;
        case MavlinkParameterClient::Result::ValueUnsupported:
            return Camera::Result::WrongArgument;
        case MavlinkParameterClient::Result::Failed:
            return Camera::Result::Error;
        case MavlinkParameterClient::Result::UnknownError:
            return Camera::Result::Error;
        default:
            return Camera::Result::Unknown;
    }
}

Camera::Result CameraImpl::set_mode(int32_t camera_id, const Camera::Mode mode)
{
    const float mavlink_mode = to_mavlink_camera_mode(mode);
    auto cmd_set_camera_mode = make_command_set_camera_mode(camera_id, mavlink_mode);
    const auto command_result = _system_impl->send_command(cmd_set_camera_mode);
    const auto camera_result = camera_result_from_command_result(command_result);

    if (camera_result == Camera::Result::Success) {
        std::lock_guard lock(_potential_cameras_mutex);
        auto maybe_potential_camera = maybe_potential_camera_for_camera_id_with_lock(camera_id);
        if (maybe_potential_camera != nullptr) {
            save_camera_mode_with_lock(*maybe_potential_camera, mode);
        }
    }

    return camera_result;
}

void CameraImpl::save_camera_mode_with_lock(PotentialCamera& potential_camera, Camera::Mode mode)
{
    potential_camera.mode = mode;

    // If there is a camera definition which is the case if we are in this
    // function, and if CAM_MODE is defined there, then we reuse that type.
    // Otherwise, we hardcode it to `uint32_t`.

    // Note that it could be that the camera definition defines options
    // different than {PHOTO, VIDEO}, in which case the mode received from
    // CAMERA_SETTINGS will be wrong.

    ParamValue value;
    if (potential_camera.camera_definition &&
        potential_camera.camera_definition->get_setting("CAM_MODE", value)) {
        if (value.is<uint8_t>()) {
            value.set<uint8_t>(static_cast<uint8_t>(mode));
        } else if (value.is<int8_t>()) {
            value.set<int8_t>(static_cast<int8_t>(mode));
        } else if (value.is<uint16_t>()) {
            value.set<uint16_t>(static_cast<uint16_t>(mode));
        } else if (value.is<int16_t>()) {
            value.set<int16_t>(static_cast<int16_t>(mode));
        } else if (value.is<uint32_t>()) {
            value.set<uint32_t>(static_cast<uint32_t>(mode));
        } else if (value.is<int32_t>()) {
            value.set<int32_t>(static_cast<int32_t>(mode));
        } else if (value.is<uint64_t>()) {
            value.set<uint64_t>(static_cast<uint64_t>(mode));
        } else if (value.is<int64_t>()) {
            value.set<int64_t>(static_cast<int64_t>(mode));
        } else if (value.is<float>()) {
            value.set<float>(static_cast<float>(mode));
        } else if (value.is<double>()) {
            value.set<double>(static_cast<double>(mode));
        }
    } else {
        value.set<uint32_t>(static_cast<uint32_t>(mode));
    }

    potential_camera.camera_definition->set_setting("CAM_MODE", value);
    refresh_params_with_lock(potential_camera);
    notify_mode();
}

float CameraImpl::to_mavlink_camera_mode(const Camera::Mode mode)
{
    switch (mode) {
        case Camera::Mode::Photo:
            return CAMERA_MODE_IMAGE;
        case Camera::Mode::Video:
            return CAMERA_MODE_VIDEO;
        default:
        case Camera::Mode::Unknown:
            return NAN;
    }
}

void CameraImpl::set_mode_async(
    int32_t camera_id, const Camera::Mode mode, const Camera::ResultCallback& callback)
{
    const auto mavlink_mode = to_mavlink_camera_mode(mode);
    auto cmd_set_camera_mode = make_command_set_camera_mode(camera_id, mavlink_mode);

    _system_impl->send_command_async(
        cmd_set_camera_mode,
        [this, callback, mode, camera_id](MavlinkCommandSender::Result result, float progress) {
            UNUSED(progress);
            receive_set_mode_command_result(result, callback, mode, camera_id);
        });
}

Camera::ModeHandle CameraImpl::subscribe_mode(const Camera::ModeCallback& callback)
{
    std::unique_lock<std::mutex> lock(_mode.mutex);
    auto handle = _mode.subscription_callbacks.subscribe(callback);
    lock.unlock();

    // TODO: implement for all

    return handle;
}

void CameraImpl::unsubscribe_mode(Camera::ModeHandle handle)
{
    std::lock_guard<std::mutex> lock(_mode.mutex);
    _mode.subscription_callbacks.unsubscribe(handle);
}

#if 0
void CameraImpl::request_status(int32_t camera_id)
{
    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_CAMERA_CAPTURE_STATUS, component_id_for_camera_id(camera_id), nullptr);
    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_STORAGE_INFORMATION, component_id_for_camera_id(camera_id), nullptr);
}
#endif

Camera::StatusHandle CameraImpl::subscribe_status(const Camera::StatusCallback& callback)
{
    std::lock_guard<std::mutex> lock(_status.mutex);
    auto handle = _status.subscription_callbacks.subscribe(callback);

    return handle;
}

void CameraImpl::unsubscribe_status(Camera::StatusHandle handle)
{
    std::lock_guard<std::mutex> lock(_status.mutex);
    _status.subscription_callbacks.unsubscribe(handle);
}

Camera::CaptureInfoHandle
CameraImpl::subscribe_capture_info(const Camera::CaptureInfoCallback& callback)
{
    std::lock_guard<std::mutex> lock(_capture_info.mutex);
    return _capture_info.callbacks.subscribe(callback);
}

void CameraImpl::unsubscribe_capture_info(Camera::CaptureInfoHandle handle)
{
    std::lock_guard<std::mutex> lock(_capture_info.mutex);
    _capture_info.callbacks.unsubscribe(handle);
}

void CameraImpl::process_heartbeat(const mavlink_message_t& message)
{
    // Check for potential camera
    std::lock_guard lock(_potential_cameras_mutex);
    auto found =
        std::any_of(_potential_cameras.begin(), _potential_cameras.end(), [&](const auto& item) {
            return item.component_id == message.compid;
        });

    if (!found) {
        auto new_potential_camera = PotentialCamera{};
        new_potential_camera.component_id = message.compid;
        _potential_cameras.emplace_back(std::move(new_potential_camera));
        check_potential_cameras_with_lock();
    }
}

void CameraImpl::check_potential_cameras_with_lock()
{
    for (auto& potential_camera : _potential_cameras) {
        // First step, get information if we don't already have it.
        if (!potential_camera.maybe_information) {
            request_camera_information(potential_camera.component_id);
            potential_camera.information_requested = true;
        }
    }
}

void CameraImpl::process_camera_capture_status(const mavlink_message_t& message)
{
    mavlink_camera_capture_status_t camera_capture_status;
    mavlink_msg_camera_capture_status_decode(&message, &camera_capture_status);

    // If image_count got smaller, consider that the storage was formatted.
    if (camera_capture_status.image_count < _status.image_count) {
        LogDebug() << "Seems like storage was formatted, setting state accordingly";
        reset_following_format_storage();
    }

    {
        std::lock_guard<std::mutex> lock(_status.mutex);

        _status.data.video_on = (camera_capture_status.video_status == 1);
        _status.data.photo_interval_on =
            (camera_capture_status.image_status == 2 || camera_capture_status.image_status == 3);
        _status.received_camera_capture_status = true;
        _status.data.recording_time_s = float(camera_capture_status.recording_time_ms) / 1e3f;

        _status.image_count = camera_capture_status.image_count;

        if (_status.image_count_at_connection == -1) {
            _status.image_count_at_connection = camera_capture_status.image_count;
        }
    }

    check_status();
}

void CameraImpl::process_storage_information(const mavlink_message_t& message)
{
    mavlink_storage_information_t storage_information;
    mavlink_msg_storage_information_decode(&message, &storage_information);

    if (storage_information.total_capacity == 0.0f) {
        // Some MAVLink systems happen to send the STORAGE_INFORMATION message
        // to indicate that the camera has a slot for a storage even if there
        // is no way to know anything about that storage (e.g. whether or not
        // there is an sdcard in the slot).
        //
        // We consider that a total capacity of 0 means that this is such a
        // message, and we don't expect MAVSDK users to leverage it, which is
        // why it is ignored.
        return;
    }

    {
        std::lock_guard<std::mutex> lock(_status.mutex);
        _status.data.storage_status = storage_status_from_mavlink(storage_information.status);
        _status.data.available_storage_mib = storage_information.available_capacity;
        _status.data.used_storage_mib = storage_information.used_capacity;
        _status.data.total_storage_mib = storage_information.total_capacity;
        _status.data.storage_id = storage_information.storage_id;
        _status.data.storage_type = storage_type_from_mavlink(storage_information.type);
        _status.received_storage_information = true;
    }

    check_status();
}

Camera::Status::StorageStatus CameraImpl::storage_status_from_mavlink(const int storage_status)
{
    switch (storage_status) {
        case STORAGE_STATUS_EMPTY:
            return Camera::Status::StorageStatus::NotAvailable;
        case STORAGE_STATUS_UNFORMATTED:
            return Camera::Status::StorageStatus::Unformatted;
        case STORAGE_STATUS_READY:
            return Camera::Status::StorageStatus::Formatted;
            break;
        case STORAGE_STATUS_NOT_SUPPORTED:
            return Camera::Status::StorageStatus::NotSupported;
        default:
            LogErr() << "Unknown storage status received.";
            return Camera::Status::StorageStatus::NotSupported;
    }
}

Camera::Status::StorageType CameraImpl::storage_type_from_mavlink(const int storage_type)
{
    switch (storage_type) {
        default:
            LogErr() << "Unknown storage_type enum value: " << storage_type;
        // FALLTHROUGH
        case STORAGE_TYPE_UNKNOWN:
            return mavsdk::Camera::Status::StorageType::Unknown;
        case STORAGE_TYPE_USB_STICK:
            return mavsdk::Camera::Status::StorageType::UsbStick;
        case STORAGE_TYPE_SD:
            return mavsdk::Camera::Status::StorageType::Sd;
        case STORAGE_TYPE_MICROSD:
            return mavsdk::Camera::Status::StorageType::Microsd;
        case STORAGE_TYPE_HD:
            return mavsdk::Camera::Status::StorageType::Hd;
        case STORAGE_TYPE_OTHER:
            return mavsdk::Camera::Status::StorageType::Other;
    }
}

void CameraImpl::process_camera_image_captured(const mavlink_message_t& message)
{
    mavlink_camera_image_captured_t image_captured;
    mavlink_msg_camera_image_captured_decode(&message, &image_captured);

    {
        mavsdk::Quaternion quaternion{};
        quaternion.w = image_captured.q[0];
        quaternion.x = image_captured.q[1];
        quaternion.y = image_captured.q[2];
        quaternion.z = image_captured.q[3];
        auto euler = to_euler_angle_from_quaternion(quaternion);

        Camera::CaptureInfo capture_info = {};
        capture_info.position.latitude_deg = image_captured.lat / 1e7;
        capture_info.position.longitude_deg = image_captured.lon / 1e7;
        capture_info.position.absolute_altitude_m = static_cast<float>(image_captured.alt) / 1e3f;
        capture_info.position.relative_altitude_m =
            static_cast<float>(image_captured.relative_alt) / 1e3f;
        capture_info.time_utc_us = image_captured.time_utc;
        capture_info.attitude_quaternion.w = image_captured.q[0];
        capture_info.attitude_quaternion.x = image_captured.q[1];
        capture_info.attitude_quaternion.y = image_captured.q[2];
        capture_info.attitude_quaternion.z = image_captured.q[3];
        capture_info.attitude_euler_angle.roll_deg = euler.roll_deg;
        capture_info.attitude_euler_angle.pitch_deg = euler.pitch_deg;
        capture_info.attitude_euler_angle.yaw_deg = euler.yaw_deg;
        capture_info.file_url = std::string(image_captured.file_url);
        capture_info.is_success = (image_captured.capture_result == 1);
        capture_info.index = image_captured.image_index;

        _status.photo_list.insert(std::make_pair(image_captured.image_index, capture_info));

        _captured_request_cv.notify_all();

        std::lock_guard<std::mutex> lock(_capture_info.mutex);
        // Notify user if a new image has been captured.
        if (_capture_info.last_advertised_image_index < capture_info.index) {
            _capture_info.callbacks.queue(
                capture_info, [this](const auto& func) { _system_impl->call_user_callback(func); });

            if (_capture_info.last_advertised_image_index != -1) {
                // Save captured indices that have been dropped to request later, however, don't
                // do it from the very beginning as there might be many photos from a previous
                // time that we don't want to request.
                for (int i = _capture_info.last_advertised_image_index + 1; i < capture_info.index;
                     ++i) {
                    if (_capture_info.missing_image_retries.find(i) ==
                        _capture_info.missing_image_retries.end()) {
                        _capture_info.missing_image_retries[i] = 0;
                    }
                }
            }

            _capture_info.last_advertised_image_index = capture_info.index;
        }

        else if (auto it = _capture_info.missing_image_retries.find(capture_info.index);
                 it != _capture_info.missing_image_retries.end()) {
            _capture_info.callbacks.queue(
                capture_info, [this](const auto& func) { _system_impl->call_user_callback(func); });
            _capture_info.missing_image_retries.erase(it);
        }
    }
}

void CameraImpl::request_missing_capture_info()
{
    std::lock_guard<std::mutex> lock(_capture_info.mutex);

    for (auto it = _capture_info.missing_image_retries.begin();
         it != _capture_info.missing_image_retries.end();
         /* ++it */) {
        if (it->second > 3) {
            it = _capture_info.missing_image_retries.erase(it);
        } else {
            ++it;
        }
    }

    /* TODO: implement again
    if (!_capture_info.missing_image_retries.empty()) {
        auto it_lowest_retries = std::min_element(
            _capture_info.missing_image_retries.begin(), _capture_info.missing_image_retries.end());
        _system_impl->mavlink_request_message().request(
            MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED,
            _camera_id + MAV_COMP_ID_CAMERA,
            nullptr,
            it_lowest_retries->first);
        it_lowest_retries->second += 1;
    }
    */
}

void CameraImpl::process_camera_settings(const mavlink_message_t& message)
{
    mavlink_camera_settings_t camera_settings;
    mavlink_msg_camera_settings_decode(&message, &camera_settings);

    std::lock_guard lock(_potential_cameras_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_camera_id_with_lock(message.compid);

    if (maybe_potential_camera == nullptr) {
        return;
    }

    save_camera_mode_with_lock(*maybe_potential_camera, to_camera_mode(camera_settings.mode_id));
}

Camera::Mode CameraImpl::to_camera_mode(const uint8_t mavlink_camera_mode)
{
    switch (mavlink_camera_mode) {
        case CAMERA_MODE_IMAGE:
            return Camera::Mode::Photo;
        case CAMERA_MODE_VIDEO:
            return Camera::Mode::Video;
        default:
            return Camera::Mode::Unknown;
    }
}

void CameraImpl::process_camera_information(const mavlink_message_t& message)
{
    mavlink_camera_information_t camera_information;
    mavlink_msg_camera_information_decode(&message, &camera_information);

    // Make sure all strings are zero terminated, so we don't overrun anywhere.
    camera_information.vendor_name[sizeof(camera_information.vendor_name) - 1] = '\0';
    camera_information.model_name[sizeof(camera_information.model_name) - 1] = '\0';
    camera_information.cam_definition_uri[sizeof(camera_information.cam_definition_uri) - 1] = '\0';

    Camera::Information new_information{};
    new_information.vendor_name = reinterpret_cast<char*>(camera_information.vendor_name);
    new_information.model_name = reinterpret_cast<char*>(camera_information.model_name);
    new_information.focal_length_mm = camera_information.focal_length;
    new_information.horizontal_sensor_size_mm = camera_information.sensor_size_h;
    new_information.vertical_sensor_size_mm = camera_information.sensor_size_v;
    new_information.horizontal_resolution_px = camera_information.resolution_h;
    new_information.vertical_resolution_px = camera_information.resolution_v;

    std::lock_guard lock(_potential_cameras_mutex);

    auto potential_camera =
        std::find_if(_potential_cameras.begin(), _potential_cameras.end(), [&](auto& item) {
            return item.component_id == message.compid;
        });

    if (potential_camera == _potential_cameras.end()) {
        auto new_potential_camera = PotentialCamera{};
        new_potential_camera.component_id = message.compid;
        _potential_cameras.emplace_back(std::move(new_potential_camera));
        potential_camera = std::prev(_potential_cameras.end());
    }

    potential_camera->maybe_information = new_information;
    potential_camera->camera_definition_url = camera_information.cam_definition_uri;
    potential_camera->camera_definition_version = camera_information.cam_definition_version;
    potential_camera->is_valid = true;
    check_camera_definition_with_lock(*potential_camera);
}

void CameraImpl::check_camera_definition_with_lock(PotentialCamera& potential_camera)
{
    const std::string url = potential_camera.camera_definition_url;

    if (potential_camera.camera_definition_url.empty()) {
        return;
    }

    const auto& info = potential_camera.maybe_information.value();
    auto file_cache_tag = replace_non_ascii_and_whitespace(
        std::string("camera_definition-") + info.model_name + "_" + info.vendor_name + "-" +
        std::to_string(potential_camera.camera_definition_version) + ".xml");

    std::optional<std::filesystem::path> cached_file_option{};
    if (_file_cache) {
        cached_file_option = _file_cache->access(file_cache_tag);
    }

    if (cached_file_option) {
        LogDebug() << "Using cached file " << cached_file_option.value();
        load_camera_definition_with_lock(potential_camera, cached_file_option.value());

    } else {
        potential_camera.is_fetching_camera_definition = true;
        auto download_path = _tmp_download_path / file_cache_tag;

        if (starts_with(url, "http://") || starts_with(url, "https://")) {
#if BUILD_WITHOUT_CURL == 1
            potential_camera.camera_definition_result = Camera::Result::ProtocolUnsupported;
#else
            HttpLoader http_loader;
            LogInfo() << "Downloading camera definition from: " << url;

            auto camera_id = camera_id_for_potential_camera_with_lock(potential_camera);

            http_loader.download_async(
                url,
                download_path,
                [download_path, file_cache_tag, camera_id, this](
                    int progress, HttpStatus status, CURLcode curl_code) mutable {
                    // TODO: check if we still exist
                    LogDebug() << "Download progress: " << progress << ", status: " << (int)status
                               << ", curl_code: " << (int)curl_code;

                    std::lock_guard lock(_potential_cameras_mutex);
                    auto maybe_potential_camera =
                        maybe_potential_camera_for_camera_id_with_lock(camera_id);
                    if (maybe_potential_camera != nullptr) {
                        LogErr() << "Failed to find camera.";
                    }

                    if (status == HttpStatus::Error) {
                        LogErr() << "File download failed with result " << curl_code;
                        maybe_potential_camera->is_fetching_camera_definition = false;
                        maybe_potential_camera->camera_definition_result = Camera::Result::Error;

                    } else if (status == HttpStatus::Finished) {
                        LogDebug() << "File download finished " << download_path;
                        if (_file_cache) {
                            // Cache the file (this will move/remove the temp file as well)
                            download_path = _file_cache->insert(file_cache_tag, download_path)
                                                .value_or(download_path);
                            LogDebug() << "Cached path: " << download_path;
                        }
                        load_camera_definition_with_lock(*maybe_potential_camera, download_path);
                        maybe_potential_camera->is_fetching_camera_definition = false;
                        // TODO: add parsing result
                        maybe_potential_camera->camera_definition_result = Camera::Result::Success;
                    }
                });
#endif
        } else if (starts_with(url, "mftp://") || starts_with(url, "mavlinkftp://")) {
            LogInfo() << "Download file: " << url << " using MAVLink FTP...";

            auto camera_id = camera_id_for_potential_camera_with_lock(potential_camera);

            _system_impl->mavlink_ftp_client().download_async(
                strip_prefix(strip_prefix(url, "mavlinkftp://"), "mftp://"),
                download_path,
                false,
                [file_cache_tag, download_path, camera_id, this](
                    MavlinkFtpClient::ClientResult client_result,
                    MavlinkFtpClient::ProgressData progress_data) mutable {
                    // TODO: check if we still exist
                    LogDebug() << "Mavlink FTP download progress: "
                               << 100 * progress_data.bytes_transferred / progress_data.total_bytes
                               << " %";
                    if (client_result == MavlinkFtpClient::ClientResult::Next) {
                        return;
                    }

                    std::lock_guard lock(_potential_cameras_mutex);
                    auto maybe_potential_camera =
                        maybe_potential_camera_for_camera_id_with_lock(camera_id);
                    if (maybe_potential_camera != nullptr) {
                        LogErr() << "Failed to find camera.";
                        return;
                    }

                    if (client_result != MavlinkFtpClient::ClientResult::Success) {
                        LogErr() << "File download failed with result " << client_result;
                        maybe_potential_camera->is_fetching_camera_definition = false;
                        maybe_potential_camera->camera_definition_result = Camera::Result::Error;
                        return;
                    }

                    LogDebug() << "File download finished to " << download_path;
                    if (_file_cache) {
                        // Cache the file (this will move/remove the temp file as well)
                        download_path = _file_cache->insert(file_cache_tag, download_path)
                                            .value_or(download_path);
                        LogDebug() << "Cached path: " << download_path;
                    }
                    load_camera_definition_with_lock(*maybe_potential_camera, download_path);
                    // TODO: add parsing result
                    maybe_potential_camera->is_fetching_camera_definition = false;
                    maybe_potential_camera->camera_definition_result = Camera::Result::Success;
                });
        } else {
            LogErr() << "Unknown protocol for URL: " << url;
            potential_camera.camera_definition_result = Camera::Result::ProtocolUnsupported;
        }
    }
}

void CameraImpl::load_camera_definition_with_lock(
    PotentialCamera& potential_camera, const std::filesystem::path& path)
{
    if (potential_camera.camera_definition == nullptr) {
        potential_camera.camera_definition = std::make_unique<CameraDefinition>();
    }

    potential_camera.camera_definition->load_file(path.string());
}

void CameraImpl::process_video_information(const mavlink_message_t& message)
{
    mavlink_video_stream_information_t received_video_info;
    mavlink_msg_video_stream_information_decode(&message, &received_video_info);

    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        // TODO: use stream_id and count
        _video_stream_info.data.status =
            (received_video_info.flags & VIDEO_STREAM_STATUS_FLAGS_RUNNING ?
                 Camera::VideoStreamInfo::VideoStreamStatus::InProgress :
                 Camera::VideoStreamInfo::VideoStreamStatus::NotRunning);
        _video_stream_info.data.spectrum =
            (received_video_info.flags & VIDEO_STREAM_STATUS_FLAGS_THERMAL ?
                 Camera::VideoStreamInfo::VideoStreamSpectrum::Infrared :
                 Camera::VideoStreamInfo::VideoStreamSpectrum::VisibleLight);

        auto& video_stream_info = _video_stream_info.data.settings;
        video_stream_info.frame_rate_hz = received_video_info.framerate;
        video_stream_info.horizontal_resolution_pix = received_video_info.resolution_h;
        video_stream_info.vertical_resolution_pix = received_video_info.resolution_v;
        video_stream_info.bit_rate_b_s = received_video_info.bitrate;
        video_stream_info.rotation_deg = received_video_info.rotation;
        video_stream_info.horizontal_fov_deg = static_cast<float>(received_video_info.hfov);
        video_stream_info.uri = received_video_info.uri;
        _video_stream_info.available = true;
    }

    notify_video_stream_info();
}

void CameraImpl::process_video_stream_status(const mavlink_message_t& message)
{
    mavlink_video_stream_status_t received_video_stream_status;
    mavlink_msg_video_stream_status_decode(&message, &received_video_stream_status);
    {
        std::lock_guard<std::mutex> lock(_video_stream_info.mutex);
        _video_stream_info.data.status =
            (received_video_stream_status.flags & VIDEO_STREAM_STATUS_FLAGS_RUNNING ?
                 Camera::VideoStreamInfo::VideoStreamStatus::InProgress :
                 Camera::VideoStreamInfo::VideoStreamStatus::NotRunning);
        _video_stream_info.data.spectrum =
            (received_video_stream_status.flags & VIDEO_STREAM_STATUS_FLAGS_THERMAL ?
                 Camera::VideoStreamInfo::VideoStreamSpectrum::Infrared :
                 Camera::VideoStreamInfo::VideoStreamSpectrum::VisibleLight);

        auto& video_stream_info = _video_stream_info.data.settings;
        video_stream_info.frame_rate_hz = received_video_stream_status.framerate;
        video_stream_info.horizontal_resolution_pix = received_video_stream_status.resolution_h;
        video_stream_info.vertical_resolution_pix = received_video_stream_status.resolution_v;
        video_stream_info.bit_rate_b_s = received_video_stream_status.bitrate;
        video_stream_info.rotation_deg = received_video_stream_status.rotation;
        video_stream_info.horizontal_fov_deg =
            static_cast<float>(received_video_stream_status.hfov);
        _video_stream_info.available = true;
    }

    notify_video_stream_info();
}

void CameraImpl::notify_video_stream_info()
{
    std::lock_guard<std::mutex> lock(_video_stream_info.mutex);

    _video_stream_info.subscription_callbacks.queue(
        _video_stream_info.data,
        [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void CameraImpl::check_status()
{
    std::lock_guard<std::mutex> lock(_status.mutex);

    if (_status.received_camera_capture_status && _status.received_storage_information) {
        _status.subscription_callbacks.queue(
            _status.data, [this](const auto& func) { _system_impl->call_user_callback(func); });

        _status.received_camera_capture_status = false;
        _status.received_storage_information = false;
    }
}

void CameraImpl::receive_command_result(
    MavlinkCommandSender::Result command_result, const Camera::ResultCallback& callback)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    if (callback) {
        _system_impl->call_user_callback([callback, camera_result]() { callback(camera_result); });
    }
}

void CameraImpl::receive_set_mode_command_result(
    const MavlinkCommandSender::Result command_result,
    const Camera::ResultCallback callback,
    const Camera::Mode mode,
    int32_t camera_id)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    if (callback) {
        _system_impl->call_user_callback([callback, camera_result]() { callback(camera_result); });
    }

    if (camera_result == Camera::Result::Success) {
        std::lock_guard lock(_potential_cameras_mutex);
        auto maybe_potential_camera = maybe_potential_camera_for_camera_id_with_lock(camera_id);
        if (maybe_potential_camera != nullptr) {
            save_camera_mode_with_lock(*maybe_potential_camera, mode);
        }
    }
}

void CameraImpl::notify_mode()
{
    // TODO implement
    // std::lock_guard<std::mutex> lock(_mode.mutex);

    //_mode.subscription_callbacks.queue(
    //    _mode.data, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

bool CameraImpl::get_possible_options_with_lock(
    PotentialCamera& camera, const std::string& setting_id, std::vector<Camera::Option>& options)
{
    options.clear();

    if (!camera.camera_definition) {
        LogWarn() << "Error: no camera definition available yet";
        return false;
    }

    std::vector<ParamValue> values;
    if (!camera.camera_definition->get_possible_options(setting_id, values)) {
        return false;
    }

    for (const auto& value : values) {
        std::stringstream ss{};
        ss << value;
        Camera::Option option{};
        option.option_id = ss.str();
        if (!camera.camera_definition->is_setting_range(setting_id)) {
            get_option_str_with_lock(
                camera, setting_id, option.option_id, option.option_description);
        }
        options.push_back(option);
    }

    return options.size() > 0;
}

Camera::Result CameraImpl::set_setting(int32_t camera_id, Camera::Setting setting)

{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    set_setting_async(
        camera_id, setting, [&prom](Camera::Result result) { prom->set_value(result); });

    return ret.get();
}

void CameraImpl::set_setting_async(
    int32_t camera_id, Camera::Setting setting, const Camera::ResultCallback callback)
{
    set_option_async(camera_id, setting.setting_id, setting.option, callback);
}

void CameraImpl::set_option_async(
    int32_t camera_id,
    const std::string& setting_id,
    const Camera::Option& option,
    const Camera::ResultCallback& callback)
{
    std::lock_guard lock(_potential_cameras_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_camera_id_with_lock(camera_id);
    if (maybe_potential_camera == nullptr) {
        if (callback != nullptr) {
            _system_impl->call_user_callback(
                [callback]() { callback(Camera::Result::CameraIdInvalid); });
        }
        return;
    }

    auto& camera = *maybe_potential_camera;

    if (camera.camera_definition == nullptr) {
        if (callback != nullptr) {
            _system_impl->call_user_callback(
                [callback]() { callback(Camera::Result::SettingsUnavailable); });
        }
        return;
    }

    // We get it first so that we have the type of the param value.
    ParamValue value;

    if (camera.camera_definition->is_setting_range(setting_id)) {
        // TODO: Get type from minimum.
        std::vector<ParamValue> all_values;
        if (!camera.camera_definition->get_all_options(setting_id, all_values)) {
            if (callback) {
                LogErr() << "Could not get all options to get type for range param.";
                _system_impl->call_user_callback([callback]() { callback(Camera::Result::Error); });
            }
            return;
        }

        if (all_values.size() == 0) {
            if (callback) {
                LogErr() << "Could not get any options to get type for range param.";
                _system_impl->call_user_callback([callback]() { callback(Camera::Result::Error); });
            }
            return;
        }
        value = all_values[0];
        // Now re-use that type.
        // FIXME: this is quite ugly, we should do better than that.
        if (!value.set_as_same_type(option.option_id)) {
            if (callback) {
                LogErr() << "Could not set option value to given type.";
                _system_impl->call_user_callback([callback]() { callback(Camera::Result::Error); });
            }
            return;
        }

    } else {
        if (!camera.camera_definition->get_option_value(setting_id, option.option_id, value)) {
            if (callback) {
                LogErr() << "Could not get option value.";
                _system_impl->call_user_callback([callback]() { callback(Camera::Result::Error); });
            }
            return;
        }

        std::vector<ParamValue> possible_values;
        camera.camera_definition->get_possible_options(setting_id, possible_values);
        bool allowed = false;
        for (const auto& possible_value : possible_values) {
            if (value == possible_value) {
                allowed = true;
            }
        }
        if (!allowed) {
            LogErr() << "Setting " << setting_id << "(" << option.option_id << ") not allowed";
            if (callback) {
                _system_impl->call_user_callback([callback]() { callback(Camera::Result::Error); });
            }
            return;
        }
    }

    _system_impl->set_param_async(
        setting_id,
        value,
        [this, camera_id, callback, setting_id, value](MavlinkParameterClient::Result result) {
            std::lock_guard lock_later(_potential_cameras_mutex);
            auto maybe_potential_camera_later =
                maybe_potential_camera_for_camera_id_with_lock(camera_id);
            // We already checked these fields earlier, so we don't check again.
            assert(maybe_potential_camera_later != nullptr);
            assert(maybe_potential_camera_later->camera_definition != nullptr);

            auto& camera_later = *maybe_potential_camera_later;

            if (result != MavlinkParameterClient::Result::Success) {
                if (callback) {
                    _system_impl->call_user_callback([callback, result]() {
                        callback(camera_result_from_parameter_result(result));
                    });
                }
                return;
            }

            if (!camera_later.camera_definition->set_setting(setting_id, value)) {
                if (callback) {
                    _system_impl->call_user_callback(
                        [callback]() { callback(Camera::Result::Error); });
                }
                return;
            }

            if (callback) {
                _system_impl->call_user_callback(
                    [callback]() { callback(Camera::Result::Success); });
            }

            refresh_params_with_lock(camera_later);
        },
        this,
        static_cast<uint8_t>(_camera_id + MAV_COMP_ID_CAMERA),
        true);
}

void CameraImpl::get_setting_async(
    int32_t camera_id, Camera::Setting setting, const Camera::GetSettingCallback callback)
{
    std::lock_guard lock(_potential_cameras_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_camera_id_with_lock(camera_id);
    if (maybe_potential_camera == nullptr) {
        if (callback != nullptr) {
            _system_impl->call_user_callback(
                [callback]() { callback(Camera::Result::CameraIdInvalid, {}); });
        }
        return;
    }

    auto& camera = *maybe_potential_camera;

    if (camera.camera_definition == nullptr) {
        if (callback != nullptr) {
            _system_impl->call_user_callback(
                [callback]() { callback(Camera::Result::SettingsUnavailable, {}); });
        }
        return;
    }

    get_option_async(
        camera_id,
        setting.setting_id,
        [this, setting, callback](Camera::Result result, const Camera::Option& option) {
            Camera::Setting new_setting{};
            new_setting.option = option;
            if (callback) {
                _system_impl->call_user_callback(
                    [callback, result, new_setting]() { callback(result, new_setting); });
            }
        });
}

std::pair<Camera::Result, Camera::Setting>
CameraImpl::get_setting(int32_t camera_id, Camera::Setting setting)
{
    auto prom = std::make_shared<std::promise<std::pair<Camera::Result, Camera::Setting>>>();
    auto ret = prom->get_future();

    get_setting_async(
        camera_id, setting, [&prom](Camera::Result result, const Camera::Setting& new_setting) {
            prom->set_value(std::make_pair<>(result, new_setting));
        });

    return ret.get();
}

Camera::Result
CameraImpl::get_option(int32_t camera_id, const std::string& setting_id, Camera::Option& option)
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    get_option_async(
        camera_id,
        setting_id,
        [prom, &option](Camera::Result result, const Camera::Option& option_gotten) {
            prom->set_value(result);
            if (result == Camera::Result::Success) {
                option = option_gotten;
            }
        });

    auto status = ret.wait_for(std::chrono::seconds(1));

    if (status == std::future_status::ready) {
        return Camera::Result::Success;
    } else {
        return Camera::Result::Timeout;
    }
}

void CameraImpl::get_option_async(
    int32_t camera_id,
    const std::string& setting_id,
    const std::function<void(Camera::Result, const Camera::Option&)>& callback)
{
    std::lock_guard lock(_potential_cameras_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_camera_id_with_lock(camera_id);
    if (maybe_potential_camera == nullptr) {
        if (callback != nullptr) {
            _system_impl->call_user_callback(
                [callback]() { callback(Camera::Result::CameraIdInvalid, {}); });
        }
        return;
    }

    auto& camera = *maybe_potential_camera;

    if (camera.camera_definition == nullptr) {
        if (callback != nullptr) {
            _system_impl->call_user_callback(
                [callback]() { callback(Camera::Result::SettingsUnavailable, {}); });
        }
        return;
    }

    ParamValue value;
    // We should have this cached and don't need to get the param.
    if (camera.camera_definition->get_setting(setting_id, value)) {
        if (callback) {
            Camera::Option new_option{};
            new_option.option_id = value.get_string();
            if (!camera.camera_definition->is_setting_range(setting_id)) {
                get_option_str_with_lock(
                    camera, setting_id, new_option.option_id, new_option.option_description);
            }
            const auto temp_callback = callback;
            _system_impl->call_user_callback([temp_callback, new_option]() {
                temp_callback(Camera::Result::Success, new_option);
            });
        }
    } else {
        // If this still happens, we request the param, but also complain.
        LogWarn() << "Setting '" << setting_id << "' not found.";
        if (callback) {
            const auto temp_callback = callback;
            _system_impl->call_user_callback(
                [temp_callback]() { temp_callback(Camera::Result::Error, {}); });
        }
    }
}

Camera::CurrentSettingsHandle
CameraImpl::subscribe_current_settings(const Camera::CurrentSettingsCallback& callback)
{
    std::unique_lock<std::mutex> lock(_subscribe_current_settings.mutex);
    auto handle = _subscribe_current_settings.callbacks.subscribe(callback);
    lock.unlock();

    notify_current_settings_for_all();
    return handle;
}

void CameraImpl::unsubscribe_current_settings(Camera::CurrentSettingsHandle handle)
{
    std::lock_guard<std::mutex> lock(_subscribe_current_settings.mutex);
    _subscribe_current_settings.callbacks.unsubscribe(handle);
}

Camera::PossibleSettingOptionsHandle CameraImpl::subscribe_possible_setting_options(
    const Camera::PossibleSettingOptionsCallback& callback)
{
    std::unique_lock<std::mutex> lock(_subscribe_possible_setting_options.mutex);
    auto handle = _subscribe_possible_setting_options.callbacks.subscribe(callback);
    lock.unlock();

    notify_possible_setting_options_for_all();
    return handle;
}

void CameraImpl::unsubscribe_possible_setting_options(Camera::PossibleSettingOptionsHandle handle)
{
    std::lock_guard<std::mutex> lock(_subscribe_possible_setting_options.mutex);
    _subscribe_possible_setting_options.callbacks.unsubscribe(handle);
}

void CameraImpl::notify_current_settings_for_all()
{
    std::lock_guard lock(_potential_cameras_mutex);

    for (auto& potential_camera : _potential_cameras) {
        if (potential_camera.camera_definition != nullptr) {
            notify_current_settings_with_lock(potential_camera);
        }
    }
}

void CameraImpl::notify_possible_setting_options_for_all()
{
    std::lock_guard lock(_potential_cameras_mutex);

    for (auto& potential_camera : _potential_cameras) {
        if (potential_camera.camera_definition != nullptr) {
            notify_possible_setting_options_with_lock(potential_camera);
        }
    }
}

void CameraImpl::notify_current_settings_with_lock(PotentialCamera& potential_camera)
{
    assert(potential_camera.camera_definition != nullptr);

    if (_subscribe_current_settings.callbacks.empty()) {
        return;
    }

    auto possible_setting_options = get_possible_setting_options_with_lock(potential_camera);
    if (possible_setting_options.first != Camera::Result::Success) {
        LogErr() << "Could not get possible settings in current options subscription.";
        return;
    }

    auto& camera = potential_camera;

    std::vector<Camera::Setting> current_settings{};

    for (auto& possible_setting : possible_setting_options.second) {
        // use the cache for this, presumably we updated it right before.
        ParamValue value;
        if (camera.camera_definition->get_setting(possible_setting.setting_id, value)) {
            Camera::Setting setting{};
            setting.setting_id = possible_setting.setting_id;
            setting.is_range =
                camera.camera_definition->is_setting_range(possible_setting.setting_id);
            get_setting_str_with_lock(camera, setting.setting_id, setting.setting_description);
            setting.option.option_id = value.get_string();
            if (!camera.camera_definition->is_setting_range(possible_setting.setting_id)) {
                get_option_str_with_lock(
                    camera,
                    setting.setting_id,
                    setting.option.option_id,
                    setting.option.option_description);
            }
            current_settings.push_back(setting);
        }
    }

    _subscribe_current_settings.callbacks.queue(
        current_settings, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void CameraImpl::notify_possible_setting_options_with_lock(PotentialCamera& potential_camera)
{
    assert(potential_camera.camera_definition != nullptr);

    if (_subscribe_possible_setting_options.callbacks.empty()) {
        return;
    }

    auto setting_options = get_possible_setting_options_with_lock(potential_camera);
    if (setting_options.second.size() == 0) {
        return;
    }

    _subscribe_possible_setting_options.callbacks.queue(
        setting_options.second,
        [this](const auto& func) { _system_impl->call_user_callback(func); });
}

std::pair<Camera::Result, std::vector<Camera::SettingOptions>>
CameraImpl::get_possible_setting_options(int32_t camera_id)
{
    std::pair<Camera::Result, std::vector<Camera::SettingOptions>> result;

    std::lock_guard lock(_potential_cameras_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_camera_id_with_lock(camera_id);
    if (maybe_potential_camera == nullptr) {
        result.first = Camera::Result::CameraIdInvalid;
        return result;
    }

    auto& camera = *maybe_potential_camera;
    if (!camera.is_fetching_camera_definition) {
        result.first = Camera::Result::SettingsLoading;
        return result;
    }

    if (camera.camera_definition == nullptr) {
        result.first = Camera::Result::SettingsUnavailable;
        return result;
    }

    return get_possible_setting_options_with_lock(*maybe_potential_camera);
}

std::pair<Camera::Result, std::vector<Camera::SettingOptions>>
CameraImpl::get_possible_setting_options_with_lock(PotentialCamera& potential_camera)
{
    std::pair<Camera::Result, std::vector<Camera::SettingOptions>> result;

    std::unordered_map<std::string, ParamValue> possible_settings;
    assert(potential_camera.camera_definition != nullptr);

    auto& camera = potential_camera;

    camera.camera_definition->get_possible_settings(possible_settings);

    for (auto& possible_setting : possible_settings) {
        Camera::SettingOptions setting_options{};
        setting_options.setting_id = possible_setting.first;
        setting_options.is_range =
            camera.camera_definition->is_setting_range(possible_setting.first);
        get_setting_str_with_lock(
            camera, setting_options.setting_id, setting_options.setting_description);
        get_possible_options_with_lock(camera, possible_setting.first, setting_options.options);
        result.second.push_back(setting_options);
    }

    return result;
}

void CameraImpl::refresh_params_with_lock(PotentialCamera& potential_camera)
{
    assert(potential_camera.camera_definition != nullptr);

    std::vector<std::pair<std::string, ParamValue>> params;
    potential_camera.camera_definition->get_unknown_params(params);
    if (params.empty()) {
        // We're assuming that we changed one option and this did not cause
        // any other possible settings to change. However, we still would
        // like to notify the current settings with this one change.
        notify_current_settings_with_lock(potential_camera);
        notify_possible_setting_options_with_lock(potential_camera);
        return;
    }

    auto camera_id = camera_id_for_potential_camera_with_lock(potential_camera);

    unsigned count = 0;
    for (const auto& param : params) {
        const std::string& param_name = param.first;
        const ParamValue& param_value_type = param.second;
        const bool is_last = (count == params.size() - 1);
        _system_impl->get_param_async(
            param_name,
            param_value_type,
            [camera_id, param_name, is_last, this](
                MavlinkParameterClient::Result result, ParamValue value) {
                if (result != MavlinkParameterClient::Result::Success) {
                    return;
                }

                std::lock_guard lock_later(_potential_cameras_mutex);
                auto maybe_potential_camera_later =
                    maybe_potential_camera_for_camera_id_with_lock(camera_id);
                // We already checked these fields earlier, so we don't check again.
                assert(maybe_potential_camera_later != nullptr);
                assert(maybe_potential_camera_later->camera_definition != nullptr);

                auto& camera_later = *maybe_potential_camera_later;

                if (camera_later.camera_definition->set_setting(param_name, value)) {
                    return;
                }

                if (is_last) {
                    notify_current_settings_with_lock(camera_later);
                    notify_possible_setting_options_with_lock(camera_later);
                }
            },
            this,
            static_cast<uint8_t>(_camera_id + MAV_COMP_ID_CAMERA),
            true);
        ++count;
    }
}

bool CameraImpl::get_setting_str_with_lock(
    PotentialCamera& potential_camera, std::string& setting_id, std::string& description)
{
    if (potential_camera.camera_definition == nullptr) {
        return false;
    }

    return potential_camera.camera_definition->get_setting_str(setting_id, description);
}

bool CameraImpl::get_option_str_with_lock(
    PotentialCamera& potential_camera,
    const std::string& setting_id,
    const std::string& option_id,
    std::string& description)
{
    if (potential_camera.camera_definition == nullptr) {
        return false;
    }

    return potential_camera.camera_definition->get_option_str(setting_id, option_id, description);
}

void CameraImpl::request_camera_settings(int32_t camera_id)
{
    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_CAMERA_SETTINGS, component_id_for_camera_id(camera_id), nullptr);
}

void CameraImpl::request_camera_information(uint8_t component_id)
{
    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_CAMERA_INFORMATION, component_id, nullptr);
}

Camera::Result CameraImpl::format_storage(int32_t camera_id, int32_t storage_id)
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    format_storage_async(
        camera_id, storage_id, [prom](Camera::Result result) { prom->set_value(result); });

    return ret.get();
}

void CameraImpl::format_storage_async(
    int32_t camera_id, int32_t storage_id, Camera::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong cmd_format{};

    cmd_format.command = MAV_CMD_STORAGE_FORMAT;
    cmd_format.params.maybe_param1 = static_cast<float>(storage_id); // storage ID
    cmd_format.params.maybe_param2 = 1.0f; // format
    cmd_format.params.maybe_param3 = 1.0f; // clear
    cmd_format.target_component_id = _camera_id + MAV_COMP_ID_CAMERA;

    _system_impl->send_command_async(
        cmd_format, [this, callback](MavlinkCommandSender::Result result, float progress) {
            UNUSED(progress);

            receive_command_result(result, [this, callback](Camera::Result camera_result) {
                if (camera_result == Camera::Result::Success) {
                    reset_following_format_storage();
                }

                callback(camera_result);
            });
        });
}

Camera::Result CameraImpl::reset_settings(int32_t camera_id)
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    reset_settings_async(camera_id, [prom](Camera::Result result) { prom->set_value(result); });

    return ret.get();
}
void CameraImpl::reset_settings_async(int32_t camera_id, const Camera::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong cmd_format{};

    cmd_format.command = MAV_CMD_RESET_CAMERA_SETTINGS;
    cmd_format.params.maybe_param1 = 1.0f; // reset
    cmd_format.target_component_id = _camera_id + MAV_COMP_ID_CAMERA;

    _system_impl->send_command_async(
        cmd_format, [this, callback](MavlinkCommandSender::Result result, float progress) {
            UNUSED(progress);

            receive_command_result(result, [this, callback](Camera::Result camera_result) {
                callback(camera_result);
            });
        });
}

void CameraImpl::reset_following_format_storage()
{
    {
        std::lock_guard<std::mutex> status_lock(_status.mutex);
        _status.photo_list.clear();
        _status.image_count = 0;
        _status.image_count_at_connection = 0;
    }
    {
        std::lock_guard<std::mutex> lock(_capture_info.mutex);
        _capture_info.last_advertised_image_index = -1;
        _capture_info.missing_image_retries.clear();
    }
}

std::pair<Camera::Result, std::vector<Camera::CaptureInfo>>
CameraImpl::list_photos(int32_t camera_id, Camera::PhotosRange photos_range)
{
    std::promise<std::pair<Camera::Result, std::vector<Camera::CaptureInfo>>> prom;
    auto ret = prom.get_future();

    list_photos_async(
        camera_id,
        photos_range,
        [&prom](Camera::Result result, std::vector<Camera::CaptureInfo> photo_list) {
            prom.set_value(std::make_pair(result, photo_list));
        });

    return ret.get();
}

void CameraImpl::list_photos_async(
    int32_t camera_id, Camera::PhotosRange photos_range, const Camera::ListPhotosCallback callback)
{
    if (!callback) {
        LogWarn() << "Trying to get a photo list with a null callback, ignoring...";
        return;
    }

    {
        std::lock_guard<std::mutex> status_lock(_status.mutex);

        if (_status.is_fetching_photos) {
            _system_impl->call_user_callback([callback]() {
                callback(Camera::Result::Busy, std::vector<Camera::CaptureInfo>{});
            });
            return;
        } else {
            _status.is_fetching_photos = true;
        }

        if (_status.image_count == -1) {
            LogErr() << "Cannot list photos: camera status has not been received yet!";
            _status.is_fetching_photos = false;
            _system_impl->call_user_callback([callback]() {
                callback(Camera::Result::Error, std::vector<Camera::CaptureInfo>{});
            });
            return;
        }
    }

    const int start_index = [this, photos_range]() {
        switch (photos_range) {
            case Camera::PhotosRange::SinceConnection:
                return _status.image_count_at_connection;
            case Camera::PhotosRange::All:
            // FALLTHROUGH
            default:
                return 0;
        }
    }();

    std::thread([this, start_index, callback]() {
        std::unique_lock<std::mutex> capture_request_lock(_captured_request_mutex);

        for (int i = start_index; i < _status.image_count; i++) {
            // In case the vehicle sends capture info, but not those we are asking, we do not
            // want to loop infinitely. The safety_count is here to abort if this happens.
            auto safety_count = 0;
            const auto safety_count_boundary = 10;

            while (_status.photo_list.find(i) == _status.photo_list.end() &&
                   safety_count < safety_count_boundary) {
                safety_count++;

                auto request_try_number = 0;
                const auto request_try_limit =
                    10; // Timeout if the request times out that many times
                auto cv_status = std::cv_status::timeout;

                while (cv_status == std::cv_status::timeout) {
                    request_try_number++;
                    if (request_try_number >= request_try_limit) {
                        std::lock_guard<std::mutex> status_lock(_status.mutex);
                        _status.is_fetching_photos = false;
                        _system_impl->call_user_callback([callback]() {
                            callback(Camera::Result::Timeout, std::vector<Camera::CaptureInfo>{});
                        });
                        return;
                    }

                    _system_impl->mavlink_request_message().request(
                        MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED,
                        _camera_id + MAV_COMP_ID_CAMERA,
                        nullptr,
                        i);
                    cv_status = _captured_request_cv.wait_for(
                        capture_request_lock, std::chrono::seconds(1));
                }
            }

            if (safety_count == safety_count_boundary) {
                std::lock_guard<std::mutex> status_lock(_status.mutex);
                _status.is_fetching_photos = false;
                _system_impl->call_user_callback([callback]() {
                    callback(Camera::Result::Error, std::vector<Camera::CaptureInfo>{});
                });
                return;
            }
        }

        std::vector<Camera::CaptureInfo> photo_list;
        {
            std::lock_guard<std::mutex> status_lock(_status.mutex);

            for (auto capture_info : _status.photo_list) {
                if (capture_info.first >= start_index) {
                    photo_list.push_back(capture_info.second);
                }
            }

            _status.is_fetching_photos = false;

            const auto temp_callback = callback;
            _system_impl->call_user_callback([temp_callback, photo_list]() {
                temp_callback(Camera::Result::Success, photo_list);
            });
        }
    }).detach();
}

Camera::Mode CameraImpl::get_mode(int32_t camera_id) {}

Camera::Status CameraImpl::get_status(int32_t camera_id) {}

Camera::VideoStreamInfo CameraImpl::get_video_stream_info(int32_t camera_id) {}

std::pair<Camera::Result, std::vector<Camera::Setting>>
CameraImpl::get_current_settings(int32_t camera_id)
{
    std::pair<Camera::Result, std::vector<Camera::Setting>> result;

    std::lock_guard lock(_potential_cameras_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_camera_id_with_lock(camera_id);
    if (maybe_potential_camera != nullptr) {
        result.first = Camera::Result::CameraIdInvalid;
        return result;
    }

    auto& camera = *maybe_potential_camera;
    if (!camera.is_fetching_camera_definition) {
        result.first = Camera::Result::SettingsLoading;
        return result;
    }

    if (camera.camera_definition == nullptr) {
        result.first = Camera::Result::SettingsUnavailable;
        return result;
    }

    std::vector<Camera::Setting> current_settings{};

    auto possible_setting_options = get_possible_setting_options_with_lock(camera);
    if (possible_setting_options.first != Camera::Result::Success) {
        result.first = possible_setting_options.first;
        return result;
    }

    for (auto& possible_setting : possible_setting_options.second) {
        // use the cache for this, presumably we updated it right before.
        ParamValue value;
        if (camera.camera_definition->get_setting(possible_setting.setting_id, value)) {
            Camera::Setting setting{};
            setting.setting_id = possible_setting.setting_id;
            setting.is_range =
                camera.camera_definition->is_setting_range(possible_setting.setting_id);
            get_setting_str_with_lock(camera, setting.setting_id, setting.setting_description);
            setting.option.option_id = value.get_string();
            if (!camera.camera_definition->is_setting_range(possible_setting.setting_id)) {
                get_option_str_with_lock(
                    camera,
                    setting.setting_id,
                    setting.option.option_id,
                    setting.option.option_description);
            }
            current_settings.push_back(setting);
        }
    }

    result.first = Camera::Result::Success;
    return result;
}

// std::pair<Camera::Result, std::vector<Camera::SettingOptions>>
// CameraImpl::get_possible_setting_options(int32_t camera_id)
//{
//     std::pair<Camera::Result, std::vector<Camera::SettingOptions>> result;
//
//     std::lock_guard lock(_potential_cameras_mutex);
//     auto maybe_potential_camera = maybe_potential_camera_for_camera_id_with_lock(camera_id);
//     if (maybe_potential_camera != nullptr) {
//         result.first = Camera::Result::CameraIdInvalid;
//         return result;
//     }
//
//     auto& camera& = *maybe_potential_camera;
//     if (!camera.is_fetching_camera_definition) {
//         result.first = Camera::Result::SettingsLoading;
//         return result;
//     }
//
//     if (camera.camera_definition == nullptr) {
//         result.first = Camera::Result::SettingsUnavailable;
//         return result;
//     }
//
//     std::unordered_map<std::string, ParamValue> cd_settings{};
//     camera.camera_definition->get_possible_settings(cd_settings);
//
//     for (const auto& cd_setting : cd_settings) {
//         if (cd_setting.first == "CAM_MODE") {
//             // We ignore the mode param.
//             continue;
//         }
//
//         result.second.push_back(cd_setting.first);
//     }
//
//     result.first = Camera::Result::Success;
//     return result;
// }

uint8_t CameraImpl::component_id_for_camera_id(int32_t camera_id)
{
    if (camera_id == 0) {
        // All cameras
        return 0;
    }

    if (camera_id < 0 || static_cast<size_t>(camera_id - 1) >= _potential_cameras.size()) {
        LogErr() << "Invalid camera ID";
        return 0;
    }

    return _potential_cameras[camera_id - 1].component_id;
}

int32_t CameraImpl::camera_id_for_potential_camera_with_lock(PotentialCamera& potential_camera)
{
    if (const auto it =
            std::find(_potential_cameras.begin(), _potential_cameras.end(), potential_camera);
        it != _potential_cameras.end()) {
        return static_cast<int32_t>(it - _potential_cameras.begin()) + 1;
    }
    // This should not happen as we never remove cameras.
    LogErr() << "Potential camera not found.";
    return 0;
}

CameraImpl::PotentialCamera*
CameraImpl::maybe_potential_camera_for_camera_id_with_lock(int32_t camera_id)
{
    if (camera_id <= 0 || (camera_id - 1) > static_cast<int32_t>(_potential_cameras.size())) {
        LogErr() << "Invalid camera ID";
        return nullptr;
    }

    return &_potential_cameras[camera_id - 1];
}

} // namespace mavsdk
