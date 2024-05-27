#pragma once

#include "plugins/camera_server/camera_server.h"
#include "server_plugin_impl_base.h"
#include "callback_list.h"
#include <thread>

namespace mavsdk {

class CameraServerImpl : public ServerPluginImplBase {
public:
    explicit CameraServerImpl(std::shared_ptr<ServerComponent> server_component);
    ~CameraServerImpl() override;

    void init() override;
    void deinit() override;

    void set_tracking_point_status(CameraServer::TrackPoint tracked_point);

    void set_tracking_rectangle_status(CameraServer::TrackRectangle tracked_rectangle);

    void set_tracking_off_status();

    CameraServer::Result set_information(CameraServer::Information information);
    CameraServer::Result set_video_streaming(CameraServer::VideoStreaming video_streaming);
    CameraServer::Result set_in_progress(bool in_progress);

    CameraServer::TakePhotoHandle
    subscribe_take_photo(const CameraServer::TakePhotoCallback& callback);
    void unsubscribe_take_photo(CameraServer::TakePhotoHandle handle);
    CameraServer::Result respond_take_photo(
        CameraServer::CameraFeedback take_photo_feedback, CameraServer::CaptureInfo capture_info);

    CameraServer::StartVideoHandle
    subscribe_start_video(const CameraServer::StartVideoCallback& callback);
    void unsubscribe_start_video(CameraServer::StartVideoHandle handle);
    CameraServer::Result respond_start_video(CameraServer::CameraFeedback start_video_feedback);

    CameraServer::StopVideoHandle
    subscribe_stop_video(const CameraServer::StopVideoCallback& callback);
    void unsubscribe_stop_video(CameraServer::StopVideoHandle handle);
    CameraServer::Result respond_stop_video(CameraServer::CameraFeedback stop_video_feedback);

    CameraServer::StartVideoStreamingHandle
    subscribe_start_video_streaming(const CameraServer::StartVideoStreamingCallback& callback);
    void unsubscribe_start_video_streaming(CameraServer::StartVideoStreamingHandle handle);
    CameraServer::Result
    respond_start_video_streaming(CameraServer::CameraFeedback start_video_streaming_feedback);

    CameraServer::StopVideoStreamingHandle
    subscribe_stop_video_streaming(const CameraServer::StopVideoStreamingCallback& callback);
    void unsubscribe_stop_video_streaming(CameraServer::StopVideoStreamingHandle handle);
    CameraServer::Result
    respond_stop_video_streaming(CameraServer::CameraFeedback stop_video_streaming_feedback);

    CameraServer::SetModeHandle subscribe_set_mode(const CameraServer::SetModeCallback& callback);
    void unsubscribe_set_mode(CameraServer::SetModeHandle handle);
    CameraServer::Result respond_set_mode(CameraServer::CameraFeedback set_mode_feedback);

    CameraServer::StorageInformationHandle
    subscribe_storage_information(const CameraServer::StorageInformationCallback& callback);
    void unsubscribe_storage_information(CameraServer::StorageInformationHandle handle);
    CameraServer::Result respond_storage_information(
        CameraServer::CameraFeedback storage_information_feedback,
        CameraServer::StorageInformation storage_information);

    CameraServer::CaptureStatusHandle
    subscribe_capture_status(const CameraServer::CaptureStatusCallback& callback);
    void unsubscribe_capture_status(CameraServer::CaptureStatusHandle handle);
    CameraServer::Result respond_capture_status(
        CameraServer::CameraFeedback capture_status_feedback,
        CameraServer::CaptureStatus capture_status);

    CameraServer::FormatStorageHandle
    subscribe_format_storage(const CameraServer::FormatStorageCallback& callback);
    void unsubscribe_format_storage(CameraServer::FormatStorageHandle handle);
    CameraServer::Result
    respond_format_storage(CameraServer::CameraFeedback format_storage_feedback);

