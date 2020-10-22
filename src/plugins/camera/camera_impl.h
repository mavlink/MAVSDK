#pragma once

#include "camera_definition.h"
#include "mavlink_include.h"
#include "plugins/camera/camera.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class CameraImpl : public PluginImplBase {
public:
    explicit CameraImpl(System& system);
    explicit CameraImpl(std::shared_ptr<System> system);
    ~CameraImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Camera::Result select_camera(size_t id);

    Camera::Result take_photo();

    Camera::Result start_photo_interval(float interval_s);
    Camera::Result stop_photo_interval();

    Camera::Result start_video();
    Camera::Result stop_video();

    void take_photo_async(const Camera::ResultCallback& callback);
    void start_photo_interval_async(float interval_s, const Camera::ResultCallback& callback);
    void stop_photo_interval_async(const Camera::ResultCallback& callback);
    void start_video_async(const Camera::ResultCallback& callback);
    void stop_video_async(const Camera::ResultCallback& callback);

    Camera::Information information() const;
    void information_async(const Camera::InformationCallback& callback);

    std::pair<Camera::Result, Camera::VideoStreamInfo> get_video_stream_info();

    Camera::VideoStreamInfo video_stream_info();
    void video_stream_info_async(Camera::VideoStreamInfoCallback callback);

    Camera::Result start_video_streaming();
    Camera::Result stop_video_streaming();

    Camera::Result set_mode(const Camera::Mode mode);
    void set_mode_async(const Camera::Mode mode, const Camera::ResultCallback& callback);

    Camera::Mode mode();
    void mode_async(const Camera::ModeCallback callback);

    void capture_info_async(Camera::CaptureInfoCallback callback);

    Camera::Status status();
    void status_async(const Camera::StatusCallback callback);

    Camera::Result set_setting(Camera::Setting setting);
    void set_setting_async(Camera::Setting setting, const Camera::ResultCallback callback);

    void get_setting_async(Camera::Setting setting, const Camera::GetSettingCallback callback);
    std::pair<Camera::Result, Camera::Setting> get_setting(Camera::Setting setting);

    std::vector<Camera::SettingOptions> possible_setting_options();

    bool is_setting_range(const std::string& setting_id);

    void current_settings_async(const Camera::CurrentSettingsCallback& callback);
    void possible_setting_options_async(const Camera::PossibleSettingOptionsCallback& callback);

    Camera::Result format_storage();
    void format_storage_async(Camera::ResultCallback callback);

    CameraImpl(const CameraImpl&) = delete;
    CameraImpl& operator=(const CameraImpl&) = delete;

