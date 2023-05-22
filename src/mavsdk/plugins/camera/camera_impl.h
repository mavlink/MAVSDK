#pragma once

#include <map>

#include "camera_definition.h"
#include "mavlink_include.h"
#include "plugins/camera/camera.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "callback_list.h"

namespace mavsdk {

class CameraImpl : public PluginImplBase {
public:
    explicit CameraImpl(System& system);
    explicit CameraImpl(std::shared_ptr<System> system);
    ~CameraImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Camera::Result prepare();
    Camera::Result select_camera(size_t id);

    Camera::Result take_photo();

    Camera::Result start_photo_interval(float interval_s);
    Camera::Result stop_photo_interval();

    Camera::Result start_video();
    Camera::Result stop_video();

    void prepare_async(const Camera::ResultCallback& callback);
    void take_photo_async(const Camera::ResultCallback& callback);
    void start_photo_interval_async(float interval_s, const Camera::ResultCallback& callback);
    void stop_photo_interval_async(const Camera::ResultCallback& callback);
    void start_video_async(const Camera::ResultCallback& callback);
    void stop_video_async(const Camera::ResultCallback& callback);

    Camera::Information information() const;
    Camera::InformationHandle subscribe_information(const Camera::InformationCallback& callback);
    void unsubscribe_information(Camera::InformationHandle handle);

    std::pair<Camera::Result, Camera::VideoStreamInfo> get_video_stream_info();

    Camera::VideoStreamInfo video_stream_info();
    Camera::VideoStreamInfoHandle
    subscribe_video_stream_info(const Camera::VideoStreamInfoCallback& callback);
    void unsubscribe_video_stream_info(Camera::VideoStreamInfoHandle handle);

    Camera::Result start_video_streaming();
    Camera::Result stop_video_streaming();

    Camera::Result set_mode(const Camera::Mode mode);
    void set_mode_async(const Camera::Mode mode, const Camera::ResultCallback& callback);

    Camera::Mode mode();
    Camera::ModeHandle subscribe_mode(const Camera::ModeCallback& callback);
    void unsubscribe_mode(Camera::ModeHandle handle);

    Camera::CaptureInfoHandle subscribe_capture_info(const Camera::CaptureInfoCallback& callback);
    void unsubscribe_capture_info(Camera::CaptureInfoHandle handle);

    Camera::Status status();
    Camera::StatusHandle subscribe_status(const Camera::StatusCallback& callback);
    void unsubscribe_status(Camera::StatusHandle handle);

    Camera::Result set_setting(Camera::Setting setting);
    void set_setting_async(Camera::Setting setting, const Camera::ResultCallback callback);

    void get_setting_async(Camera::Setting setting, const Camera::GetSettingCallback callback);
    std::pair<Camera::Result, Camera::Setting> get_setting(Camera::Setting setting);

    std::vector<Camera::SettingOptions> possible_setting_options();

    bool is_setting_range(const std::string& setting_id);

    Camera::CurrentSettingsHandle
    subscribe_current_settings(const Camera::CurrentSettingsCallback& callback);
    void unsubscribe_current_settings(Camera::CurrentSettingsHandle handle);
    Camera::PossibleSettingOptionsHandle
    subscribe_possible_setting_options(const Camera::PossibleSettingOptionsCallback& callback);
    void unsubscribe_possible_setting_options(Camera::PossibleSettingOptionsHandle handle);

    Camera::Result format_storage();
    void format_storage_async(Camera::ResultCallback callback);

    std::pair<Camera::Result, std::vector<Camera::CaptureInfo>>
    list_photos(Camera::PhotosRange photos_range);
    void
    list_photos_async(Camera::PhotosRange photos_range, const Camera::ListPhotosCallback callback);

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
    void update_component();

    void receive_set_mode_command_result(
        const MavlinkCommandSender::Result command_result,
        const Camera::ResultCallback callback,
        const Camera::Mode mode);

    static Camera::Result
    camera_result_from_command_result(const MavlinkCommandSender::Result command_result);
    static Camera::Result
    camera_result_from_parameter_result(const MavlinkParameterClient::Result parameter_result);

    void receive_command_result(
        MavlinkCommandSender::Result command_result, const Camera::ResultCallback& callback);

    static bool interval_valid(float interval_s);

    void process_camera_image_captured(const mavlink_message_t& message);
    void process_storage_information(const mavlink_message_t& message);
    void process_camera_capture_status(const mavlink_message_t& message);
    void process_camera_settings(const mavlink_message_t& message);
    void process_camera_information(const mavlink_message_t& message);
    void process_video_information(const mavlink_message_t& message);
    void process_video_stream_status(const mavlink_message_t& message);
    void process_flight_information(const mavlink_message_t& message);
    void reset_following_format_storage();

    Camera::EulerAngle to_euler_angle_from_quaternion(Camera::Quaternion quaternion);