    CameraServer::ResetSettingsHandle
    subscribe_reset_settings(const CameraServer::ResetSettingsCallback& callback);
    void unsubscribe_reset_settings(CameraServer::ResetSettingsHandle handle);
    CameraServer::Result
    respond_reset_settings(CameraServer::CameraFeedback reset_settings_feedback);

    CameraServer::ZoomInStartHandle
    subscribe_zoom_in_start(const CameraServer::ZoomInStartCallback& callback);
    void unsubscribe_zoom_in_start(CameraServer::ZoomInStartHandle handle);
    CameraServer::Result respond_zoom_in_start(CameraServer::CameraFeedback zoom_in_start_feedback);
    CameraServer::ZoomOutStartHandle
    subscribe_zoom_out_start(const CameraServer::ZoomOutStartCallback& callback);
    void unsubscribe_zoom_out_start(CameraServer::ZoomOutStartHandle handle);
    CameraServer::Result
    respond_zoom_out_start(CameraServer::CameraFeedback zoom_out_start_feedback);
    CameraServer::ZoomStopHandle
    subscribe_zoom_stop(const CameraServer::ZoomStopCallback& callback);
    void unsubscribe_zoom_stop(CameraServer::ZoomStopHandle handle);
    CameraServer::Result respond_zoom_stop(CameraServer::CameraFeedback zoom_stop_feedback);
    CameraServer::ZoomRangeHandle
    subscribe_zoom_range(const CameraServer::ZoomRangeCallback& callback);
    void unsubscribe_zoom_range(CameraServer::ZoomRangeHandle handle);
    CameraServer::Result respond_zoom_range(CameraServer::CameraFeedback zoom_range_feedback);
    CameraServer::TrackingPointCommandHandle
    subscribe_tracking_point_command(const CameraServer::TrackingPointCommandCallback& callback);
    void unsubscribe_tracking_point_command(CameraServer::TrackingPointCommandHandle handle);
    CameraServer::Result
    respond_tracking_point_command(CameraServer::CameraFeedback tracking_point_feedback);

    CameraServer::TrackingRectangleCommandHandle subscribe_tracking_rectangle_command(
        const CameraServer::TrackingRectangleCommandCallback& callback);
    void
    unsubscribe_tracking_rectangle_command(CameraServer::TrackingRectangleCommandHandle handle);
    CameraServer::Result
    respond_tracking_rectangle_command(CameraServer::CameraFeedback tracking_rectangle_feedback);

    CameraServer::TrackingOffCommandHandle
    subscribe_tracking_off_command(const CameraServer::TrackingOffCommandCallback& callback);
    void unsubscribe_tracking_off_command(CameraServer::TrackingOffCommandHandle handle);
    CameraServer::Result
    respond_tracking_off_command(CameraServer::CameraFeedback tracking_off_feedback);

private:
    enum StatusFlags {
        IN_PROGRESS = 1 << 0,
        INTERVAL_SET = 1 << 1,
    };

    enum class TriggerControl {
        IGNORE = -1,
        DISABLE = 0,
        ENABLE = 1,
    };

    enum class TrackingMode {
        NONE = 0,
        POINT = 1,
        RECTANGLE = 2,
    };

    bool parse_version_string(const std::string& version_str);
    bool parse_version_string(const std::string& version_str, uint32_t& version);
    bool is_command_sender_ok(const MavlinkCommandReceiver::CommandLong& command);
    void start_image_capture_interval(float interval, int32_t count, int32_t index);
    void stop_image_capture_interval();
    void start_sending_tracking_status(uint32_t interval_us);
    void stop_sending_tracking_status();
    void send_tracking_status_with_interval(uint32_t interval_us);

