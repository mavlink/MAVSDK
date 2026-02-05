#pragma once

#include <cstdint>
#include <filesystem>
#include <map>
#include <mutex>
#include <thread>

#include "camera_definition.h"
#include "file_cache.h"
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

    Camera::Result take_photo(int32_t camera_id);

    Camera::Result start_photo_interval(int32_t camera_id, float interval_s);
    Camera::Result stop_photo_interval(int32_t camera_id);

    Camera::Result start_video(int32_t camera_id);
    Camera::Result stop_video(int32_t camera_id);

    Camera::Result zoom_in_start(int32_t camera_id);
    Camera::Result zoom_out_start(int32_t camera_id);
    Camera::Result zoom_stop(int32_t camera_id);
    Camera::Result zoom_range(int32_t camera_id, float range);

    Camera::Result track_point(int32_t camera_id, float point_x, float point_y, float radius);
    Camera::Result track_rectangle(
        int32_t camera_id,
        float top_left_x,
        float top_left_y,
        float bottom_right_x,
        float bottom_right_y);
    Camera::Result track_stop(int32_t camera_id);
    Camera::Result focus_in_start(int32_t camera_id);
    Camera::Result focus_out_start(int32_t camera_id);
    Camera::Result focus_stop(int32_t camera_id);
    Camera::Result focus_range(int32_t camera_id, float range);

    void zoom_in_start_async(int32_t camera_id, const Camera::ResultCallback& callback);
    void zoom_out_start_async(int32_t camera_id, const Camera::ResultCallback& callback);
    void zoom_stop_async(int32_t camera_id, const Camera::ResultCallback& callback);
    void zoom_range_async(int32_t camera_id, float range, const Camera::ResultCallback& callback);

    void track_point_async(
        int32_t camera_id,
        float point_x,
        float point_y,
        float radius,
        const Camera::ResultCallback& callback);
    void track_rectangle_async(
        int32_t camera_id,
        float top_left_x,
        float top_left_y,
        float bottom_right_x,
        float bottom_right_y,
        const Camera::ResultCallback& callback);
    void track_stop_async(int32_t camera_id, const Camera::ResultCallback& callback);

    void focus_in_start_async(int32_t camera_id, const Camera::ResultCallback& callback);
    void focus_out_start_async(int32_t camera_id, const Camera::ResultCallback& callback);
    void focus_stop_async(int32_t camera_id, const Camera::ResultCallback& callback);
    void focus_range_async(int32_t camera_id, float range, const Camera::ResultCallback& callback);

    void take_photo_async(int32_t camera_id, const Camera::ResultCallback& callback);
    void start_photo_interval_async(
        int32_t camera_id, float interval_s, const Camera::ResultCallback& callback);
    void stop_photo_interval_async(int32_t camera_id, const Camera::ResultCallback& callback);
    void start_video_async(int32_t camera_id, const Camera::ResultCallback& callback);
    void stop_video_async(int32_t camera_id, const Camera::ResultCallback& callback);

    Camera::CameraList camera_list();
    Camera::CameraListHandle subscribe_camera_list(const Camera::CameraListCallback& callback);
    void unsubscribe_camera_list(Camera::CameraListHandle handle);

    Camera::VideoStreamInfo video_stream_info();
    Camera::VideoStreamInfoHandle
    subscribe_video_stream_info(const Camera::VideoStreamInfoCallback& callback);
    void unsubscribe_video_stream_info(Camera::VideoStreamInfoHandle handle);

    std::pair<Camera::Result, Camera::VideoStreamInfo> get_video_stream_info(int32_t camera_id);

    Camera::Result start_video_streaming(int32_t camera_id, int32_t stream_id);
    Camera::Result stop_video_streaming(int32_t camera_id, int32_t stream_id);

    Camera::Result set_mode(int32_t camera_id, Camera::Mode mode);
    void
    set_mode_async(int32_t camera_id, Camera::Mode mode, const Camera::ResultCallback& callback);

    std::pair<Camera::Result, Camera::Mode> get_mode(int32_t camera_id);

    Camera::ModeHandle subscribe_mode(const Camera::ModeCallback& callback);
    void unsubscribe_mode(Camera::ModeHandle handle);

    Camera::CaptureInfoHandle subscribe_capture_info(const Camera::CaptureInfoCallback& callback);
    void unsubscribe_capture_info(Camera::CaptureInfoHandle handle);

    Camera::StorageHandle subscribe_storage(const Camera::StorageCallback& callback);
    void unsubscribe_storage(Camera::StorageHandle handle);

    std::pair<Camera::Result, Camera::Storage> get_storage(int32_t camera_id);

    void get_setting_async(
        int32_t camera_id,
        const Camera::Setting& setting,
        const Camera::GetSettingCallback& callback);
    Camera::Result set_setting(int32_t camera_id, const Camera::Setting& setting);
    void set_setting_async(
        int32_t camera_id, const Camera::Setting& setting, const Camera::ResultCallback& callback);

    std::pair<Camera::Result, Camera::Setting>
    get_setting(int32_t camera_id, const Camera::Setting& setting);

    bool is_setting_range(const std::string& setting_id);

    Camera::CurrentSettingsHandle
    subscribe_current_settings(const Camera::CurrentSettingsCallback& callback);
    void unsubscribe_current_settings(Camera::CurrentSettingsHandle handle);

    std::pair<Camera::Result, std::vector<Camera::Setting>> get_current_settings(int32_t camera_id);

    Camera::PossibleSettingOptionsHandle
    subscribe_possible_setting_options(const Camera::PossibleSettingOptionsCallback& callback);
    void unsubscribe_possible_setting_options(Camera::PossibleSettingOptionsHandle handle);

    std::pair<Camera::Result, std::vector<Camera::SettingOptions>>
    get_possible_setting_options(int32_t camera_id);

    Camera::Result format_storage(int32_t camera_id, int32_t storage_id);
    void format_storage_async(
        int32_t camera_id, int32_t storage_id, const Camera::ResultCallback& callback);

    Camera::Result reset_settings(int32_t camera_id);
    void reset_settings_async(int32_t camera_id, const Camera::ResultCallback& callback);

    std::pair<Camera::Result, std::vector<Camera::CaptureInfo>>
    list_photos(int32_t camera_id, Camera::PhotosRange photos_range);
    void list_photos_async(
        int32_t camera_id,
        Camera::PhotosRange photos_range,
        const Camera::ListPhotosCallback& callback);

    CameraImpl(const CameraImpl&) = delete;
    CameraImpl& operator=(const CameraImpl&) = delete;

