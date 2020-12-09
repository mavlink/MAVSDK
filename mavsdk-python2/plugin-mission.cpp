#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mission/mission.h>

namespace py = pybind11;
void init_mission(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::Mission> mission(m, "Mission");

    mission.def("upload_mission", &mavsdk::Mission::upload_mission)
        .def("cancel_mission_upload", &mavsdk::Mission::cancel_mission_upload)

        .def("cancel_mission_download", &mavsdk::Mission::cancel_mission_download)
        .def("start_mission", &mavsdk::Mission::start_mission)
        .def("pause_mission", &mavsdk::Mission::pause_mission)
        .def("clear_mission", &mavsdk::Mission::clear_mission)
        .def("set_current_mission_item", &mavsdk::Mission::set_current_mission_item)

        .def("mission_progress", &mavsdk::Mission::mission_progress)

        .def(
            "set_return_to_launch_after_mission",
            &mavsdk::Mission::set_return_to_launch_after_mission)

        ;

    py::class_<mavsdk::Mission::MissionItem> mission_item(mission, "MissionItem");
    mission_item.def(py::init<>())
        .def_readwrite("latitude_deg", &mavsdk::Mission::MissionItem::latitude_deg)
        .def_readwrite("longitude_deg", &mavsdk::Mission::MissionItem::longitude_deg)
        .def_readwrite("relative_altitude_m", &mavsdk::Mission::MissionItem::relative_altitude_m)
        .def_readwrite("speed_m_s", &mavsdk::Mission::MissionItem::speed_m_s)
        .def_readwrite("is_fly_through", &mavsdk::Mission::MissionItem::is_fly_through)
        .def_readwrite("gimbal_pitch_deg", &mavsdk::Mission::MissionItem::gimbal_pitch_deg)
        .def_readwrite("gimbal_yaw_deg", &mavsdk::Mission::MissionItem::gimbal_yaw_deg)
        .def_readwrite("camera_action", &mavsdk::Mission::MissionItem::camera_action)
        .def_readwrite("loiter_time_s", &mavsdk::Mission::MissionItem::loiter_time_s)
        .def_readwrite(
            "camera_photo_interval_s", &mavsdk::Mission::MissionItem::camera_photo_interval_s);

    py::enum_<mavsdk::Mission::MissionItem::CameraAction>(mission_item, "CameraAction")
        .value("None", mavsdk::Mission::MissionItem::CameraAction::None)
        .value("TakePhoto", mavsdk::Mission::MissionItem::CameraAction::TakePhoto)
        .value("StartPhotoInterval", mavsdk::Mission::MissionItem::CameraAction::StartPhotoInterval)
        .value("StopPhotoInterval", mavsdk::Mission::MissionItem::CameraAction::StopPhotoInterval)
        .value("StartVideo", mavsdk::Mission::MissionItem::CameraAction::StartVideo)
        .value("StopVideo", mavsdk::Mission::MissionItem::CameraAction::StopVideo);

    py::class_<mavsdk::Mission::MissionPlan> mission_plan(mission, "MissionPlan");
    mission_plan.def(py::init<>())
        .def_readwrite("mission_items", &mavsdk::Mission::MissionPlan::mission_items);

    py::class_<mavsdk::Mission::MissionProgress> mission_progress(mission, "MissionProgress");
    mission_progress.def(py::init<>())
        .def_readwrite("current", &mavsdk::Mission::MissionProgress::current)
        .def_readwrite("total", &mavsdk::Mission::MissionProgress::total);

    py::enum_<mavsdk::Mission::Result>(mission, "Result")
        .value("Unknown", mavsdk::Mission::Result::Unknown)
        .value("Success", mavsdk::Mission::Result::Success)
        .value("Error", mavsdk::Mission::Result::Error)
        .value("TooManyMissionItems", mavsdk::Mission::Result::TooManyMissionItems)
        .value("Busy", mavsdk::Mission::Result::Busy)
        .value("Timeout", mavsdk::Mission::Result::Timeout)
        .value("InvalidArgument", mavsdk::Mission::Result::InvalidArgument)
        .value("Unsupported", mavsdk::Mission::Result::Unsupported)
        .value("NoMissionAvailable", mavsdk::Mission::Result::NoMissionAvailable)
        .value("FailedToOpenQgcPlan", mavsdk::Mission::Result::FailedToOpenQgcPlan)
        .value("FailedToParseQgcPlan", mavsdk::Mission::Result::FailedToParseQgcPlan)
        .value("UnsupportedMissionCmd", mavsdk::Mission::Result::UnsupportedMissionCmd)
        .value("TransferCancelled", mavsdk::Mission::Result::TransferCancelled);

    system.def(
        "mission",
        &mavsdk::System::mission,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}