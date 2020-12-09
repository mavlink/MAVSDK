#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/log_files/log_files.h>

namespace py = pybind11;
void init_log_files(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::LogFiles> log_files(m, "LogFiles");

    log_files

        ;

    py::class_<mavsdk::LogFiles::ProgressData> progress_data(log_files, "ProgressData");
    progress_data.def(py::init<>())
        .def_readwrite("progress", &mavsdk::LogFiles::ProgressData::progress);

    py::class_<mavsdk::LogFiles::Entry> entry(log_files, "Entry");
    entry.def(py::init<>())
        .def_readwrite("id", &mavsdk::LogFiles::Entry::id)
        .def_readwrite("date", &mavsdk::LogFiles::Entry::date)
        .def_readwrite("size_bytes", &mavsdk::LogFiles::Entry::size_bytes);

    py::enum_<mavsdk::LogFiles::Result>(log_files, "Result")
        .value("Unknown", mavsdk::LogFiles::Result::Unknown)
        .value("Success", mavsdk::LogFiles::Result::Success)
        .value("Next", mavsdk::LogFiles::Result::Next)
        .value("NoLogfiles", mavsdk::LogFiles::Result::NoLogfiles)
        .value("Timeout", mavsdk::LogFiles::Result::Timeout)
        .value("InvalidArgument", mavsdk::LogFiles::Result::InvalidArgument)
        .value("FileOpenFailed", mavsdk::LogFiles::Result::FileOpenFailed);

    system.def(
        "log_files",
        &mavsdk::System::log_files,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}