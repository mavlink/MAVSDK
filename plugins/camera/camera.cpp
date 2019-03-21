#include "plugins/camera/camera.h"
#include "camera_impl.h"

namespace dronecode_sdk {

Camera::Camera(System &system) : PluginBase(), _impl{new CameraImpl(system)} {}

Camera::~Camera() {}

Camera::Result Camera::select_camera(unsigned id)
{
    return _impl->select_camera(id);
}

Camera::Result Camera::take_photo()
{
    return _impl->take_photo();
}

Camera::Result Camera::start_photo_interval(float interval_s)
{
    return _impl->start_photo_interval(interval_s);
}

Camera::Result Camera::stop_photo_interval()
{
    return _impl->stop_photo_interval();
}

Camera::Result Camera::start_video()
{
    return _impl->start_video();
}

Camera::Result Camera::stop_video()
{
    return _impl->stop_video();
}

void Camera::take_photo_async(const result_callback_t &callback)
{
    _impl->take_photo_async(callback);
}

void Camera::start_photo_interval_async(float interval_s, const result_callback_t &callback)
{
    _impl->start_photo_interval_async(interval_s, callback);
}

void Camera::stop_photo_interval_async(const result_callback_t &callback)
{
    _impl->stop_photo_interval_async(callback);
}

void Camera::start_video_async(const result_callback_t &callback)
{
    _impl->start_video_async(callback);
}

Camera::Information Camera::get_information()
{
    return _impl->get_information();
}

Camera::Result Camera::start_video_streaming()
{
    return _impl->start_video_streaming();
}

Camera::Result Camera::stop_video_streaming()
{
    return _impl->stop_video_streaming();
}

void Camera::set_video_stream_settings(const VideoStreamSettings &settings)
{
    return _impl->set_video_stream_settings(settings);
}

Camera::Result Camera::get_video_stream_info(VideoStreamInfo &info)
{
    return _impl->get_video_stream_info(info);
}

void Camera::get_video_stream_info_async(const get_video_stream_info_callback_t callback)
{
    _impl->get_video_stream_info_async(callback);
}

void Camera::subscribe_video_stream_info(const subscribe_video_stream_info_callback_t callback)
{
    _impl->subscribe_video_stream_info(callback);
}

void Camera::stop_video_async(const result_callback_t &callback)
{
    _impl->stop_video_async(callback);
}

Camera::Result Camera::set_mode(const Mode mode)
{
    return _impl->set_mode(mode);
}

void Camera::set_mode_async(const Mode mode, const mode_callback_t &callback)
{
    _impl->set_mode_async(mode, callback);
}

void Camera::get_mode_async(const mode_callback_t &callback)
{
    _impl->get_mode_async(callback);
}

void Camera::subscribe_mode(const subscribe_mode_callback_t callback)
{
    _impl->subscribe_mode(callback);
}

void Camera::get_status_async(get_status_callback_t callback)
{
    _impl->get_status_async(callback);
}

void Camera::subscribe_status(const Camera::subscribe_status_callback_t callback)
{
    _impl->subscribe_status(callback);
}

void Camera::subscribe_capture_info(capture_info_callback_t callback)
{
    _impl->subscribe_capture_info(callback);
}

void Camera::set_option_async(const result_callback_t &callback,
                              const std::string &setting_id,
                              const Option &option)
{
    _impl->set_option_async(setting_id, option, callback);
}

Camera::Result Camera::get_option(const std::string &setting_id, Option &option)
{
    return _impl->get_option(setting_id, option);
}

void Camera::get_option_async(const std::string &setting_id, const get_option_callback_t &callback)
{
    _impl->get_option_async(setting_id, callback);
}

bool Camera::get_possible_setting_options(std::vector<std::string> &settings)
{
    return _impl->get_possible_setting_options(settings);
}

bool Camera::get_possible_options(const std::string &setting_id,
                                  std::vector<Camera::Option> &options)
{
    return _impl->get_possible_options(setting_id, options);
}

void Camera::subscribe_current_settings(const subscribe_current_settings_callback_t &callback)
{
    _impl->subscribe_current_settings(callback);
}

void Camera::subscribe_possible_setting_options(
    const subscribe_possible_setting_options_callback_t &callback)
{
    _impl->subscribe_possible_setting_options(callback);
}

Camera::Result Camera::format_storage()
{
    return _impl->format_storage();
}

void Camera::format_storage_async(result_callback_t callback)
{
    _impl->format_storage_async(callback);
}

std::string Camera::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::IN_PROGRESS:
            return "In progress";
        case Result::BUSY:
            return "Busy";
        case Result::DENIED:
            return "Denied";
        case Result::ERROR:
            return "Error";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::WRONG_ARGUMENT:
            return "Wrong argument";
        case Result::UNKNOWN:
        // FALLTHROUGH
        default:
            return "Unknown";
    }
}