    void notify_mode();
    void notify_video_stream_info();
    void notify_current_settings();
    void notify_possible_setting_options();

    void check_status();

    bool should_fetch_camera_definition(const std::string& uri) const;
    bool fetch_camera_definition(
        const mavlink_camera_information_t& camera_information, std::string& camera_definition_out);
    bool download_definition_file(const std::string& uri, std::string& camera_definition_out);
    bool
    load_stored_definition(const mavlink_camera_information_t&, std::string& camera_definition_out);

    void refresh_params();
    void invalidate_params();

    void save_camera_mode(const float mavlink_camera_mode);
    float to_mavlink_camera_mode(const Camera::Mode mode) const;
    Camera::Mode to_camera_mode(const uint8_t mavlink_camera_mode) const;

    void* _camera_information_call_every_cookie{nullptr};
    void* _flight_information_call_every_cookie{nullptr};
    void* _check_connection_status_call_every_cookie{nullptr};
    void* _request_missing_capture_info_cookie{nullptr};

    void request_camera_settings();
    void request_camera_information();
    void request_video_stream_info();
    void request_video_stream_status();
    void request_status();
    void request_flight_information();

    MavlinkCommandSender::CommandLong make_command_take_photo(float interval_s, float no_of_photos);
    MavlinkCommandSender::CommandLong make_command_stop_photo();

    MavlinkCommandSender::CommandLong make_command_request_flight_information();
    MavlinkCommandSender::CommandLong make_command_request_camera_info();
    MavlinkCommandSender::CommandLong make_command_set_camera_mode(float mavlink_mode);
    MavlinkCommandSender::CommandLong make_command_request_camera_settings();
    MavlinkCommandSender::CommandLong make_command_request_camera_capture_status();
    MavlinkCommandSender::CommandLong make_command_request_camera_image_captured(size_t photo_id);
    MavlinkCommandSender::CommandLong make_command_request_storage_info();

    MavlinkCommandSender::CommandLong make_command_start_video(float capture_status_rate_hz);
    MavlinkCommandSender::CommandLong make_command_stop_video();

    MavlinkCommandSender::CommandLong make_command_start_video_streaming();
    MavlinkCommandSender::CommandLong make_command_stop_video_streaming();

    MavlinkCommandSender::CommandLong make_command_request_video_stream_info();
    MavlinkCommandSender::CommandLong make_command_request_video_stream_status();

    void request_missing_capture_info();

    std::unique_ptr<CameraDefinition> _camera_definition{};
    bool _is_fetching_camera_definition{false};
    bool _has_camera_definition_timed_out{false};
    size_t _camera_definition_fetch_count{0};
    using CameraDefinitionCallback = std::function<void(bool)>;
    CameraDefinitionCallback _camera_definition_callback{};

    std::atomic<size_t> _camera_id{0};
    std::atomic<bool> _camera_found{false};

    struct {
        std::mutex mutex{};
        Camera::Status data{};
        bool received_camera_capture_status{false};
        bool received_storage_information{false};
        int image_count{-1};
        int image_count_at_connection{-1};
        std::map<int, Camera::CaptureInfo> photo_list;
        bool is_fetching_photos{false};

        CallbackList<Camera::Status> subscription_callbacks{};
        void* call_every_cookie{nullptr};
    } _status{};

    static constexpr double DEFAULT_TIMEOUT_S = 3.0;

    struct {
        std::mutex mutex{};
        Camera::Mode data{};
        CallbackList<Camera::Mode> subscription_callbacks{};
        void* call_every_cookie{nullptr};
    } _mode{};

    struct {
        std::mutex mutex{};
        int sequence = 1; // The MAVLink spec says the sequence starts at 1.
    } _capture{};

    struct {
        std::mutex mutex{};
        CallbackList<Camera::CaptureInfo> callbacks{};
        int last_advertised_image_index{-1};
        std::map<int, int> missing_image_retries{};
    } _capture_info{};

    struct {
        std::mutex mutex{};
        Camera::VideoStreamInfo data{};
        bool available{false};
        void* call_every_cookie{nullptr};
        CallbackList<Camera::VideoStreamInfo> subscription_callbacks{};
    } _video_stream_info{};

    struct {
        mutable std::mutex mutex{};
        Camera::Information data{};
        CallbackList<Camera::Information> subscription_callbacks{};
    } _information{};

    struct {
        std::mutex mutex{};
        CallbackList<std::vector<Camera::Setting>> callbacks{};
    } _subscribe_current_settings{};

    struct {
        std::mutex mutex{};
        CallbackList<std::vector<Camera::SettingOptions>> callbacks{};
    } _subscribe_possible_setting_options{};

    std::condition_variable _captured_request_cv;
    std::mutex _captured_request_mutex;
};

} // namespace mavsdk