    std::optional<mavlink_command_ack_t>
    process_camera_information_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_camera_settings_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_storage_information_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_storage_format(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_camera_capture_status_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_reset_camera_settings(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_set_camera_mode(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_set_camera_zoom(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_set_camera_focus(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_set_storage_usage(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_image_start_capture(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_image_stop_capture(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_camera_image_capture_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_video_start_capture(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_video_stop_capture(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_video_start_streaming(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_video_stop_streaming(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_video_stream_information_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_video_stream_status_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_track_point_command(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_track_rectangle_command(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_track_off_command(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_command_ack_t>
    process_set_message_interval(const MavlinkCommandReceiver::CommandLong& command);

    void send_capture_status();

    bool _is_information_set{};

    // CAMERA_TRACKING_STATUS messages sending fields
    std::mutex _tracking_status_mutex{};
    bool _sending_tracking_status{};
    TrackingMode _tracking_mode{};
    CameraServer::TrackPoint _tracked_point{};
    CameraServer::TrackRectangle _tracked_rectangle{};
    std::thread _tracking_status_sending_thread{};

    CameraServer::Information _information{};
    bool _is_video_streaming_set{};
    CameraServer::VideoStreaming _video_streaming{};

    CameraServer::CaptureStatus _capture_status{};

    // CAMERA_CAPTURE_STATUS fields
    // TODO: how do we keep this info in sync between plugin instances?
    bool _is_image_capture_in_progress{};
    bool _is_image_capture_interval_set{};
    float _image_capture_timer_interval_s{};
    CallEveryHandler::Cookie _image_capture_timer_cookie{};
    int32_t _image_capture_count{};

    CallbackList<int32_t> _take_photo_callbacks{};
    CallbackList<int32_t> _start_video_callbacks{};
    CallbackList<int32_t> _stop_video_callbacks{};
    CallbackList<int32_t> _start_video_streaming_callbacks{};
    CallbackList<int32_t> _stop_video_streaming_callbacks{};
    CallbackList<CameraServer::Mode> _set_mode_callbacks{};
    CallbackList<int32_t> _storage_information_callbacks{};
    CallbackList<int32_t> _capture_status_callbacks{};
    CallbackList<int32_t> _format_storage_callbacks{};
    CallbackList<int32_t> _reset_settings_callbacks{};
    CallbackList<CameraServer::TrackPoint> _tracking_point_callbacks{};
    CallbackList<CameraServer::TrackRectangle> _tracking_rectangle_callbacks{};
    CallbackList<int32_t> _tracking_off_callbacks{};

    MavlinkCommandReceiver::CommandLong _last_take_photo_command;
    MavlinkCommandReceiver::CommandLong _last_start_video_command;
    MavlinkCommandReceiver::CommandLong _last_stop_video_command;
    MavlinkCommandReceiver::CommandLong _last_start_video_streaming_command;
    MavlinkCommandReceiver::CommandLong _last_stop_video_streaming_command;
    MavlinkCommandReceiver::CommandLong _last_set_mode_command;
    MavlinkCommandReceiver::CommandLong _last_storage_information_command;
    MavlinkCommandReceiver::CommandLong _last_capture_status_command;
    MavlinkCommandReceiver::CommandLong _last_format_storage_command;
    MavlinkCommandReceiver::CommandLong _last_reset_settings_command;
    MavlinkCommandReceiver::CommandLong _last_track_point_command;
    MavlinkCommandReceiver::CommandLong _last_track_rectangle_command;
    MavlinkCommandReceiver::CommandLong _last_tracking_off_command;

    uint8_t _last_storage_id;

    CallbackList<int32_t> _zoom_in_start_callbacks{};
    CallbackList<int32_t> _zoom_out_start_callbacks{};
    CallbackList<int32_t> _zoom_stop_callbacks{};
    CallbackList<float> _zoom_range_callbacks{};

    MavlinkCommandReceiver::CommandLong _last_zoom_in_start_command;
    MavlinkCommandReceiver::CommandLong _last_zoom_out_start_command;
    MavlinkCommandReceiver::CommandLong _last_zoom_stop_command;
    MavlinkCommandReceiver::CommandLong _last_zoom_range_command;
};

} // namespace mavsdk
