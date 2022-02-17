#pragma once

#include "plugins/camera_server/camera_server.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class CameraServerImpl : public PluginImplBase {
public:
    explicit CameraServerImpl(System& system);
    explicit CameraServerImpl(std::shared_ptr<System> system);
    ~CameraServerImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    CameraServer::Result set_information(CameraServer::Information information);
    CameraServer::Result set_in_progress(bool in_progress);

    void subscribe_take_photo(CameraServer::TakePhotoCallback callback);

    CameraServer::Result respond_take_photo(CameraServer::CaptureInfo capture_info);

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

    bool _is_information_set{};
    CameraServer::Information _information{};

    // CAMERA_CAPTURE_STATUS fields
    // TODO: how do we keep this info in sync between plugin instances?
    bool _is_image_capture_in_progress{};
    bool _is_image_capture_interval_set{};
    float _image_capture_timer_interval_s{};
    void* _image_capture_timer_cookie{};
    int32_t _image_capture_count{};

    CameraServer::TakePhotoCallback _take_photo_callback{};

    bool parse_version_string(const std::string& version_str);
    bool parse_version_string(const std::string& version_str, uint32_t& version);
    void start_image_capture_interval(float interval, int32_t count, int32_t index);
    void stop_image_capture_interval();

    std::optional<mavlink_message_t>
    process_camera_information_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_camera_settings_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_storage_information_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_storage_format(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_camera_capture_status_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_reset_camera_settings(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_set_camera_mode(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_set_camera_zoom(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_set_camera_focus(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_set_storage_usage(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_image_start_capture(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_image_stop_capture(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_camera_image_capture_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_video_start_capture(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_video_stop_capture(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_video_start_streaming(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_video_stop_streaming(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_video_stream_information_request(const MavlinkCommandReceiver::CommandLong& command);
    std::optional<mavlink_message_t>
    process_video_stream_status_request(const MavlinkCommandReceiver::CommandLong& command);
};

} // namespace mavsdk