private:
    bool get_possible_setting_options(std::vector<std::string>& settings);
    bool get_possible_options(const std::string& setting_id, std::vector<Camera::Option>& options);

    void set_option_async(
        const std::string& setting_id,
        const Camera::Option& option,
        const Camera::ResultCallback& callback);

    Camera::Result get_option(const std::string& setting_id, Camera::Option& option);
    void get_option_async(
        const std::string& setting_id,
        const std::function<void(Camera::Result, const Camera::Option&)>& callback);

    bool get_setting_str(const std::string& setting_id, std::string& description);
    bool get_option_str(
        const std::string& setting_id, const std::string& option_id, std::string& description);

    void check_connection_status();
    void manual_enable();
    void manual_disable();

    void receive_set_mode_command_result(
        const MavlinkCommandSender::Result command_result,
        const Camera::ResultCallback callback,
        const Camera::Mode mode);

    static Camera::Result
    camera_result_from_command_result(const MavlinkCommandSender::Result command_result);

    void receive_command_result(
        MavlinkCommandSender::Result command_result, const Camera::ResultCallback& callback);

    static bool interval_valid(float interval_s);

    void process_camera_image_captured(const mavlink_message_t& message);
    void process_storage_information(const mavlink_message_t& message);
    void process_camera_capture_status(const mavlink_message_t& message);
    void process_camera_settings(const mavlink_message_t& message);
    void process_camera_information(const mavlink_message_t& message);
    void process_video_information(const mavlink_message_t& message);
    void process_flight_information(const mavlink_message_t& message);

    Camera::EulerAngle to_euler_angle_from_quaternion(Camera::Quaternion quaternion);

    void notify_mode();
    void notify_video_stream_info();
    void notify_current_settings();
    void notify_possible_setting_options();

    void check_status();

    bool load_definition_file(const std::string& uri, std::string& content);

    void refresh_params();
    void invalidate_params();

    void save_camera_mode(const float mavlink_camera_mode);
    float to_mavlink_camera_mode(const Camera::Mode mode) const;
    Camera::Mode to_camera_mode(const uint8_t mavlink_camera_mode) const;

    void* _camera_information_call_every_cookie{nullptr};
    void* _flight_information_call_every_cookie{nullptr};
    void* _check_connection_status_call_every_cookie{nullptr};

    void request_camera_settings();
    void request_camera_information();
    void request_video_stream_info();
    void request_status();
    void request_flight_information();

    MavlinkCommandSender::CommandLong make_command_take_photo(float interval_s, float no_of_photos);
    MavlinkCommandSender::CommandLong make_command_stop_photo();

    MavlinkCommandSender::CommandLong make_command_request_flight_information();
    MavlinkCommandSender::CommandLong make_command_request_camera_info();
    MavlinkCommandSender::CommandLong make_command_set_camera_mode(float mavlink_mode);
    MavlinkCommandSender::CommandLong make_command_request_camera_settings();
    MavlinkCommandSender::CommandLong make_command_request_camera_capture_status();
    MavlinkCommandSender::CommandLong make_command_request_storage_info();

    MavlinkCommandSender::CommandLong make_command_start_video(float capture_status_rate_hz);
    MavlinkCommandSender::CommandLong make_command_stop_video();

    MavlinkCommandSender::CommandLong make_command_start_video_streaming();
    MavlinkCommandSender::CommandLong make_command_stop_video_streaming();

    MavlinkCommandSender::CommandLong make_command_request_video_stream_info();

    std::unique_ptr<CameraDefinition> _camera_definition{};

    std::atomic<size_t> _camera_id{0};
    std::atomic<bool> _camera_found{false};

    struct {
        std::mutex mutex{};
        Camera::Status data{};
        bool received_camera_capture_status{false};
        bool received_storage_information{false};

        Camera::StatusCallback subscription_callback{nullptr};
        void* call_every_cookie{nullptr};
    } _status{};

    static constexpr double DEFAULT_TIMEOUT_S = 3.0;

    struct {
        std::mutex mutex{};
        Camera::Mode data{};
        Camera::ModeCallback subscription_callback{nullptr};
        void* call_every_cookie{nullptr};
    } _mode{};

    struct {
        std::mutex mutex{};
        int sequence = 1; // The MAVLink spec says the sequence starts at 1.
    } _capture{};

    struct {
        std::mutex mutex{};
        Camera::CaptureInfoCallback callback{nullptr};
    } _capture_info{};

    struct {
        std::mutex mutex{};
        Camera::VideoStreamInfo data{};
        bool available{false};
        void* call_every_cookie{nullptr};
        Camera::VideoStreamInfoCallback subscription_callback{nullptr};
    } _video_stream_info{};

    struct {
        mutable std::mutex mutex{};
        Camera::Information data{};
        Camera::InformationCallback subscription_callback{nullptr};
    } _information{};

    struct {
        std::mutex mutex{};
        Camera::CurrentSettingsCallback callback{nullptr};
    } _subscribe_current_settings{};

    struct {
        std::mutex mutex{};
        Camera::PossibleSettingOptionsCallback callback{nullptr};
    } _subscribe_possible_setting_options{};
};

} // namespace mavsdk
