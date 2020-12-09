#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/calibration/calibration.h>

namespace py = pybind11;
void init_calibration(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::Calibration> calibration(m, "Calibration");

    calibration

        .def("cancel", &mavsdk::Calibration::cancel);

    py::enum_<mavsdk::Calibration::Result>(calibration, "Result")
        .value("Unknown", mavsdk::Calibration::Result::Unknown)
        .value("Success", mavsdk::Calibration::Result::Success)
        .value("Next", mavsdk::Calibration::Result::Next)
        .value("Failed", mavsdk::Calibration::Result::Failed)
        .value("NoSystem", mavsdk::Calibration::Result::NoSystem)
        .value("ConnectionError", mavsdk::Calibration::Result::ConnectionError)
        .value("Busy", mavsdk::Calibration::Result::Busy)
        .value("CommandDenied", mavsdk::Calibration::Result::CommandDenied)
        .value("Timeout", mavsdk::Calibration::Result::Timeout)
        .value("Cancelled", mavsdk::Calibration::Result::Cancelled)
        .value("FailedArmed", mavsdk::Calibration::Result::FailedArmed);

    py::class_<mavsdk::Calibration::ProgressData> progress_data(calibration, "ProgressData");
    progress_data.def(py::init<>())
        .def_readwrite("has_progress", &mavsdk::Calibration::ProgressData::has_progress)
        .def_readwrite("progress", &mavsdk::Calibration::ProgressData::progress)
        .def_readwrite("has_status_text", &mavsdk::Calibration::ProgressData::has_status_text)
        .def_readwrite("status_text", &mavsdk::Calibration::ProgressData::status_text);

    system.def(
        "calibration",
        &mavsdk::System::calibration,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}