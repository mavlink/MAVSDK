#pragma once

#include "system.h"
#include "camera.h"
#include "plugin_impl_base.h"
#include "camera_definition.h"
#include "mavlink_system.h"

namespace dronecore {

class CameraImpl : public PluginImplBase
{
public:
    CameraImpl(System &system);
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

    void set_video_stream_settings(const Camera::VideoStreamSettings &settings);
    Camera::Result get_video_stream_info(Camera::VideoStreamInfo &info);

    Camera::Result start_video_streaming();
    Camera::Result stop_video_streaming();

    void set_mode_async(Camera::Mode mode, const Camera::mode_callback_t &callback);
    void get_mode_async(Camera::mode_callback_t callback);

    void capture_info_async(Camera::capture_info_callback_t callback);

    void get_status_async(Camera::get_status_callback_t callback);

    void set_option_async(const std::string &setting,
                          const std::string &option,
                          const Camera::result_callback_t &callback);

    void get_option_async(const std::string &setting,
                          const Camera::get_option_callback_t &callback);

    bool get_possible_settings(std::vector<std::string> &settings);
    bool get_possible_options(const std::string &setting_name, std::vector<std::string> &options);

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
        void *timeout_cookie {nullptr};
    } _status;

    static constexpr double DEFAULT_TIMEOUT_S = 3.0;

    struct {
        std::mutex mutex {};
        Camera::mode_callback_t callback {nullptr};
        void *timeout_cookie {nullptr};
    } _get_mode;

    struct {
        std::mutex mutex {};
        int sequence = 1; // The MAVLink spec says the sequence starts at 1.
    } _capture;

    struct {
        std::mutex mutex {};
        Camera::capture_info_callback_t callback {nullptr};
    } _capture_info;

    struct {
        std::mutex mutex {};
        Camera::VideoStreamInfo info;
        bool available = false;
        void reset()
        {
            info = Camera::VideoStreamInfo();
            available = false;
        }
    } _video_stream_info;

    void receive_set_mode_command_result(MAVLinkCommands::Result command_result,
                                         const Camera::mode_callback_t &callback,
                                         Camera::Mode mode);

    void get_mode_timeout_happened();

    void receive_get_mode_command_result(MAVLinkCommands::Result command_result);

    static Camera::Result camera_result_from_command_result(
        MAVLinkCommands::Result command_result);

    static void receive_command_result(MAVLinkCommands::Result command_result,
                                       const Camera::result_callback_t &callback);

    static bool interval_valid(float interval_s);

    void process_camera_image_captured(const mavlink_message_t &message);
    void process_storage_information(const mavlink_message_t &message);
    void process_camera_capture_status(const mavlink_message_t &message);
    void process_camera_settings(const mavlink_message_t &message);
    void process_camera_information(const mavlink_message_t &message);
    void process_video_information(const mavlink_message_t &message);

    void receive_storage_information_result(MAVLinkCommands::Result result);
    void receive_camera_capture_status_result(MAVLinkCommands::Result result);

    void check_status();

    void status_timeout_happened();

    void load_definition_file(const std::string &uri);
    void receive_param(const std::string &name, bool success, MAVLinkParameters::ParamValue value);

    // Utility methods for convenience
    MAVLinkCommands::CommandLong make_command_take_photo(float interval_s, float no_of_photos);
    MAVLinkCommands::CommandLong make_command_stop_photo();

    MAVLinkCommands::CommandLong make_command_request_camera_info();
    MAVLinkCommands::CommandLong make_command_set_camera_mode(float mavlink_mode);
    MAVLinkCommands::CommandLong make_command_request_camera_settings();
    MAVLinkCommands::CommandLong make_command_request_camera_capture_status();
    MAVLinkCommands::CommandLong make_command_request_storage_info();

    MAVLinkCommands::CommandLong make_command_start_video(float capture_status_rate_hz);
    MAVLinkCommands::CommandLong make_command_stop_video();

    MAVLinkCommands::CommandLong make_command_start_video_streaming();
    MAVLinkCommands::CommandLong make_command_stop_video_streaming();

    mavlink_message_t
    make_message_set_video_stream_settings(const Camera::VideoStreamSettings &settings);

    MAVLinkCommands::CommandLong make_command_request_video_stream_info();


    std::unique_ptr<CameraDefinition> _camera_definition {};
};


} // namespace dronecore