bool operator==(const Camera::VideoStreamSettings &lhs, const Camera::VideoStreamSettings &rhs)
{
    return lhs.frame_rate_hz == rhs.frame_rate_hz &&
           lhs.horizontal_resolution_pix == rhs.horizontal_resolution_pix &&
           lhs.vertical_resolution_pix == rhs.vertical_resolution_pix &&
           lhs.bit_rate_b_s == rhs.bit_rate_b_s && lhs.rotation_deg == rhs.rotation_deg &&
           lhs.uri == rhs.uri;
}

std::ostream &operator<<(std::ostream &str,
                         Camera::VideoStreamSettings const &video_stream_settings)
{
    return str << "[frame_rate_hz: " << video_stream_settings.frame_rate_hz
               << ", horizontal_resolution_pix: " << video_stream_settings.horizontal_resolution_pix
               << ", vertical_resolution_pix: " << video_stream_settings.vertical_resolution_pix
               << ", bit_rate_b_s: " << video_stream_settings.bit_rate_b_s
               << ", rotation_deg: " << video_stream_settings.rotation_deg
               << ", uri: " << video_stream_settings.uri << "]";
}

bool operator==(const Camera::VideoStreamInfo &lhs, const Camera::VideoStreamInfo &rhs)
{
    return lhs.settings == rhs.settings && lhs.status == rhs.status;
}

std::ostream &operator<<(std::ostream &str, Camera::VideoStreamInfo const &video_stream_info)
{
    return str << "[" << std::endl
               << "settings: " << video_stream_info.settings << std::endl
               << "status: " << video_stream_info.status << std::endl
               << "]";
}

std::ostream &operator<<(std::ostream &str,
                         Camera::VideoStreamInfo::Status const &video_stream_info_status)
{
    switch (video_stream_info_status) {
        case Camera::VideoStreamInfo::Status::IN_PROGRESS:
            return str << "IN_PROGRESS";
        case Camera::VideoStreamInfo::Status::NOT_RUNNING:
            return str << "NOT_RUNNING";
        default:
            return str << "UNKNOWN";
    }
}

bool operator==(const Camera::CaptureInfo &lhs, const Camera::CaptureInfo &rhs)
{
    return lhs.position == rhs.position && lhs.attitude_quaternion == rhs.attitude_quaternion &&
           lhs.attitude_euler_angle == rhs.attitude_euler_angle &&
           lhs.time_utc_us == rhs.time_utc_us && lhs.success == rhs.success &&
           lhs.index == rhs.index && lhs.file_url == rhs.file_url;
}

std::ostream &operator<<(std::ostream &str, Camera::CaptureInfo const &capture_info)
{
    return str << "[" << std::endl
               << "position: " << capture_info.position << std::endl
               << "attitude_quaternion: " << capture_info.attitude_quaternion << std::endl
               << "attitude_euler_angle: " << capture_info.attitude_euler_angle << std::endl
               << "time_utc_us: " << capture_info.time_utc_us << std::endl
               << "is_success: " << capture_info.success << std::endl
               << "index: " << capture_info.index << std::endl
               << "file_url: " << capture_info.file_url << std::endl;
}

bool operator==(const Camera::CaptureInfo::Position &lhs, const Camera::CaptureInfo::Position &rhs)
{
    return lhs.latitude_deg == rhs.latitude_deg && lhs.longitude_deg == rhs.longitude_deg &&
           lhs.absolute_altitude_m == rhs.absolute_altitude_m &&
           lhs.relative_altitude_m == rhs.relative_altitude_m;
}

std::ostream &operator<<(std::ostream &str, Camera::CaptureInfo::Position const &position)
{
    return str << "[latitude_deg: " << position.latitude_deg
               << ", longitude_deg: " << position.longitude_deg
               << ", absolute_altitude_m: " << position.absolute_altitude_m
               << ", relative_altitude_m: " << position.relative_altitude_m << "]";
}