private:
    struct CaptureStatus {
        bool received_camera_capture_status{false};
        bool received_storage_information{false};
        int image_count{-1};
        int image_count_at_connection{-1};
        std::map<int, Camera::CaptureInfo> photo_list;
        Camera::PhotosRange photos_range{Camera::PhotosRange::SinceConnection};
    };

    struct CaptureInfo {
        int last_advertised_image_index{-1};
        std::map<int, int> missing_image_retries{};
    };

    struct PotentialCamera {
        explicit PotentialCamera(uint8_t new_component_id) : component_id(new_component_id) {}

        std::optional<Camera::Information> maybe_information;
        std::unique_ptr<CameraDefinition> camera_definition{};
        std::string camera_definition_url{};
        unsigned camera_definition_version{};
        Camera::Result camera_definition_result{Camera::Result::Unknown};
        bool is_fetching_camera_definition{false};
        size_t camera_definition_fetch_count{0};

        bool information_requested{false};

        CaptureStatus capture_status{};
        CaptureInfo capture_info{};

        Camera::Storage storage{};
        Camera::VideoStreamInfo video_stream_info{};
        Camera::Mode mode{Camera::Mode::Unknown};

        uint16_t capture_sequence{0};

        bool received_storage{false};
        bool received_video_stream_info{false};

        // This is 1-6 for autopilot based cameras. Commands need to be sent out to 1 (autopilot).
        uint8_t component_id;

        bool operator==(const PotentialCamera& other) const
        {
            return this->component_id == other.component_id;
        }
    };

    Camera::CameraList camera_list_with_lock();

    std::pair<Camera::Result, std::vector<Camera::SettingOptions>>
    get_possible_setting_options_with_lock(PotentialCamera& potential_camera);

    void get_setting_async_with_lock(
        PotentialCamera& potential_camera,
        Camera::Setting setting,
        const Camera::GetSettingCallback& callback);

    bool get_possible_options_with_lock(
        PotentialCamera& camera,
        const std::string& setting_id,
        std::vector<Camera::Option>& options);

    void set_option_async(
        int32_t camera_id,
        const std::string& setting_id,
        const Camera::Option& option,
        const Camera::ResultCallback& callback);

    Camera::Result
    get_option(int32_t camera_id, const std::string& setting_id, Camera::Option& option);
    void get_option_async(
        int32_t camera_id,
        const std::string& setting_id,
        const std::function<void(Camera::Result, const Camera::Option&)>& callback);

    bool get_setting_str_with_lock(
        PotentialCamera& potential_camera, const std::string& setting_id, std::string& description);
    bool get_option_str_with_lock(
        PotentialCamera& potential_camera,
        const std::string& setting_id,
        const std::string& option_id,
        std::string& description);

    void receive_set_mode_command_result(
        MavlinkCommandSender::Result command_result,
        const Camera::ResultCallback& callback,
        Camera::Mode mode,
        int32_t camera_id);

    static Camera::Result
    camera_result_from_command_result(MavlinkCommandSender::Result command_result);
    static Camera::Result
    camera_result_from_parameter_result(MavlinkParameterClient::Result parameter_result);

    void receive_command_result(
        MavlinkCommandSender::Result command_result, const Camera::ResultCallback& callback) const;

    void process_heartbeat(const mavlink_message_t& message);
    void process_camera_image_captured(const mavlink_message_t& message);
    void process_storage_information(const mavlink_message_t& message);
    void process_camera_capture_status(const mavlink_message_t& message);
    void process_camera_settings(const mavlink_message_t& message);
    void process_camera_information(const mavlink_message_t& message);
    void process_video_information(const mavlink_message_t& message);
    void process_video_stream_status(const mavlink_message_t& message);

    void reset_following_format_storage_with_lock(PotentialCamera& potential_camera);

    void check_potential_cameras_with_lock();
    void check_camera_definition_with_lock(PotentialCamera& potential_camera);
    void load_camera_definition_with_lock(
        PotentialCamera& potential_camera, const std::filesystem::path& path);

    void notify_mode_for_all_with_lock();
    void notify_mode_with_lock(PotentialCamera& camera);

    void notify_video_stream_info_for_all_with_lock();
    void notify_video_stream_info_with_lock(PotentialCamera& camera);

    void notify_current_settings_for_all_with_lock();
    void notify_current_settings_with_lock(PotentialCamera& potential_camera);

    void notify_possible_setting_options_for_all_with_lock();
    void notify_possible_setting_options_with_lock(PotentialCamera& potential_camera);

    void notify_camera_list_with_lock();

    void notify_storage_for_all_with_lock();
    void notify_storage_with_lock(PotentialCamera& camera);

    void refresh_params_with_lock(PotentialCamera& camera, bool initial_load);

    void save_camera_mode_with_lock(PotentialCamera& potential_camera, Camera::Mode mode);

    static Camera::Storage::StorageStatus storage_status_from_mavlink(int storage_status);
    static Camera::Storage::StorageType storage_type_from_mavlink(int storage_type);
    static float to_mavlink_camera_mode(Camera::Mode mode);
    static Camera::Mode to_camera_mode(uint8_t mavlink_camera_mode);

    CallEveryHandler::Cookie _camera_information_call_every_cookie{};
    CallEveryHandler::Cookie _request_missing_capture_info_cookie{};

    // void request_camera_settings(int32_t camera_id);
    void request_camera_information(uint8_t component_id);
    // void request_video_stream_info(int32_t camera_id);
    // void request_video_stream_status(int32_t camera_id);
    // void request_status(int32_t camera_id);

    MavlinkCommandSender::CommandLong
    make_command_take_photo(int32_t camera_id, float interval_s, float no_of_photos);
    MavlinkCommandSender::CommandLong make_command_stop_photo(int32_t camera_id);

    MavlinkCommandSender::CommandLong
    make_command_set_camera_mode(int32_t camera_id, float mavlink_mode);

    MavlinkCommandSender::CommandLong
    make_command_start_video(int32_t camera_id, float capture_status_rate_hz);
    MavlinkCommandSender::CommandLong make_command_stop_video(int32_t camera_id);

    MavlinkCommandSender::CommandLong
    make_command_start_video_streaming(int32_t camera_id, int32_t stream_id);
    MavlinkCommandSender::CommandLong
    make_command_stop_video_streaming(int32_t camera_id, int32_t stream_id);

    MavlinkCommandSender::CommandLong make_command_zoom_in(int32_t camera_id);
    MavlinkCommandSender::CommandLong make_command_zoom_out(int32_t camera_id);
    MavlinkCommandSender::CommandLong make_command_zoom_stop(int32_t camera_id);
    MavlinkCommandSender::CommandLong make_command_zoom_range(int32_t camera_id, float range);
    MavlinkCommandSender::CommandLong
    make_command_track_point(int32_t camera_id, float point_x, float point_y, float radius);
    MavlinkCommandSender::CommandLong make_command_track_rectangle(
        int32_t camera_id,
        float top_left_x,
        float top_left_y,
        float bottom_right_x,
        float bottom_right_y);
    MavlinkCommandSender::CommandLong make_command_track_stop(int32_t camera_id);
    MavlinkCommandSender::CommandLong make_command_focus_in(int32_t camera_id);
    MavlinkCommandSender::CommandLong make_command_focus_out(int32_t camera_id);
    MavlinkCommandSender::CommandLong make_command_focus_stop(int32_t camera_id);
    MavlinkCommandSender::CommandLong make_command_focus_range(int32_t camera_id, float range);

    void request_slower();
    void request_faster();

    void request_missing_capture_info();

    void subscribe_to_param_changes_with_lock(
        PotentialCamera& camera, std::string param_name, const ParamValue& param_value_type);

    uint16_t get_and_increment_capture_sequence(int32_t camera_id);

    std::vector<PotentialCamera*> potential_cameras_with_lock();
    int32_t camera_id_for_potential_camera_with_lock(const PotentialCamera& potential_camera);
    PotentialCamera* maybe_potential_camera_for_camera_id_with_lock(int32_t camera_id);
    std::vector<PotentialCamera*> potential_cameras_for_camera_id_with_lock(int32_t camera_id);

    PotentialCamera* maybe_potential_camera_for_component_id_with_lock(
        uint8_t component_id, uint8_t camera_device_id);

    static std::string get_filename_from_path(const std::string& path);

    static uint8_t fixup_component_target(uint8_t component_id);

    std::mutex _mutex;
    std::vector<PotentialCamera> _potential_cameras;
    CallbackList<Camera::CameraList> _camera_list_subscription_callbacks{};
    CallbackList<Camera::PossibleSettingOptionsUpdate>
        _subscribe_possible_setting_options_callbacks{};
    CallbackList<Camera::CurrentSettingsUpdate> _subscribe_current_settings_callbacks{};
    CallbackList<Camera::ModeUpdate> _mode_subscription_callbacks{};
    CallbackList<Camera::CaptureInfo> _capture_info_callbacks{};
    CallbackList<Camera::VideoStreamUpdate> _video_stream_info_subscription_callbacks{};

    CallEveryHandler::Cookie _check_potential_cameras_call_every_cookie{};

    std::optional<FileCache> _file_cache{};
    std::filesystem::path _tmp_download_path{};

    CallbackList<Camera::StorageUpdate> _storage_subscription_callbacks{};

    CallEveryHandler::Cookie _request_slower_call_every_cookie{};
    CallEveryHandler::Cookie _request_faster_call_every_cookie{};

#if BUILD_WITHOUT_CURL != 1
    std::unique_ptr<HttpLoader> _http_loader{nullptr};
#endif

    bool _debugging{false};
};

} // namespace mavsdk
