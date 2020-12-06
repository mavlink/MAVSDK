#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera/camera.h>

namespace py = pybind11;
void init_camera(py::module_ &m) {
    py::class_<mavsdk::Camera> camera(m, "Camera");


camera.def(py::init<std::shared_ptr<mavsdk::System>>())
    .def("take_photo", &mavsdk::Camera::take_photo)
    .def("start_photo_interval", &mavsdk::Camera::start_photo_interval)
    .def("stop_photo_interval", &mavsdk::Camera::stop_photo_interval)
    .def("start_video", &mavsdk::Camera::start_video)
    .def("stop_video", &mavsdk::Camera::stop_video)
    .def("start_video_streaming", &mavsdk::Camera::start_video_streaming)
    .def("stop_video_streaming", &mavsdk::Camera::stop_video_streaming)
    .def("set_mode", &mavsdk::Camera::set_mode)
    .def("mode", &mavsdk::Camera::mode)
    .def("information", &mavsdk::Camera::information)
    .def("video_stream_info", &mavsdk::Camera::video_stream_info)

    .def("status", &mavsdk::Camera::status)

    .def("possible_setting_options", &mavsdk::Camera::possible_setting_options)
    .def("set_setting", &mavsdk::Camera::set_setting)

    .def("format_storage", &mavsdk::Camera::format_storage)
;


    py::enum_<mavsdk::Camera::Mode>(camera, "Mode")
        .value("Unknown", mavsdk::Camera::Mode::Unknown)
        .value("Photo", mavsdk::Camera::Mode::Photo)
        .value("Video", mavsdk::Camera::Mode::Video);





py::enum_<mavsdk::Camera::Result>(camera, "Result")
    .value("Unknown", mavsdk::Camera::Result::Unknown)
    .value("Success", mavsdk::Camera::Result::Success)
    .value("InProgress", mavsdk::Camera::Result::InProgress)
    .value("Busy", mavsdk::Camera::Result::Busy)
    .value("Denied", mavsdk::Camera::Result::Denied)
    .value("Error", mavsdk::Camera::Result::Error)
    .value("Timeout", mavsdk::Camera::Result::Timeout)
    .value("WrongArgument", mavsdk::Camera::Result::WrongArgument);



py::class_<mavsdk::Camera::Position> position(camera, "Position");
position
    .def(py::init<>())
    .def_readwrite("latitude_deg", &mavsdk::Camera::Position::latitude_deg)
    .def_readwrite("longitude_deg", &mavsdk::Camera::Position::longitude_deg)
    .def_readwrite("absolute_altitude_m", &mavsdk::Camera::Position::absolute_altitude_m)
    .def_readwrite("relative_altitude_m", &mavsdk::Camera::Position::relative_altitude_m);




py::class_<mavsdk::Camera::Quaternion> quaternion(camera, "Quaternion");
quaternion
    .def(py::init<>())
    .def_readwrite("w", &mavsdk::Camera::Quaternion::w)
    .def_readwrite("x", &mavsdk::Camera::Quaternion::x)
    .def_readwrite("y", &mavsdk::Camera::Quaternion::y)
    .def_readwrite("z", &mavsdk::Camera::Quaternion::z);




py::class_<mavsdk::Camera::EulerAngle> euler_angle(camera, "EulerAngle");
euler_angle
    .def(py::init<>())
    .def_readwrite("roll_deg", &mavsdk::Camera::EulerAngle::roll_deg)
    .def_readwrite("pitch_deg", &mavsdk::Camera::EulerAngle::pitch_deg)
    .def_readwrite("yaw_deg", &mavsdk::Camera::EulerAngle::yaw_deg);




py::class_<mavsdk::Camera::CaptureInfo> capture_info(camera, "CaptureInfo");
capture_info
    .def(py::init<>())
    .def_readwrite("position", &mavsdk::Camera::CaptureInfo::position)
    .def_readwrite("attitude_quaternion", &mavsdk::Camera::CaptureInfo::attitude_quaternion)
    .def_readwrite("attitude_euler_angle", &mavsdk::Camera::CaptureInfo::attitude_euler_angle)
    .def_readwrite("time_utc_us", &mavsdk::Camera::CaptureInfo::time_utc_us)
    .def_readwrite("is_success", &mavsdk::Camera::CaptureInfo::is_success)
    .def_readwrite("index", &mavsdk::Camera::CaptureInfo::index)
    .def_readwrite("file_url", &mavsdk::Camera::CaptureInfo::file_url);




py::class_<mavsdk::Camera::VideoStreamSettings> video_stream_settings(camera, "VideoStreamSettings");
video_stream_settings
    .def(py::init<>())
    .def_readwrite("frame_rate_hz", &mavsdk::Camera::VideoStreamSettings::frame_rate_hz)
    .def_readwrite("horizontal_resolution_pix", &mavsdk::Camera::VideoStreamSettings::horizontal_resolution_pix)
    .def_readwrite("vertical_resolution_pix", &mavsdk::Camera::VideoStreamSettings::vertical_resolution_pix)
    .def_readwrite("bit_rate_b_s", &mavsdk::Camera::VideoStreamSettings::bit_rate_b_s)
    .def_readwrite("rotation_deg", &mavsdk::Camera::VideoStreamSettings::rotation_deg)
    .def_readwrite("uri", &mavsdk::Camera::VideoStreamSettings::uri);





py::class_<mavsdk::Camera::VideoStreamInfo> video_stream_info(camera, "VideoStreamInfo");
video_stream_info
    .def(py::init<>())
    .def_readwrite("settings", &mavsdk::Camera::VideoStreamInfo::settings)
    .def_readwrite("status", &mavsdk::Camera::VideoStreamInfo::status);


py::enum_<mavsdk::Camera::VideoStreamInfo::Status>(video_stream_info, "Status")
    .value("NotRunning", mavsdk::Camera::VideoStreamInfo::Status::NotRunning)
    .value("InProgress", mavsdk::Camera::VideoStreamInfo::Status::InProgress);




py::class_<mavsdk::Camera::Status> status(camera, "Status");
status
    .def(py::init<>())
    .def_readwrite("video_on", &mavsdk::Camera::Status::video_on)
    .def_readwrite("photo_interval_on", &mavsdk::Camera::Status::photo_interval_on)
    .def_readwrite("used_storage_mib", &mavsdk::Camera::Status::used_storage_mib)
    .def_readwrite("available_storage_mib", &mavsdk::Camera::Status::available_storage_mib)
    .def_readwrite("total_storage_mib", &mavsdk::Camera::Status::total_storage_mib)
    .def_readwrite("recording_time_s", &mavsdk::Camera::Status::recording_time_s)
    .def_readwrite("media_folder_name", &mavsdk::Camera::Status::media_folder_name)
    .def_readwrite("storage_status", &mavsdk::Camera::Status::storage_status);


py::enum_<mavsdk::Camera::Status::StorageStatus>(status, "StorageStatus")
    .value("NotAvailable", mavsdk::Camera::Status::StorageStatus::NotAvailable)
    .value("Unformatted", mavsdk::Camera::Status::StorageStatus::Unformatted)
    .value("Formatted", mavsdk::Camera::Status::StorageStatus::Formatted);



py::class_<mavsdk::Camera::Option> option(camera, "Option");
option
    .def(py::init<>())
    .def_readwrite("option_id", &mavsdk::Camera::Option::option_id)
    .def_readwrite("option_description", &mavsdk::Camera::Option::option_description);




py::class_<mavsdk::Camera::Setting> setting(camera, "Setting");
setting
    .def(py::init<>())
    .def_readwrite("setting_id", &mavsdk::Camera::Setting::setting_id)
    .def_readwrite("setting_description", &mavsdk::Camera::Setting::setting_description)
    .def_readwrite("option", &mavsdk::Camera::Setting::option)
    .def_readwrite("is_range", &mavsdk::Camera::Setting::is_range);




py::class_<mavsdk::Camera::SettingOptions> setting_options(camera, "SettingOptions");
setting_options
    .def(py::init<>())
    .def_readwrite("setting_id", &mavsdk::Camera::SettingOptions::setting_id)
    .def_readwrite("setting_description", &mavsdk::Camera::SettingOptions::setting_description)
    .def_readwrite("options", &mavsdk::Camera::SettingOptions::options)
    .def_readwrite("is_range", &mavsdk::Camera::SettingOptions::is_range);




py::class_<mavsdk::Camera::Information> information(camera, "Information");
information
    .def(py::init<>())
    .def_readwrite("vendor_name", &mavsdk::Camera::Information::vendor_name)
    .def_readwrite("model_name", &mavsdk::Camera::Information::model_name);



}