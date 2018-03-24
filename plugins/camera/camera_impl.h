#pragma once

#include "device.h"
#include "camera.h"
#include "plugin_impl_base.h"

namespace dronecore {

class CameraImpl : public PluginImplBase
{
public:
    CameraImpl(Device &device);
    ~CameraImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Camera::Result take_photo();

    Camera::Result start_photo_interval(float interval_s);
    Camera::Result stop_photo_interval();

    Camera::Result start_video();
    Camera::Result stop_video();

    void take_photo_async(const Camera::result_callback_t &callback);

    void start_photo_interval_async(float interval_s, const Camera::result_callback_t &callback);
    void stop_photo_interval_async(const Camera::result_callback_t &callback);

    void start_video_async(const Camera::result_callback_t &callback);
    void stop_video_async(const Camera::result_callback_t &callback);

    void set_mode_async(Camera::Mode mode, const Camera::mode_callback_t &callback);
    void get_mode_async(Camera::mode_callback_t callback);

    void capture_info_async(Camera::capture_info_callback_t callback);

    void get_status_async(Camera::get_status_callback_t callback);

    // Non-copyable
    CameraImpl(const CameraImpl &) = delete;
    const CameraImpl &operator=(const CameraImpl &) = delete;

private:
    struct {
        std::mutex mutex {};
        Camera::get_status_callback_t callback {nullptr};
        Camera::Status data {};
        bool received_camera_capture_status {false};
        bool received_storage_information {false};
    } _last_status;

    struct {
        std::mutex mutex {};
        Camera::mode_callback_t callback {nullptr};
    } _get_mode;

    void receive_set_mode_command_result(MavlinkCommands::Result command_result,
                                         const Camera::mode_callback_t &callback,
                                         Camera::Mode mode);

    void receive_get_mode_timeout();

    static Camera::Result camera_result_from_command_result(
        MavlinkCommands::Result command_result);

    static void receive_command_result(MavlinkCommands::Result command_result,
                                       const Camera::result_callback_t &callback);

    static bool interval_valid(float interval_s);

    void process_camera_image_captured(const mavlink_message_t &message);
    void process_storage_information(const mavlink_message_t &message);
    void process_camera_capture_status(const mavlink_message_t &message);
    void process_camera_settings(const mavlink_message_t &message);
    void receive_storage_information_result(MavlinkCommands::Result result);

    void receive_camera_capture_status_result(MavlinkCommands::Result result);

    void check_last_status();

    void status_timeout_happened();

    static constexpr double DEFAULT_TIMEOUT_S = 3.0;

    int _capture_sequence = 0;

    Camera::capture_info_callback_t _capture_info_callback {nullptr};

    void *_timeout_cookie = nullptr;
};


} // namespace dronecore
