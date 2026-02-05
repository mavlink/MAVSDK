#include "camera_impl.h"
#include "camera_definition.h"
#include "system.h"
#include "unused.h"
#include "callback_list.tpp"
#include "fs_utils.h"
#include "string_utils.h"
#include "math_utils.h"

#if BUILD_WITHOUT_CURL != 1
#include "http_loader.h"
#endif

#include <algorithm>
#include <cassert>
#include <cmath>
#include <chrono>
#include <iterator>
#include <filesystem>
#include <functional>
#include <string>
#include <thread>

namespace mavsdk {

template class CallbackList<Camera::Mode>;
template class CallbackList<std::vector<Camera::Setting>>;
template class CallbackList<std::vector<Camera::SettingOptions>>;
template class CallbackList<Camera::CaptureInfo>;
template class CallbackList<Camera::VideoStreamInfo>;
template class CallbackList<Camera::Storage>;

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
    if (const char* env_p = std::getenv("MAVSDK_CAMERA_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Camera debugging is on.";
            _debugging = true;
        }
    }

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

    _request_missing_capture_info_cookie =
        _system_impl->add_call_every([this]() { request_missing_capture_info(); }, 0.5);

    // TODO: use SET_INTERVAL commands instead and fall back to request commands when needed.
    _request_slower_call_every_cookie =
        _system_impl->add_call_every([this]() { request_slower(); }, 20.0);
    _request_faster_call_every_cookie =
        _system_impl->add_call_every([this]() { request_faster(); }, 5.0);
}

void CameraImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers_blocking(this);

    _system_impl->cancel_all_param(this);

    _system_impl->remove_call_every(_request_missing_capture_info_cookie);
    _system_impl->remove_call_every(_request_slower_call_every_cookie);
    _system_impl->remove_call_every(_request_faster_call_every_cookie);

    // FIXME: There is a race condition here.
    // We need to wait until all call every calls are done before we go
    // out of scope.
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::lock_guard lock(_mutex);
    _storage_subscription_callbacks.clear();
    _mode_subscription_callbacks.clear();
    _capture_info_callbacks.clear();
    _video_stream_info_subscription_callbacks.clear();
    _camera_list_subscription_callbacks.clear();
    _subscribe_current_settings_callbacks.clear();
    _subscribe_possible_setting_options_callbacks.clear();
}

