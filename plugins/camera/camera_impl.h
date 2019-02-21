#pragma once

#include "system.h"
#include "plugins/camera/camera.h"
#include "plugin_impl_base.h"
#include "camera_definition.h"
#include "mavlink_include.h"

namespace dronecode_sdk {

class CameraImpl : public PluginImplBase {
public:
    CameraImpl(System &system);
    ~CameraImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Camera::Result select_camera(unsigned id);

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

    Camera::Information get_information();

    void set_video_stream_settings(const Camera::VideoStreamSettings &settings);
    Camera::Result get_video_stream_info(Camera::VideoStreamInfo &info);
    void get_video_stream_info_async(const Camera::get_video_stream_info_callback_t callback);
    void subscribe_video_stream_info(const Camera::subscribe_video_stream_info_callback_t callback);

    Camera::Result start_video_streaming();
    Camera::Result stop_video_streaming();

    Camera::Result set_mode(const Camera::Mode mode);
    void set_mode_async(const Camera::Mode mode, const Camera::mode_callback_t &callback);
    void get_mode_async(Camera::mode_callback_t callback);
    void subscribe_mode(const Camera::subscribe_mode_callback_t callback);

    void subscribe_capture_info(Camera::capture_info_callback_t callback);

    void get_status_async(Camera::get_status_callback_t callback);
    void subscribe_status(const Camera::subscribe_status_callback_t callback);

    void set_option_async(const std::string &setting_id,
                          const Camera::Option &option,
                          const Camera::result_callback_t &callback);

    Camera::Result get_option(const std::string &setting_id, Camera::Option &option);
    void get_option_async(const std::string &setting_id,
                          const Camera::get_option_callback_t &callback);

    bool get_possible_setting_options(std::vector<std::string> &settings);
    bool get_possible_options(const std::string &setting_id, std::vector<Camera::Option> &options);

    bool get_setting_str(const std::string &setting_id, std::string &description);
    bool get_option_str(const std::string &setting_id,
                        const std::string &option_id,
                        std::string &description);

    void subscribe_current_settings(const Camera::subscribe_current_settings_callback_t &callback);
    void subscribe_possible_setting_options(
        const Camera::subscribe_possible_setting_options_callback_t &callback);

    Camera::Result format_storage();
    void format_storage_async(Camera::result_callback_t callback);

    CameraImpl(const CameraImpl &) = delete;
    CameraImpl &operator=(const CameraImpl &) = delete;

private:
    struct {
        std::mutex mutex{};
        Camera::get_status_callback_t callback{nullptr};
        Camera::Status data{};
        bool received_camera_capture_status{false};
        bool received_storage_information{false};
        void *timeout_cookie{nullptr};
        void *call_every_cookie{nullptr};
    } _status{};

    static constexpr double DEFAULT_TIMEOUT_S = 3.0;

    struct {
        std::mutex mutex{};
        Camera::mode_callback_t callback{nullptr};
        void *timeout_cookie{nullptr};
    } _get_mode{};

    struct {
        std::mutex mutex{};
        int sequence = 1; // The MAVLink spec says the sequence starts at 1.
    } _capture{};

    struct {
        std::mutex mutex{};
        Camera::capture_info_callback_t callback{nullptr};
    } _capture_info{};

    struct {
        std::mutex mutex{};
        Camera::VideoStreamInfo info{};
        bool available{false};
        Camera::get_video_stream_info_callback_t callback{nullptr};
        void *timeout_cookie{nullptr};
        void *call_every_cookie{nullptr};
    } _video_stream_info{};

    struct {
        std::mutex mutex{};
        Camera::Information data{};
    } _information{};

    void *_flight_information_call_every_cookie{nullptr};

    void receive_set_mode_command_result(const MAVLinkCommands::Result command_result,
                                         const Camera::mode_callback_t &callback,
                                         const Camera::Mode mode);

    void get_mode_timeout_happened();

    void receive_get_mode_command_result(MAVLinkCommands::Result command_result);

    static Camera::Result
    camera_result_from_command_result(const MAVLinkCommands::Result command_result);

    static void receive_command_result(MAVLinkCommands::Result command_result,
                                       const Camera::result_callback_t &callback);

    static bool interval_valid(float interval_s);

    void process_camera_image_captured(const mavlink_message_t &message);
    void process_storage_information(const mavlink_message_t &message);
    void process_camera_capture_status(const mavlink_message_t &message);
    void process_camera_settings(const mavlink_message_t &message);
    void process_camera_information(const mavlink_message_t &message);
    void process_video_information(const mavlink_message_t &message);
    void process_flight_information(const mavlink_message_t &message);

    void receive_storage_information_result(MAVLinkCommands::Result result);
    void receive_camera_capture_status_result(MAVLinkCommands::Result result);

    Camera::CaptureInfo::EulerAngle
    to_euler_angle_from_quaternion(Camera::CaptureInfo::Quaternion quaternion);

    void notify_mode(const Camera::Mode mode);
    void notify_video_stream_info();
    void notify_capture_info(Camera::CaptureInfo capture_info);
    void notify_status(Camera::Status status);
    void notify_current_settings();
    void notify_possible_setting_options();

    void check_status();

    void status_timeout_happened();
    void get_video_stream_info_timeout();

    bool load_definition_file(const std::string &uri, std::string &content);

    void refresh_params();
    void invalidate_params();

    void request_flight_information();

    void save_camera_mode(const float mavlink_camera_mode);
    float to_mavlink_camera_mode(const Camera::Mode mode) const;
    Camera::Mode to_camera_mode(const uint8_t mavlink_camera_mode) const;

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

    std::unique_ptr<CameraDefinition> _camera_definition{};

    Camera::subscribe_mode_callback_t _subscribe_mode_callback{nullptr};
    Camera::subscribe_video_stream_info_callback_t _subscribe_video_stream_info_callback{nullptr};
    Camera::subscribe_status_callback_t _subscribe_status_callback{nullptr};
    Camera::subscribe_current_settings_callback_t _subscribe_current_settings_callback{nullptr};
    Camera::subscribe_possible_setting_options_callback_t
        _subscribe_possible_setting_options_callback{nullptr};

    std::atomic<unsigned> _component_id{MAV_COMP_ID_CAMERA};
};

} // namespace dronecode_sdk