bool operator==(const Camera::CaptureInfo::Quaternion &lhs,
                const Camera::CaptureInfo::Quaternion &rhs)
{
    return lhs.w == rhs.w && lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

std::ostream &operator<<(std::ostream &str, Camera::CaptureInfo::Quaternion const &quaternion)
{
    return str << "[w: " << quaternion.w << ", x: " << quaternion.x << ", y: " << quaternion.y
               << ", z: " << quaternion.z << "]";
}

bool operator==(const Camera::CaptureInfo::EulerAngle &lhs,
                const Camera::CaptureInfo::EulerAngle &rhs)
{
    return lhs.yaw_deg == rhs.yaw_deg && lhs.pitch_deg == rhs.pitch_deg &&
           lhs.roll_deg == rhs.roll_deg;
}

std::ostream &operator<<(std::ostream &str, Camera::CaptureInfo::EulerAngle const &euler_angle)
{
    return str << "[yaw_deg: " << euler_angle.yaw_deg << ", pitch_deg: " << euler_angle.pitch_deg
               << ", roll_deg: " << euler_angle.roll_deg;
}

bool operator==(const Camera::Status &lhs, const Camera::Status &rhs)
{
    return lhs.video_on == rhs.video_on && lhs.photo_interval_on == rhs.photo_interval_on &&
           lhs.used_storage_mib == rhs.used_storage_mib &&
           lhs.available_storage_mib == rhs.available_storage_mib &&
           lhs.total_storage_mib == rhs.total_storage_mib &&
           lhs.media_folder_name == rhs.media_folder_name &&
           lhs.recording_time_s == rhs.recording_time_s && lhs.storage_status == rhs.storage_status;
}

std::ostream &operator<<(std::ostream &str, Camera::Status const &status)
{
    return str << "[video_on: " << status.video_on
               << ", photo_interval_on: " << status.photo_interval_on
               << ", used_storage_mib: " << status.used_storage_mib
               << ", available_storage_mib: " << status.available_storage_mib
               << ", total_storage_mib: " << status.total_storage_mib
               << ", recording_time_s: " << status.recording_time_s
               << ", media_folder_name: " << status.media_folder_name
               << ", storage_status: " << status.storage_status << "]";
}

std::ostream &operator<<(std::ostream &str, Camera::Status::StorageStatus const &storage_status)
{
    switch (storage_status) {
        case Camera::Status::StorageStatus::UNFORMATTED:
            return str << "UNFORMATTED";
        case Camera::Status::StorageStatus::FORMATTED:
            return str << "FORMATTED";
        case Camera::Status::StorageStatus::NOT_AVAILABLE:
            return str << "NOT_AVAILABLE";
        default:
            return str << "UNKNOWN";
    }
}

bool operator==(const Camera::Setting &lhs, const Camera::Setting &rhs)
{
    return lhs.setting_id == rhs.setting_id && lhs.setting_description == rhs.setting_description &&
           lhs.option == rhs.option;
}

std::ostream &operator<<(std::ostream &str, Camera::Setting const &setting)
{
    return str << "[setting_id: " << setting.setting_id
               << ", setting_description: " << setting.setting_description
               << ", option: " << setting.option << "]";
}

bool operator==(const Camera::Option &lhs, const Camera::Option &rhs)
{
    return (lhs.option_id == rhs.option_id) && (lhs.option_description == rhs.option_description);
}

std::ostream &operator<<(std::ostream &str, Camera::Option const &option)
{
    return str << "[option_id: " << option.option_id
               << ", option_description: " << option.option_description << "]";
}

bool operator==(const Camera::SettingOptions &lhs, const Camera::SettingOptions &rhs)
{
    if (lhs.options.size() != rhs.options.size()) {
        return false;
    }

    for (size_t i = 0; i < lhs.options.size(); i++) {
        if (!(lhs.options.at(i) == rhs.options.at(i))) {
            return false;
        }
    }

    return lhs.setting_id == rhs.setting_id && lhs.setting_description == rhs.setting_description;
}

std::ostream &operator<<(std::ostream &str, Camera::SettingOptions const &setting_options)
{
    str << "[setting_id: " << setting_options.setting_id;
    str << ", setting_description: " << setting_options.setting_description;
    str << ", options: [";

    for (const auto &option : setting_options.options) {
        str << option;
    }

    str << "]";

    return str;
}

} // namespace dronecode_sdk