void CameraImpl::enable() {}
void CameraImpl::disable() {}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_take_photo(int32_t component_id, float interval_s, float no_of_photos)
{
    MavlinkCommandSender::CommandLong cmd{};

    cmd.command = MAV_CMD_IMAGE_START_CAPTURE;
    cmd.params.maybe_param1 = 0.0f; // Reserved, set to 0
    cmd.params.maybe_param2 = interval_s;
    cmd.params.maybe_param3 = no_of_photos;
    cmd.params.maybe_param4 = get_and_increment_capture_sequence(component_id);
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_zoom_out(int32_t component_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_ZOOM;
    cmd.params.maybe_param1 = static_cast<float>(ZOOM_TYPE_CONTINUOUS);
    cmd.params.maybe_param2 = -1.f;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_zoom_in(int32_t component_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_ZOOM;
    cmd.params.maybe_param1 = static_cast<float>(ZOOM_TYPE_CONTINUOUS);
    cmd.params.maybe_param2 = 1.f;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_zoom_stop(int32_t component_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_ZOOM;
    cmd.params.maybe_param1 = static_cast<float>(ZOOM_TYPE_CONTINUOUS);
    cmd.params.maybe_param2 = 0.f;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_zoom_range(int32_t component_id, float range)
{
    // Clip to safe range.
    range = std::max(0.f, std::min(range, 100.f));

    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_ZOOM;
    cmd.params.maybe_param1 = static_cast<float>(ZOOM_TYPE_RANGE);
    cmd.params.maybe_param2 = range;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_track_point(
    int32_t component_id, float point_x, float point_y, float radius)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_CAMERA_TRACK_POINT;
    cmd.params.maybe_param1 = point_x;
    cmd.params.maybe_param2 = point_y;
    cmd.params.maybe_param3 = radius;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_track_rectangle(
    int32_t component_id,
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
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_track_stop(int32_t component_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_CAMERA_STOP_TRACKING;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_focus_in(int32_t component_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_FOCUS;
    cmd.params.maybe_param1 = static_cast<float>(FOCUS_TYPE_CONTINUOUS);
    cmd.params.maybe_param2 = -1.f;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_focus_out(int32_t component_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_FOCUS;
    cmd.params.maybe_param1 = static_cast<float>(FOCUS_TYPE_CONTINUOUS);
    cmd.params.maybe_param2 = 1.f;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_focus_stop(int32_t component_id)
{
    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_FOCUS;
    cmd.params.maybe_param1 = static_cast<float>(FOCUS_TYPE_CONTINUOUS);
    cmd.params.maybe_param2 = 0.f;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_focus_range(int32_t component_id, float range)
{
    // Clip to safe range.
    range = std::max(0.f, std::min(range, 100.f));

    MavlinkCommandSender::CommandLong cmd{};
    cmd.command = MAV_CMD_SET_CAMERA_FOCUS;
    cmd.params.maybe_param1 = static_cast<float>(FOCUS_TYPE_RANGE);
    cmd.params.maybe_param2 = range;
    cmd.target_component_id = fixup_component_target(component_id);

    return cmd;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_stop_photo(int32_t component_id)
{
    MavlinkCommandSender::CommandLong cmd_stop_photo{};

    cmd_stop_photo.command = MAV_CMD_IMAGE_STOP_CAPTURE;
    cmd_stop_photo.target_component_id = fixup_component_target(component_id);

    return cmd_stop_photo;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_start_video(int32_t component_id, float capture_status_rate_hz)
{
    MavlinkCommandSender::CommandLong cmd_start_video{};

    cmd_start_video.command = MAV_CMD_VIDEO_START_CAPTURE;
    cmd_start_video.params.maybe_param1 = 0.f; // Reserved, set to 0
    cmd_start_video.params.maybe_param2 = capture_status_rate_hz;
    cmd_start_video.target_component_id = fixup_component_target(component_id);

    return cmd_start_video;
}

MavlinkCommandSender::CommandLong CameraImpl::make_command_stop_video(int32_t component_id)
{
    MavlinkCommandSender::CommandLong cmd_stop_video{};

    cmd_stop_video.command = MAV_CMD_VIDEO_STOP_CAPTURE;
    cmd_stop_video.params.maybe_param1 = 0.f; // Reserved, set to 0
    cmd_stop_video.target_component_id = fixup_component_target(component_id);

    return cmd_stop_video;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_set_camera_mode(int32_t component_id, float mavlink_mode)
{
    MavlinkCommandSender::CommandLong cmd_set_camera_mode{};

    cmd_set_camera_mode.command = MAV_CMD_SET_CAMERA_MODE;
    cmd_set_camera_mode.params.maybe_param1 = 0.0f; // Reserved, set to 0
    cmd_set_camera_mode.params.maybe_param2 = mavlink_mode;
    cmd_set_camera_mode.target_component_id = fixup_component_target(component_id);

    return cmd_set_camera_mode;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_start_video_streaming(int32_t component_id, int32_t stream_id)
{
    MavlinkCommandSender::CommandLong cmd_start_video_streaming{};

    cmd_start_video_streaming.command = MAV_CMD_VIDEO_START_STREAMING;
    cmd_start_video_streaming.params.maybe_param1 = static_cast<float>(stream_id);
    cmd_start_video_streaming.target_component_id = fixup_component_target(component_id);

    return cmd_start_video_streaming;
}

MavlinkCommandSender::CommandLong
CameraImpl::make_command_stop_video_streaming(int32_t component_id, int32_t stream_id)
{
    MavlinkCommandSender::CommandLong cmd_stop_video_streaming{};

    cmd_stop_video_streaming.command = MAV_CMD_VIDEO_STOP_STREAMING;
    cmd_stop_video_streaming.params.maybe_param1 = static_cast<float>(stream_id);
    cmd_stop_video_streaming.target_component_id = fixup_component_target(component_id);

    return cmd_stop_video_streaming;
}

Camera::Result CameraImpl::take_photo(int32_t component_id)
{
    // Take 1 photo only with no interval
    auto cmd_take_photo = make_command_take_photo(component_id, 0.f, 1.0f);

    return camera_result_from_command_result(_system_impl->send_command(cmd_take_photo));
}

Camera::Result CameraImpl::zoom_out_start(int32_t component_id)
{
    auto cmd = make_command_zoom_out(component_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::zoom_in_start(int32_t component_id)
{
    auto cmd = make_command_zoom_in(component_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::zoom_stop(int32_t component_id)
{
    auto cmd = make_command_zoom_stop(component_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::zoom_range(int32_t component_id, float range)
{
    auto cmd = make_command_zoom_range(component_id, range);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result
CameraImpl::track_point(int32_t component_id, float point_x, float point_y, float radius)
{
    auto cmd = make_command_track_point(component_id, point_x, point_y, radius);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::track_rectangle(
    int32_t component_id,
    float top_left_x,
    float top_left_y,
    float bottom_right_x,
    float bottom_right_y)
{
    auto cmd = make_command_track_rectangle(
        component_id, top_left_x, top_left_y, bottom_right_x, bottom_right_y);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::track_stop(int32_t component_id)
{
    auto cmd = make_command_track_stop(component_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::focus_in_start(int32_t component_id)
{
    auto cmd = make_command_focus_in(component_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::focus_out_start(int32_t component_id)
{
    auto cmd = make_command_focus_out(component_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::focus_stop(int32_t component_id)
{
    auto cmd = make_command_focus_stop(component_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::focus_range(int32_t component_id, float range)
{
    auto cmd = make_command_focus_range(component_id, range);

    return camera_result_from_command_result(_system_impl->send_command(cmd));
}

Camera::Result CameraImpl::start_photo_interval(int32_t component_id, float interval_s)
{
    auto cmd_take_photo_time_lapse = make_command_take_photo(component_id, interval_s, 0.f);

    return camera_result_from_command_result(_system_impl->send_command(cmd_take_photo_time_lapse));
}

Camera::Result CameraImpl::stop_photo_interval(int32_t component_id)
{
    auto cmd_stop_photo_interval = make_command_stop_photo(component_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd_stop_photo_interval));
}

Camera::Result CameraImpl::start_video(int32_t component_id)
{
    // Capture status rate is not set
    auto cmd_start_video = make_command_start_video(component_id, 0.f);

    return camera_result_from_command_result(_system_impl->send_command(cmd_start_video));
}

Camera::Result CameraImpl::stop_video(int32_t component_id)
{
    auto cmd_stop_video = make_command_stop_video(component_id);

    return camera_result_from_command_result(_system_impl->send_command(cmd_stop_video));
}

void CameraImpl::zoom_in_start_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    auto cmd = make_command_zoom_in(component_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::zoom_out_start_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    auto cmd = make_command_zoom_out(component_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::zoom_stop_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    auto cmd = make_command_zoom_stop(component_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::zoom_range_async(
    int32_t component_id, float range, const Camera::ResultCallback& callback)
{
    auto cmd = make_command_zoom_range(component_id, range);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::track_point_async(
    int32_t component_id,
    float point_x,
    float point_y,
    float radius,
    const Camera::ResultCallback& callback)
{
    auto cmd = make_command_track_point(component_id, point_x, point_y, radius);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::track_rectangle_async(
    int32_t component_id,
    float top_left_x,
    float top_left_y,
    float bottom_right_x,
    float bottom_right_y,
    const Camera::ResultCallback& callback)
{
    auto cmd = make_command_track_rectangle(
        component_id, top_left_x, top_left_y, bottom_right_x, bottom_right_y);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::track_stop_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    auto cmd = make_command_track_stop(component_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::focus_in_start_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    auto cmd = make_command_focus_in(component_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::focus_out_start_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    auto cmd = make_command_focus_out(component_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::focus_stop_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    auto cmd = make_command_focus_stop(component_id);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::focus_range_async(
    int32_t component_id, float range, const Camera::ResultCallback& callback)
{
    auto cmd = make_command_focus_range(component_id, range);

    _system_impl->send_command_async(
        cmd, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::take_photo_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    // Take 1 photo only with no interval
    auto cmd_take_photo = make_command_take_photo(component_id, 0.f, 1.0f);

    _system_impl->send_command_async(
        cmd_take_photo, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::start_photo_interval_async(
    int32_t component_id, float interval_s, const Camera::ResultCallback& callback)
{
    auto cmd_take_photo_time_lapse = make_command_take_photo(component_id, interval_s, 0.f);

    _system_impl->send_command_async(
        cmd_take_photo_time_lapse, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::stop_photo_interval_async(
    int32_t component_id, const Camera::ResultCallback& callback)
{
    auto cmd_stop_photo_interval = make_command_stop_photo(component_id);

    _system_impl->send_command_async(
        cmd_stop_photo_interval, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::start_video_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    // Capture status rate is not set
    auto cmd_start_video = make_command_start_video(component_id, 0.f);

    _system_impl->send_command_async(
        cmd_start_video, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

void CameraImpl::stop_video_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    auto cmd_stop_video = make_command_stop_video(component_id);

    _system_impl->send_command_async(
        cmd_stop_video, [this, callback](MavlinkCommandSender::Result result, float) {
            receive_command_result(result, callback);
        });
}

Camera::CameraList CameraImpl::camera_list()
{
    std::lock_guard lock(_mutex);

    return camera_list_with_lock();
}

Camera::CameraList CameraImpl::camera_list_with_lock()
{
    Camera::CameraList result{};

    for (auto& potential_camera : potential_cameras_with_lock()) {
        if (!potential_camera->maybe_information) {
            continue;
        }
        result.cameras.push_back(potential_camera->maybe_information.value());
    }

    return result;
}

Camera::CameraListHandle
CameraImpl::subscribe_camera_list(const Camera::CameraListCallback& callback)
{
    std::lock_guard lock(_mutex);
    auto handle = _camera_list_subscription_callbacks.subscribe(callback);

    notify_camera_list_with_lock();

    return handle;
}

void CameraImpl::unsubscribe_camera_list(Camera::CameraListHandle handle)
{
    std::lock_guard lock(_mutex);
    _camera_list_subscription_callbacks.unsubscribe(handle);
}

void CameraImpl::notify_camera_list_with_lock()
{
    _system_impl->call_user_callback([&]() {
        _camera_list_subscription_callbacks.queue(
            camera_list_with_lock(), [this](const auto& func) { func(); });
    });
}

Camera::Result CameraImpl::start_video_streaming(int32_t component_id, int32_t stream_id)
{
    auto command = make_command_start_video_streaming(component_id, stream_id);

    return camera_result_from_command_result(_system_impl->send_command(command));
}

Camera::Result CameraImpl::stop_video_streaming(int32_t component_id, int32_t stream_id)
{
    auto command = make_command_stop_video_streaming(component_id, stream_id);

    return camera_result_from_command_result(_system_impl->send_command(command));
}

void CameraImpl::request_slower()
{
    std::lock_guard lock(_mutex);

    for (auto& camera : _potential_cameras) {
        _system_impl->mavlink_request_message().request(
            MAVLINK_MSG_ID_VIDEO_STREAM_INFORMATION,
            fixup_component_target(camera.component_id),
            nullptr);
    }
}

void CameraImpl::request_faster()
{
    std::lock_guard lock(_mutex);

    for (auto& camera : _potential_cameras) {
        _system_impl->mavlink_request_message().request(
            MAVLINK_MSG_ID_VIDEO_STREAM_STATUS,
            fixup_component_target(camera.component_id),
            nullptr);

        _system_impl->mavlink_request_message().request(
            MAVLINK_MSG_ID_CAMERA_CAPTURE_STATUS,
            fixup_component_target(camera.component_id),
            nullptr);

        _system_impl->mavlink_request_message().request(
            MAVLINK_MSG_ID_STORAGE_INFORMATION,
            fixup_component_target(camera.component_id),
            nullptr);

        _system_impl->mavlink_request_message().request(
            MAVLINK_MSG_ID_CAMERA_SETTINGS, fixup_component_target(camera.component_id), nullptr);
    }
}

Camera::VideoStreamInfoHandle
CameraImpl::subscribe_video_stream_info(const Camera::VideoStreamInfoCallback& callback)
{
    std::lock_guard lock(_mutex);

    auto handle = _video_stream_info_subscription_callbacks.subscribe(callback);

    notify_video_stream_info_for_all_with_lock();

    return handle;
}

void CameraImpl::unsubscribe_video_stream_info(Camera::VideoStreamInfoHandle handle)
{
    std::lock_guard lock(_mutex);
    _video_stream_info_subscription_callbacks.unsubscribe(handle);
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
            // FALLTHROUGH
        case MavlinkCommandSender::Result::Failed:
            return Camera::Result::Error;
        case MavlinkCommandSender::Result::Denied:
            // FALLTHROUGH
        case MavlinkCommandSender::Result::TemporarilyRejected:
            return Camera::Result::Denied;
        case MavlinkCommandSender::Result::Timeout:
            return Camera::Result::Timeout;
        case MavlinkCommandSender::Result::Unsupported:
            return Camera::Result::ActionUnsupported;
        case MavlinkCommandSender::Result::Cancelled:
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
        case MavlinkParameterClient::Result::WrongType:
            // FALLTHROUGH
        case MavlinkParameterClient::Result::ParamNameTooLong:
            // FALLTHROUGH
        case MavlinkParameterClient::Result::NotFound:
            // FALLTHROUGH
        case MavlinkParameterClient::Result::ValueUnsupported:
            return Camera::Result::WrongArgument;
        case MavlinkParameterClient::Result::ConnectionError:
            // FALLTHROUGH
        case MavlinkParameterClient::Result::Failed:
            // FALLTHROUGH
        case MavlinkParameterClient::Result::UnknownError:
            return Camera::Result::Error;
        default:
            return Camera::Result::Unknown;
    }
}

Camera::Result CameraImpl::set_mode(int32_t component_id, const Camera::Mode mode)
{
    const float mavlink_mode = to_mavlink_camera_mode(mode);
    auto cmd_set_camera_mode = make_command_set_camera_mode(component_id, mavlink_mode);
    const auto command_result = _system_impl->send_command(cmd_set_camera_mode);
    const auto camera_result = camera_result_from_command_result(command_result);

    if (camera_result == Camera::Result::Success) {
        std::lock_guard lock(_mutex);
        auto maybe_potential_camera =
            maybe_potential_camera_for_component_id_with_lock(component_id, 0);
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
    // different from {PHOTO, VIDEO}, in which case the mode received from
    // CAMERA_SETTINGS will be wrong.

    if (!potential_camera.camera_definition) {
        return;
    }

    ParamValue value;
    if (potential_camera.camera_definition->get_setting("CAM_MODE", value)) {
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
    refresh_params_with_lock(potential_camera, false);
    notify_mode_with_lock(potential_camera);
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
    int32_t component_id, const Camera::Mode mode, const Camera::ResultCallback& callback)
{
    const auto mavlink_mode = to_mavlink_camera_mode(mode);
    auto cmd_set_camera_mode = make_command_set_camera_mode(component_id, mavlink_mode);

    _system_impl->send_command_async(
        cmd_set_camera_mode,
        [this, callback, mode, component_id](MavlinkCommandSender::Result result, float progress) {
            UNUSED(progress);
            receive_set_mode_command_result(result, callback, mode, component_id);
        });
}

Camera::ModeHandle CameraImpl::subscribe_mode(const Camera::ModeCallback& callback)
{
    std::lock_guard lock(_mutex);
    auto handle = _mode_subscription_callbacks.subscribe(callback);

    notify_mode_for_all_with_lock();

    return handle;
}

void CameraImpl::unsubscribe_mode(Camera::ModeHandle handle)
{
    std::lock_guard lock(_mutex);
    _mode_subscription_callbacks.unsubscribe(handle);
}

Camera::StorageHandle CameraImpl::subscribe_storage(const Camera::StorageCallback& callback)
{
    std::lock_guard lock(_mutex);
    auto handle = _storage_subscription_callbacks.subscribe(callback);

    notify_storage_for_all_with_lock();

    return handle;
}

void CameraImpl::unsubscribe_storage(Camera::StorageHandle handle)
{
    std::lock_guard lock(_mutex);
    _storage_subscription_callbacks.unsubscribe(handle);
}

Camera::CaptureInfoHandle
CameraImpl::subscribe_capture_info(const Camera::CaptureInfoCallback& callback)
{
    std::lock_guard lock(_mutex);
    return _capture_info_callbacks.subscribe(callback);
}

void CameraImpl::unsubscribe_capture_info(Camera::CaptureInfoHandle handle)
{
    std::lock_guard lock(_mutex);
    _capture_info_callbacks.unsubscribe(handle);
}

void CameraImpl::process_heartbeat(const mavlink_message_t& message)
{
    // Check for potential camera
    std::lock_guard lock(_mutex);
    auto found =
        std::any_of(_potential_cameras.begin(), _potential_cameras.end(), [&](const auto& item) {
            return item.component_id == message.compid;
        });

    if (!found) {
        _potential_cameras.emplace_back(message.compid);
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

    std::lock_guard lock(_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_component_id_with_lock(
        message.compid, camera_capture_status.camera_device_id);

    if (maybe_potential_camera == nullptr) {
        return;
    }

    auto& camera = *maybe_potential_camera;

    // If image_count got smaller, consider that the storage was formatted.
    if (camera_capture_status.image_count < camera.capture_status.image_count) {
        LogInfo() << "Seems like storage was formatted, setting state accordingly";
        reset_following_format_storage_with_lock(camera);
    }

    camera.storage.video_on = (camera_capture_status.video_status == 1);
    camera.storage.photo_interval_on =
        (camera_capture_status.image_status == 2 || camera_capture_status.image_status == 3);
    camera.capture_status.received_camera_capture_status = true;
    camera.storage.recording_time_s =
        static_cast<float>(camera_capture_status.recording_time_ms) / 1e3f;

    camera.capture_status.image_count = camera_capture_status.image_count;

    if (camera.capture_status.image_count_at_connection == -1) {
        camera.capture_status.image_count_at_connection = camera_capture_status.image_count;
    }
}

void CameraImpl::process_storage_information(const mavlink_message_t& message)
{
    mavlink_storage_information_t storage_information;
    mavlink_msg_storage_information_decode(&message, &storage_information);

    std::lock_guard lock(_mutex);
    // TODO: should STORAGE_INFORMATION have a camera_device_id?
    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(message.compid, 0);

    if (maybe_potential_camera == nullptr) {
        return;
    }

    auto& camera = *maybe_potential_camera;

    camera.storage.storage_status = storage_status_from_mavlink(storage_information.status);
    camera.storage.available_storage_mib = storage_information.available_capacity;
    camera.storage.used_storage_mib = storage_information.used_capacity;
    camera.storage.total_storage_mib = storage_information.total_capacity;
    camera.storage.storage_id = storage_information.storage_id;
    camera.storage.storage_type = storage_type_from_mavlink(storage_information.type);
    camera.received_storage = true;

    notify_storage_with_lock(camera);
}

Camera::Storage::StorageStatus CameraImpl::storage_status_from_mavlink(const int storage_status)
{
    switch (storage_status) {
        case STORAGE_STATUS_EMPTY:
            return Camera::Storage::StorageStatus::NotAvailable;
        case STORAGE_STATUS_UNFORMATTED:
            return Camera::Storage::StorageStatus::Unformatted;
        case STORAGE_STATUS_READY:
            return Camera::Storage::StorageStatus::Formatted;
            break;
        case STORAGE_STATUS_NOT_SUPPORTED:
            return Camera::Storage::StorageStatus::NotSupported;
        default:
            LogErr() << "Unknown storage status received.";
            return Camera::Storage::StorageStatus::NotSupported;
    }
}

Camera::Storage::StorageType CameraImpl::storage_type_from_mavlink(const int storage_type)
{
    switch (storage_type) {
        default:
            LogErr() << "Unknown storage_type enum value: " << storage_type;
        // FALLTHROUGH
        case STORAGE_TYPE_UNKNOWN:
            return mavsdk::Camera::Storage::StorageType::Unknown;
        case STORAGE_TYPE_USB_STICK:
            return mavsdk::Camera::Storage::StorageType::UsbStick;
        case STORAGE_TYPE_SD:
            return mavsdk::Camera::Storage::StorageType::Sd;
        case STORAGE_TYPE_MICROSD:
            return mavsdk::Camera::Storage::StorageType::Microsd;
        case STORAGE_TYPE_HD:
            return mavsdk::Camera::Storage::StorageType::Hd;
        case STORAGE_TYPE_OTHER:
            return mavsdk::Camera::Storage::StorageType::Other;
    }
}

void CameraImpl::process_camera_image_captured(const mavlink_message_t& message)
{
    mavlink_camera_image_captured_t image_captured;
    mavlink_msg_camera_image_captured_decode(&message, &image_captured);

    std::lock_guard lock(_mutex);
    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(message.compid, image_captured.camera_id);

    if (maybe_potential_camera == nullptr) {
        return;
    }

    auto& camera = *maybe_potential_camera;

    mavsdk::Quaternion quaternion{};
    quaternion.w = image_captured.q[0];
    quaternion.x = image_captured.q[1];
    quaternion.y = image_captured.q[2];
    quaternion.z = image_captured.q[3];
    auto euler = to_euler_angle_from_quaternion(quaternion);

    Camera::CaptureInfo capture_info = {};
    capture_info.component_id = camera.component_id;
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

    camera.capture_status.photo_list.insert(
        std::make_pair(image_captured.image_index, capture_info));

    // Notify user if a new image has been captured.
    if (camera.capture_info.last_advertised_image_index < capture_info.index) {
        _capture_info_callbacks.queue(
            capture_info, [this](const auto& func) { _system_impl->call_user_callback(func); });

        if (camera.capture_info.last_advertised_image_index != -1) {
            // Save captured indices that have been dropped to request later, however, don't
            // do it from the very beginning as there might be many photos from a previous
            // time that we don't want to request.
            for (int i = camera.capture_info.last_advertised_image_index + 1;
                 i < capture_info.index;
                 ++i) {
                if (camera.capture_info.missing_image_retries.find(i) ==
                    camera.capture_info.missing_image_retries.end()) {
                    camera.capture_info.missing_image_retries[i] = 0;
                }
            }
        }

        camera.capture_info.last_advertised_image_index = capture_info.index;
    }

    else if (auto it = camera.capture_info.missing_image_retries.find(capture_info.index);
             it != camera.capture_info.missing_image_retries.end()) {
        _capture_info_callbacks.queue(
            capture_info, [this](const auto& func) { _system_impl->call_user_callback(func); });
        camera.capture_info.missing_image_retries.erase(it);
    }
}

void CameraImpl::request_missing_capture_info()
{
    std::lock_guard lock(_mutex);
    for (auto& potential_camera : _potential_cameras) {
        // Clean out entries once we have tried 3 times.
        for (auto it = potential_camera.capture_info.missing_image_retries.begin();
             it != potential_camera.capture_info.missing_image_retries.end();
             /* ++it */) {
            if (it->second > 3) {
                it = potential_camera.capture_info.missing_image_retries.erase(it);
            } else {
                ++it;
            }
        }

        // Request a few entries, 3 each time.
        for (unsigned i = 0; i < 3; ++i) {
            if (!potential_camera.capture_info.missing_image_retries.empty()) {
                auto it_lowest_retries = std::min_element(
                    potential_camera.capture_info.missing_image_retries.begin(),
                    potential_camera.capture_info.missing_image_retries.end());
                _system_impl->mavlink_request_message().request(
                    MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED,
                    potential_camera.component_id,
                    nullptr,
                    it_lowest_retries->first);
                it_lowest_retries->second += 1;
            }
        }
    }
}

void CameraImpl::process_camera_settings(const mavlink_message_t& message)
{
    mavlink_camera_settings_t camera_settings;
    mavlink_msg_camera_settings_decode(&message, &camera_settings);

    std::lock_guard lock(_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_component_id_with_lock(
        message.compid, camera_settings.camera_device_id);

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
    // TODO: Check the case for 1-6.
    new_information.component_id = message.compid;
    new_information.vendor_name = reinterpret_cast<char*>(camera_information.vendor_name);
    new_information.model_name = reinterpret_cast<char*>(camera_information.model_name);
    new_information.focal_length_mm = camera_information.focal_length;
    new_information.horizontal_sensor_size_mm = camera_information.sensor_size_h;
    new_information.vertical_sensor_size_mm = camera_information.sensor_size_v;
    new_information.horizontal_resolution_px = camera_information.resolution_h;
    new_information.vertical_resolution_px = camera_information.resolution_v;

    std::lock_guard lock(_mutex);

    auto potential_camera =
        std::find_if(_potential_cameras.begin(), _potential_cameras.end(), [&](auto& item) {
            return item.component_id == message.compid;
        });

    if (potential_camera == _potential_cameras.end()) {
        _potential_cameras.emplace_back(message.compid);
        potential_camera = std::prev(_potential_cameras.end());
    }

    // We need a copy of the component ID inside the information.
    potential_camera->component_id = new_information.component_id;
    potential_camera->maybe_information = new_information;
    potential_camera->camera_definition_url = camera_information.cam_definition_uri;
    potential_camera->camera_definition_version = camera_information.cam_definition_version;
    check_camera_definition_with_lock(*potential_camera);
}

void CameraImpl::check_camera_definition_with_lock(PotentialCamera& potential_camera)
{
    const std::string url = potential_camera.camera_definition_url;

    if (potential_camera.camera_definition_url.empty()) {
        potential_camera.camera_definition_result = Camera::Result::Unavailable;
        notify_camera_list_with_lock();
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
        LogInfo() << "Using cached file " << cached_file_option.value();
        load_camera_definition_with_lock(potential_camera, cached_file_option.value());
        potential_camera.is_fetching_camera_definition = false;
        potential_camera.camera_definition_result = Camera::Result::Success;
        notify_camera_list_with_lock();

    } else {
        potential_camera.is_fetching_camera_definition = true;

        if (url.empty()) {
            LogInfo() << "No camera definition URL available";
            potential_camera.camera_definition_result = Camera::Result::ProtocolUnsupported;
            notify_camera_list_with_lock();

        } else if (starts_with(url, "http://") || starts_with(url, "https://")) {
#if BUILD_WITHOUT_CURL == 1
            potential_camera.camera_definition_result = Camera::Result::ProtocolUnsupported;
            notify_camera_list_with_lock();
#else
            if (_http_loader == nullptr) {
                _http_loader = std::make_unique<HttpLoader>();
            }
            LogInfo() << "Downloading camera definition from: " << url;

            auto component_id = potential_camera.component_id;

            auto download_path = _tmp_download_path / file_cache_tag;

            _http_loader->download_async(
                url,
                download_path.string(),
                [download_path, file_cache_tag, component_id, this](
                    int progress, HttpStatus status, CURLcode curl_code) mutable {
                    // TODO: check if we still exist
                    LogDebug() << "Download progress: " << progress
                               << ", status: " << static_cast<int>(status)
                               << ", curl_code: " << std::to_string(curl_code);

                    std::lock_guard lock(_mutex);
                    auto maybe_potential_camera =
                        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
                    if (maybe_potential_camera == nullptr) {
                        LogErr() << "Failed to find camera.";
                    }

                    if (status == HttpStatus::Error) {
                        LogErr() << "File download failed with result "
                                 << std::to_string(curl_code);
                        maybe_potential_camera->is_fetching_camera_definition = false;
                        maybe_potential_camera->camera_definition_result = Camera::Result::Error;
                        notify_camera_list_with_lock();

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
                        maybe_potential_camera->camera_definition_result = Camera::Result::Success;
                        notify_camera_list_with_lock();
                    }
                });
#endif
        } else if (starts_with(url, "mftp://") || starts_with(url, "mavlinkftp://")) {
            LogInfo() << "Download file: " << url << " using MAVLink FTP...";

            auto component_id = potential_camera.component_id;

            auto downloaded_filename = strip_prefix(strip_prefix(url, "mavlinkftp://"), "mftp://");

            _system_impl->mavlink_ftp_client().download_async(
                downloaded_filename,
                _tmp_download_path.string(),
                false,
                [file_cache_tag, downloaded_filename, component_id, this](
                    MavlinkFtpClient::ClientResult client_result,
                    MavlinkFtpClient::ProgressData progress_data) mutable {
                    // TODO: check if we still exist
                    if (client_result == MavlinkFtpClient::ClientResult::Next) {
                        LogDebug()
                            << "Mavlink FTP download progress: "
                            << 100 * progress_data.bytes_transferred / progress_data.total_bytes
                            << " %";
                        return;
                    }

                    // Use call_user_callback to defer callback execution and avoid deadlock
                    _system_impl->call_user_callback(
                        [file_cache_tag, downloaded_filename, component_id, client_result, this]() {
                            std::lock_guard lock(_mutex);
                            auto maybe_potential_camera =
                                maybe_potential_camera_for_component_id_with_lock(component_id, 0);
                            if (maybe_potential_camera == nullptr) {
                                LogErr() << "Failed to find camera with ID " << component_id;
                                return;
                            }

                            if (client_result != MavlinkFtpClient::ClientResult::Success) {
                                LogErr() << "File download failed with result " << client_result;
                                maybe_potential_camera->is_fetching_camera_definition = false;
                                maybe_potential_camera->camera_definition_result =
                                    Camera::Result::Error;
                                notify_camera_list_with_lock();
                                return;
                            }

                            auto downloaded_filepath = _tmp_download_path / downloaded_filename;

                            LogDebug() << "File download finished to " << downloaded_filepath;
                            if (_file_cache) {
                                // Cache the file (this will move/remove the temp file as well)
                                downloaded_filepath =
                                    _file_cache->insert(file_cache_tag, downloaded_filepath)
                                        .value_or(downloaded_filepath);
                                LogDebug() << "Cached path: " << downloaded_filepath;
                            }
                            load_camera_definition_with_lock(
                                *maybe_potential_camera, downloaded_filepath);
                            maybe_potential_camera->is_fetching_camera_definition = false;
                            maybe_potential_camera->camera_definition_result =
                                Camera::Result::Success;
                            notify_camera_list_with_lock();
                        });
                });
        } else {
            LogErr() << "Unknown protocol for URL: " << url;
            potential_camera.camera_definition_result = Camera::Result::ProtocolUnsupported;
            notify_camera_list_with_lock();
        }
    }
}

void CameraImpl::load_camera_definition_with_lock(
    PotentialCamera& potential_camera, const std::filesystem::path& path)
{
    if (potential_camera.camera_definition == nullptr) {
        potential_camera.camera_definition = std::make_unique<CameraDefinition>();
    }

    if (!potential_camera.camera_definition->load_file(path.string())) {
        LogErr() << "Failed to load camera definition: " << path;
        // We can't keep something around that's not loaded correctly.
        potential_camera.camera_definition = nullptr;
        return;
    }

    // We assume default settings initially and then load the params one by one.
    // This way we can start using it straightaway.
    potential_camera.camera_definition->reset_to_default_settings(true);

    refresh_params_with_lock(potential_camera, true);
}

void CameraImpl::process_video_information(const mavlink_message_t& message)
{
    mavlink_video_stream_information_t received_video_info;
    mavlink_msg_video_stream_information_decode(&message, &received_video_info);

    std::lock_guard lock(_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_component_id_with_lock(
        message.compid, received_video_info.camera_device_id);

    if (maybe_potential_camera == nullptr) {
        return;
    }

    auto& camera = *maybe_potential_camera;

    // TODO: use stream_id and count
    camera.video_stream_info.status =
        (received_video_info.flags & VIDEO_STREAM_STATUS_FLAGS_RUNNING ?
             Camera::VideoStreamInfo::VideoStreamStatus::InProgress :
             Camera::VideoStreamInfo::VideoStreamStatus::NotRunning);
    camera.video_stream_info.spectrum =
        (received_video_info.flags & VIDEO_STREAM_STATUS_FLAGS_THERMAL ?
             Camera::VideoStreamInfo::VideoStreamSpectrum::Infrared :
             Camera::VideoStreamInfo::VideoStreamSpectrum::VisibleLight);

    auto& video_stream_info = camera.video_stream_info.settings;
    video_stream_info.frame_rate_hz = received_video_info.framerate;
    video_stream_info.horizontal_resolution_pix = received_video_info.resolution_h;
    video_stream_info.vertical_resolution_pix = received_video_info.resolution_v;
    video_stream_info.bit_rate_b_s = received_video_info.bitrate;
    video_stream_info.rotation_deg = received_video_info.rotation;
    video_stream_info.horizontal_fov_deg = static_cast<float>(received_video_info.hfov);
    video_stream_info.uri = received_video_info.uri;
    camera.video_stream_info.stream_id = received_video_info.stream_id;
    camera.received_video_stream_info = true;

    notify_video_stream_info_with_lock(camera);
}

void CameraImpl::process_video_stream_status(const mavlink_message_t& message)
{
    mavlink_video_stream_status_t received_video_stream_status;
    mavlink_msg_video_stream_status_decode(&message, &received_video_stream_status);

    std::lock_guard lock(_mutex);
    auto maybe_potential_camera = maybe_potential_camera_for_component_id_with_lock(
        message.compid, received_video_stream_status.camera_device_id);

    if (maybe_potential_camera == nullptr) {
        return;
    }

    auto& camera = *maybe_potential_camera;

    camera.video_stream_info.status =
        (received_video_stream_status.flags & VIDEO_STREAM_STATUS_FLAGS_RUNNING ?
             Camera::VideoStreamInfo::VideoStreamStatus::InProgress :
             Camera::VideoStreamInfo::VideoStreamStatus::NotRunning);
    camera.video_stream_info.spectrum =
        (received_video_stream_status.flags & VIDEO_STREAM_STATUS_FLAGS_THERMAL ?
             Camera::VideoStreamInfo::VideoStreamSpectrum::Infrared :
             Camera::VideoStreamInfo::VideoStreamSpectrum::VisibleLight);

    auto& video_stream_info = camera.video_stream_info.settings;
    video_stream_info.frame_rate_hz = received_video_stream_status.framerate;
    video_stream_info.horizontal_resolution_pix = received_video_stream_status.resolution_h;
    video_stream_info.vertical_resolution_pix = received_video_stream_status.resolution_v;
    video_stream_info.bit_rate_b_s = received_video_stream_status.bitrate;
    video_stream_info.rotation_deg = received_video_stream_status.rotation;
    video_stream_info.horizontal_fov_deg = static_cast<float>(received_video_stream_status.hfov);

    // We only set this when we get the static information rather than just the status.
    // camera.received_video_stream_info = true;

    notify_video_stream_info_with_lock(camera);
}

void CameraImpl::notify_video_stream_info_for_all_with_lock()
{
    for (auto& camera : _potential_cameras) {
        notify_video_stream_info_with_lock(camera);
    }
}

void CameraImpl::notify_video_stream_info_with_lock(PotentialCamera& camera)
{
    if (!camera.received_video_stream_info) {
        return;
    }

    _video_stream_info_subscription_callbacks.queue(
        Camera::VideoStreamUpdate{camera.component_id, camera.video_stream_info},
        [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void CameraImpl::notify_storage_for_all_with_lock()
{
    for (auto& potential_camera : potential_cameras_with_lock()) {
        notify_storage_with_lock(*potential_camera);
    }
}

void CameraImpl::notify_storage_with_lock(PotentialCamera& camera)
{
    if (!camera.received_storage) {
        return;
    }

    _storage_subscription_callbacks.queue(
        Camera::StorageUpdate{camera.component_id, camera.storage},
        [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void CameraImpl::receive_command_result(
    MavlinkCommandSender::Result command_result, const Camera::ResultCallback& callback) const
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    if (callback) {
        _system_impl->call_user_callback([callback, camera_result]() { callback(camera_result); });
    }
}

void CameraImpl::receive_set_mode_command_result(
    const MavlinkCommandSender::Result command_result,
    const Camera::ResultCallback& callback,
    const Camera::Mode mode,
    int32_t component_id)
{
    Camera::Result camera_result = camera_result_from_command_result(command_result);

    if (callback) {
        _system_impl->call_user_callback([callback, camera_result]() { callback(camera_result); });
    }

    if (camera_result == Camera::Result::Success) {
        std::lock_guard lock(_mutex);
        auto maybe_potential_camera =
            maybe_potential_camera_for_component_id_with_lock(component_id, 0);
        if (maybe_potential_camera != nullptr) {
            save_camera_mode_with_lock(*maybe_potential_camera, mode);
        }
    }
}

void CameraImpl::notify_mode_for_all_with_lock()
{
    for (auto& camera : potential_cameras_with_lock()) {
        notify_mode_with_lock(*camera);
    }
}

void CameraImpl::notify_mode_with_lock(PotentialCamera& camera)
{
    _mode_subscription_callbacks.queue(
        Camera::ModeUpdate{camera.component_id, camera.mode},
        [this](const auto& func) { _system_impl->call_user_callback(func); });
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
        Camera::Option option{};
        option.option_id = value.get_string();
        if (!camera.camera_definition->is_setting_range(setting_id)) {
            get_option_str_with_lock(
                camera, setting_id, option.option_id, option.option_description);
        }
        options.push_back(option);
    }

    return !options.empty();
}

Camera::Result CameraImpl::set_setting(int32_t component_id, const Camera::Setting& setting)

{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    set_setting_async(
        component_id, setting, [&prom](Camera::Result result) { prom->set_value(result); });

    return ret.get();
}

void CameraImpl::set_setting_async(
    int32_t component_id, const Camera::Setting& setting, const Camera::ResultCallback& callback)
{
    set_option_async(component_id, setting.setting_id, setting.option, callback);
}

void CameraImpl::set_option_async(
    int32_t component_id,
    const std::string& setting_id,
    const Camera::Option& option,
    const Camera::ResultCallback& callback)
{
    std::lock_guard lock(_mutex);
    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
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
                [callback]() { callback(Camera::Result::Unavailable); });
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

        if (all_values.empty()) {
            if (callback) {
                LogErr() << "Could not get any options to get type for range param.";
                _system_impl->call_user_callback([callback]() { callback(Camera::Result::Error); });
            }
            return;
        }
        value = all_values[0];

        // Now re-use that type. This is quite ugly, but I don't know how we could do better than
        // that.
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
        [this, component_id, callback, setting_id, value](MavlinkParameterClient::Result result) {
            std::lock_guard lock_later(_mutex);
            auto maybe_potential_camera_later =
                maybe_potential_camera_for_component_id_with_lock(component_id, 0);
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
            refresh_params_with_lock(camera_later, false);
        },
        this,
        camera.component_id,
        true);
}

void CameraImpl::get_setting_async(
    int32_t component_id,
    const Camera::Setting& setting,
    const Camera::GetSettingCallback& callback)
{
    {
        std::lock_guard lock(_mutex);
        auto maybe_potential_camera =
            maybe_potential_camera_for_component_id_with_lock(component_id, 0);
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
                    [callback]() { callback(Camera::Result::Unavailable, {}); });
            }
            return;
        }
    }

    get_option_async(
        component_id,
        setting.setting_id,
        [this, callback](Camera::Result result, const Camera::Option& option) {
            Camera::Setting new_setting{};
            new_setting.option = option;
            if (callback) {
                _system_impl->call_user_callback(
                    [callback, result, new_setting]() { callback(result, new_setting); });
            }
        });
}

std::pair<Camera::Result, Camera::Setting>
CameraImpl::get_setting(int32_t component_id, const Camera::Setting& setting)
{
    auto prom = std::make_shared<std::promise<std::pair<Camera::Result, Camera::Setting>>>();
    auto ret = prom->get_future();

    get_setting_async(
        component_id, setting, [&prom](Camera::Result result, const Camera::Setting& new_setting) {
            prom->set_value(std::make_pair<>(result, new_setting));
        });

    return ret.get();
}

Camera::Result
CameraImpl::get_option(int32_t component_id, const std::string& setting_id, Camera::Option& option)
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    get_option_async(
        component_id,
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
    int32_t component_id,
    const std::string& setting_id,
    const std::function<void(Camera::Result, const Camera::Option&)>& callback)
{
    std::lock_guard lock(_mutex);
    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
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
                [callback]() { callback(Camera::Result::Unavailable, {}); });
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
    std::lock_guard lock(_mutex);
    auto handle = _subscribe_current_settings_callbacks.subscribe(callback);

    notify_current_settings_for_all_with_lock();
    return handle;
}

void CameraImpl::unsubscribe_current_settings(Camera::CurrentSettingsHandle handle)
{
    std::lock_guard lock(_mutex);
    _subscribe_current_settings_callbacks.unsubscribe(handle);
}

Camera::PossibleSettingOptionsHandle CameraImpl::subscribe_possible_setting_options(
    const Camera::PossibleSettingOptionsCallback& callback)
{
    std::lock_guard lock(_mutex);
    auto handle = _subscribe_possible_setting_options_callbacks.subscribe(callback);

    notify_possible_setting_options_for_all_with_lock();
    return handle;
}

void CameraImpl::unsubscribe_possible_setting_options(Camera::PossibleSettingOptionsHandle handle)
{
    _subscribe_possible_setting_options_callbacks.unsubscribe(handle);
}

void CameraImpl::notify_current_settings_for_all_with_lock()
{
    for (auto& potential_camera : potential_cameras_with_lock()) {
        if (potential_camera->camera_definition != nullptr) {
            notify_current_settings_with_lock(*potential_camera);
        }
    }
}

void CameraImpl::notify_possible_setting_options_for_all_with_lock()
{
    for (auto& potential_camera : potential_cameras_with_lock()) {
        if (potential_camera->camera_definition != nullptr) {
            notify_possible_setting_options_with_lock(*potential_camera);
        }
    }
}

void CameraImpl::notify_current_settings_with_lock(PotentialCamera& potential_camera)
{
    assert(potential_camera.camera_definition != nullptr);

    if (_subscribe_current_settings_callbacks.empty()) {
        return;
    }

    auto possible_setting_options = get_possible_setting_options_with_lock(potential_camera);
    if (possible_setting_options.first != Camera::Result::Success) {
        LogErr() << "Could not get possible settings in current options subscription.";
        return;
    }

    auto& camera = potential_camera;

    Camera::CurrentSettingsUpdate update{};
    update.component_id = potential_camera.component_id;

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
            update.current_settings.push_back(setting);
        }
    }

    _subscribe_current_settings_callbacks.queue(
        update, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void CameraImpl::notify_possible_setting_options_with_lock(PotentialCamera& potential_camera)
{
    assert(potential_camera.camera_definition != nullptr);

    if (_subscribe_possible_setting_options_callbacks.empty()) {
        return;
    }

    Camera::PossibleSettingOptionsUpdate update{};
    update.component_id = potential_camera.component_id;

    auto setting_options = get_possible_setting_options_with_lock(potential_camera);
    if (setting_options.second.empty()) {
        return;
    }

    update.setting_options = setting_options.second;

    _subscribe_possible_setting_options_callbacks.queue(
        update, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

std::pair<Camera::Result, std::vector<Camera::SettingOptions>>
CameraImpl::get_possible_setting_options(int32_t component_id)
{
    std::pair<Camera::Result, std::vector<Camera::SettingOptions>> result;

    std::lock_guard lock(_mutex);
    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
    if (maybe_potential_camera == nullptr) {
        result.first = Camera::Result::CameraIdInvalid;
        return result;
    }

    auto& camera = *maybe_potential_camera;
    if (camera.is_fetching_camera_definition || camera.camera_definition == nullptr) {
        result.first = Camera::Result::Unavailable;
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

    result.first = Camera::Result::Success;
    return result;
}

void CameraImpl::refresh_params_with_lock(PotentialCamera& potential_camera, bool initial_load)
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

    auto component_id = potential_camera.component_id;

    unsigned count = 0;
    for (const auto& param : params) {
        const std::string& param_name = param.first;
        const ParamValue& param_value_type = param.second;
        const bool is_last = (count == params.size() - 1);
        if (_debugging) {
            LogDebug() << "Trying to get param: " << param_name;
        }
        _system_impl->param_sender(potential_camera.component_id, true)
            ->get_param_async(
                param_name,
                param_value_type,
                [component_id, param_name, is_last, this](
                    MavlinkParameterClient::Result result, const ParamValue& value) {
                    if (result != MavlinkParameterClient::Result::Success) {
                        return;
                    }

                    std::lock_guard lock_later(_mutex);
                    auto maybe_potential_camera_later =
                        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
                    // We already checked these fields earlier, so we don't check again.
                    assert(maybe_potential_camera_later != nullptr);
                    assert(maybe_potential_camera_later->camera_definition != nullptr);

                    auto& camera_later = *maybe_potential_camera_later;

                    if (camera_later.camera_definition->set_setting(param_name, value)) {
                        if (_debugging) {
                            LogDebug() << "Got setting for " << param_name << ": " << value;
                        }
                        return;
                    }

                    if (is_last) {
                        notify_current_settings_with_lock(camera_later);
                        notify_possible_setting_options_with_lock(camera_later);
                    }
                },
                this);

        if (initial_load) {
            subscribe_to_param_changes_with_lock(potential_camera, param_name, param_value_type);
        }
        ++count;
    }
}

void CameraImpl::subscribe_to_param_changes_with_lock(
    PotentialCamera& camera, std::string param_name, const ParamValue& param_value_type)
{
    auto component_id = camera.component_id;
    auto changed = [this, component_id, param_name](auto new_param) {
        if (_debugging) {
            LogDebug() << "Got changing param: " << param_name << " -> " << new_param;
        }

        std::lock_guard lock_later(_mutex);

        auto maybe_potential_camera_later =
            maybe_potential_camera_for_component_id_with_lock(component_id, 0);
        // We already checked these fields earlier, so we don't check again.
        assert(maybe_potential_camera_later != nullptr);
        assert(maybe_potential_camera_later->camera_definition != nullptr);
        auto& camera_later = *maybe_potential_camera_later;

        ParamValue param_value;
        param_value.set<decltype(new_param)>(new_param);
        camera_later.camera_definition->set_setting(param_name, param_value);
    };

    if (param_value_type.is<uint8_t>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<uint8_t>(param_name, changed, this);
    } else if (param_value_type.is<uint8_t>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<uint16_t>(param_name, changed, this);
    } else if (param_value_type.is<uint32_t>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<uint32_t>(param_name, changed, this);
    } else if (param_value_type.is<uint64_t>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<uint64_t>(param_name, changed, this);
    } else if (param_value_type.is<int8_t>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<int8_t>(param_name, changed, this);
    } else if (param_value_type.is<int16_t>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<int16_t>(param_name, changed, this);
    } else if (param_value_type.is<int32_t>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<int32_t>(param_name, changed, this);
    } else if (param_value_type.is<int64_t>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<int64_t>(param_name, changed, this);
    } else if (param_value_type.is<float>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<float>(param_name, changed, this);
    } else if (param_value_type.is<double>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<double>(param_name, changed, this);
    } else if (param_value_type.is<std::string>()) {
        _system_impl->param_sender(camera.component_id, true)
            ->subscribe_param_changed<std::string>(param_name, changed, this);
    } else {
        LogErr() << "Unknown type for param " << param_name;
    }
}

bool CameraImpl::get_setting_str_with_lock(
    PotentialCamera& potential_camera, const std::string& setting_id, std::string& description)
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

void CameraImpl::request_camera_information(uint8_t component_id)
{
    _system_impl->mavlink_request_message().request(
        MAVLINK_MSG_ID_CAMERA_INFORMATION, fixup_component_target(component_id), nullptr);
}

Camera::Result CameraImpl::format_storage(int32_t component_id, int32_t storage_id)
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    format_storage_async(
        component_id, storage_id, [prom](Camera::Result result) { prom->set_value(result); });

    return ret.get();
}

void CameraImpl::format_storage_async(
    int32_t component_id, int32_t storage_id, const Camera::ResultCallback& callback)
{
    MavlinkCommandSender::CommandLong cmd_format{};

    cmd_format.command = MAV_CMD_STORAGE_FORMAT;
    cmd_format.params.maybe_param1 = static_cast<float>(storage_id); // storage ID
    cmd_format.params.maybe_param2 = 1.0f; // format
    cmd_format.params.maybe_param3 = 1.0f; // clear
    cmd_format.target_component_id = component_id;

    _system_impl->send_command_async(
        cmd_format, [this, callback](MavlinkCommandSender::Result result, float progress) {
            UNUSED(progress);

            receive_command_result(result, [this, callback](Camera::Result camera_result) {
                callback(camera_result);
            });
        });
}

Camera::Result CameraImpl::reset_settings(int32_t component_id)
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    reset_settings_async(component_id, [prom](Camera::Result result) { prom->set_value(result); });

    return ret.get();
}

void CameraImpl::reset_settings_async(int32_t component_id, const Camera::ResultCallback& callback)
{
    MavlinkCommandSender::CommandLong cmd_format{};

    cmd_format.command = MAV_CMD_RESET_CAMERA_SETTINGS;
    cmd_format.params.maybe_param1 = 1.0f; // reset
    cmd_format.target_component_id = component_id;

    _system_impl->send_command_async(
        cmd_format, [this, callback](MavlinkCommandSender::Result result, float progress) {
            UNUSED(progress);

            receive_command_result(result, [this, callback](Camera::Result camera_result) {
                callback(camera_result);
            });
        });
}

void CameraImpl::reset_following_format_storage_with_lock(PotentialCamera& camera)
{
    camera.capture_status.photo_list.clear();
    camera.capture_status.image_count = 0;
    camera.capture_status.image_count_at_connection = 0;
    camera.capture_info.last_advertised_image_index = -1;
    camera.capture_info.missing_image_retries.clear();
}

std::pair<Camera::Result, std::vector<Camera::CaptureInfo>>
CameraImpl::list_photos(int32_t component_id, Camera::PhotosRange photos_range)
{
    std::promise<std::pair<Camera::Result, std::vector<Camera::CaptureInfo>>> prom;
    auto ret = prom.get_future();

    list_photos_async(
        component_id,
        photos_range,
        [&prom](Camera::Result result, const std::vector<Camera::CaptureInfo>& photo_list) {
            prom.set_value(std::make_pair(result, photo_list));
        });

    return ret.get();
}

void CameraImpl::list_photos_async(
    int32_t component_id,
    Camera::PhotosRange photos_range,
    const Camera::ListPhotosCallback& callback)
{
    if (!callback) {
        LogWarn() << "Trying to get a photo list with a null callback, ignoring...";
        return;
    }

    std::lock_guard lock(_mutex);

    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
    if (maybe_potential_camera == nullptr) {
        LogWarn() << "Invalid camera ID: " << component_id;
        return;
    }
    auto& camera = *maybe_potential_camera;

    if (camera.capture_status.image_count == -1) {
        LogErr() << "Cannot list photos: camera status has not been received yet!";
        _system_impl->call_user_callback(
            [callback]() { callback(Camera::Result::Error, std::vector<Camera::CaptureInfo>{}); });
        return;
    }

    // By default we just try to keep track of photos captured while we were connected.
    // If the API user asks for all, we will start requesting all previous ones.

    if (camera.capture_status.photos_range == Camera::PhotosRange::SinceConnection &&
        photos_range == Camera::PhotosRange::All) {
        camera.capture_status.photos_range = photos_range;

        auto oldest_photo_it = std::min_element(
            camera.capture_status.photo_list.begin(),
            camera.capture_status.photo_list.end(),
            [](auto& a, auto& b) { return a.first < b.first; });

        if (oldest_photo_it != camera.capture_status.photo_list.end()) {
            for (int i = 0; i < oldest_photo_it->first; ++i) {
                if (camera.capture_info.missing_image_retries.find(i) ==
                    camera.capture_info.missing_image_retries.end()) {
                    camera.capture_info.missing_image_retries[i] = 0;
                }
            }
        }
    }

    const int start_index = [&, this]() {
        switch (photos_range) {
            case Camera::PhotosRange::SinceConnection:
                return camera.capture_status.image_count_at_connection;
            case Camera::PhotosRange::All:
            // FALLTHROUGH
            default:
                return 0;
        }
    }();

    std::vector<Camera::CaptureInfo> photo_list;

    for (const auto& capture_info : camera.capture_status.photo_list) {
        if (capture_info.first >= start_index) {
            photo_list.push_back(capture_info.second);
        }
    }

    _system_impl->call_user_callback(
        [callback, photo_list]() { callback(Camera::Result::Success, photo_list); });
}

std::pair<Camera::Result, Camera::Mode> CameraImpl::get_mode(int32_t component_id)
{
    std::pair<Camera::Result, Camera::Mode> result{};

    std::lock_guard lock(_mutex);

    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
    if (maybe_potential_camera == nullptr) {
        result.first = Camera::Result::CameraIdInvalid;
    } else {
        result.first = Camera::Result::Success;
        result.second = maybe_potential_camera->mode;
    }

    return result;
}

std::pair<Camera::Result, Camera::Storage> CameraImpl::get_storage(int32_t component_id)
{
    std::pair<Camera::Result, Camera::Storage> result{};

    std::lock_guard lock(_mutex);

    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
    if (maybe_potential_camera == nullptr) {
        result.first = Camera::Result::CameraIdInvalid;
    } else {
        if (maybe_potential_camera->received_storage) {
            result.first = Camera::Result::Success;
            result.second = maybe_potential_camera->storage;
        } else {
            result.first = Camera::Result::Unavailable;
        }
    }

    return result;
}

std::pair<Camera::Result, Camera::VideoStreamInfo>
CameraImpl::get_video_stream_info(int32_t component_id)
{
    std::pair<Camera::Result, Camera::VideoStreamInfo> result{};

    std::lock_guard lock(_mutex);

    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
    if (maybe_potential_camera == nullptr) {
        result.first = Camera::Result::CameraIdInvalid;
    } else {
        result.first = Camera::Result::Success;
        result.second = maybe_potential_camera->video_stream_info;
    }

    return result;
}

std::pair<Camera::Result, std::vector<Camera::Setting>>
CameraImpl::get_current_settings(int32_t component_id)
{
    std::pair<Camera::Result, std::vector<Camera::Setting>> result;

    std::lock_guard lock(_mutex);
    auto maybe_potential_camera =
        maybe_potential_camera_for_component_id_with_lock(component_id, 0);
    if (maybe_potential_camera == nullptr) {
        result.first = Camera::Result::CameraIdInvalid;
        return result;
    }

    auto& camera = *maybe_potential_camera;
    if (camera.is_fetching_camera_definition || camera.camera_definition == nullptr) {
        result.first = Camera::Result::Unavailable;
        return result;
    }

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
            result.second.push_back(setting);
        }
    }

    result.first = Camera::Result::Success;
    return result;
}

uint16_t CameraImpl::get_and_increment_capture_sequence(int32_t component_id)
{
    if (component_id == 0) {
        // All cameras
        return 0;
    }

    std::lock_guard lock(_mutex);

    for (auto& potential_camera : _potential_cameras) {
        if (potential_camera.component_id == component_id) {
            return potential_camera.capture_sequence++;
        }
    }

    return 0;
}

std::vector<CameraImpl::PotentialCamera*> CameraImpl::potential_cameras_with_lock()
{
    std::vector<CameraImpl::PotentialCamera*> result;

    for (auto& potential_camera : _potential_cameras) {
        if (!potential_camera.maybe_information) {
            continue;
        }
        result.push_back(&potential_camera);
    }

    return result;
}

CameraImpl::PotentialCamera* CameraImpl::maybe_potential_camera_for_component_id_with_lock(
    uint8_t component_id, uint8_t camera_device_id)
{
    // Component Ids 1-6 means the camera is connected to the autopilot and the
    // ID is set by the camera_device_id instead.
    if (component_id == 1 && camera_device_id != 0) {
        component_id = camera_device_id;
    }

    const auto it = std::find_if(
        _potential_cameras.begin(), _potential_cameras.end(), [&](auto& potential_camera) {
            return potential_camera.component_id == component_id;
        });

    if (it == _potential_cameras.end()) {
        return nullptr;
    }

    // How to get pointer from iterator?
    return &(*it);
}

uint8_t CameraImpl::fixup_component_target(uint8_t component_id)
{
    // Component Ids 1-6 means the camera is connected to the autopilot.
    if (component_id >= 1 && component_id <= 6) {
        return 1;
    }

    return component_id;
}

} // namespace mavsdk
