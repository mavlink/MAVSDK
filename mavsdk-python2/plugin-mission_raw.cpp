#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mission_raw/mission_raw.h>

namespace py = pybind11;
void init_mission_raw(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::MissionRaw> mission_raw(m, "MissionRaw");

    mission_raw.def("upload_mission", &mavsdk::MissionRaw::upload_mission)
        .def("cancel_mission_upload", &mavsdk::MissionRaw::cancel_mission_upload)
        .def("download_mission", &mavsdk::MissionRaw::download_mission)
        .def("cancel_mission_download", &mavsdk::MissionRaw::cancel_mission_download)
        .def("start_mission", &mavsdk::MissionRaw::start_mission)
        .def("pause_mission", &mavsdk::MissionRaw::pause_mission)
        .def("clear_mission", &mavsdk::MissionRaw::clear_mission)
        .def("set_current_mission_item", &mavsdk::MissionRaw::set_current_mission_item)
        .def("mission_progress", &mavsdk::MissionRaw::mission_progress)

        ;

    py::class_<mavsdk::MissionRaw::MissionProgress> mission_progress(
        mission_raw, "MissionProgress");
    mission_progress.def(py::init<>())
        .def_readwrite("current", &mavsdk::MissionRaw::MissionProgress::current)
        .def_readwrite("total", &mavsdk::MissionRaw::MissionProgress::total);

    py::class_<mavsdk::MissionRaw::MissionItem> mission_item(mission_raw, "MissionItem");
    mission_item.def(py::init<>())
        .def_readwrite("seq", &mavsdk::MissionRaw::MissionItem::seq)
        .def_readwrite("frame", &mavsdk::MissionRaw::MissionItem::frame)
        .def_readwrite("command", &mavsdk::MissionRaw::MissionItem::command)
        .def_readwrite("current", &mavsdk::MissionRaw::MissionItem::current)
        .def_readwrite("autocontinue", &mavsdk::MissionRaw::MissionItem::autocontinue)
        .def_readwrite("param1", &mavsdk::MissionRaw::MissionItem::param1)
        .def_readwrite("param2", &mavsdk::MissionRaw::MissionItem::param2)
        .def_readwrite("param3", &mavsdk::MissionRaw::MissionItem::param3)
        .def_readwrite("param4", &mavsdk::MissionRaw::MissionItem::param4)
        .def_readwrite("x", &mavsdk::MissionRaw::MissionItem::x)
        .def_readwrite("y", &mavsdk::MissionRaw::MissionItem::y)
        .def_readwrite("z", &mavsdk::MissionRaw::MissionItem::z)
        .def_readwrite("mission_type", &mavsdk::MissionRaw::MissionItem::mission_type);

    py::enum_<mavsdk::MissionRaw::Result>(mission_raw, "Result")
        .value("Unknown", mavsdk::MissionRaw::Result::Unknown)
        .value("Success", mavsdk::MissionRaw::Result::Success)
        .value("Error", mavsdk::MissionRaw::Result::Error)
        .value("TooManyMissionItems", mavsdk::MissionRaw::Result::TooManyMissionItems)
        .value("Busy", mavsdk::MissionRaw::Result::Busy)
        .value("Timeout", mavsdk::MissionRaw::Result::Timeout)
        .value("InvalidArgument", mavsdk::MissionRaw::Result::InvalidArgument)
        .value("Unsupported", mavsdk::MissionRaw::Result::Unsupported)
        .value("NoMissionAvailable", mavsdk::MissionRaw::Result::NoMissionAvailable)
        .value("TransferCancelled", mavsdk::MissionRaw::Result::TransferCancelled);

    system.def(
        "mission_raw",
        &mavsdk::System::mission_raw,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}