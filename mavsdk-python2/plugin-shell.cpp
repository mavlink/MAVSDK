#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/shell/shell.h>

namespace py = pybind11;
void init_shell(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::Shell> shell(m, "Shell");

    shell.def("send", &mavsdk::Shell::send)

        ;

    py::enum_<mavsdk::Shell::Result>(shell, "Result")
        .value("Unknown", mavsdk::Shell::Result::Unknown)
        .value("Success", mavsdk::Shell::Result::Success)
        .value("NoSystem", mavsdk::Shell::Result::NoSystem)
        .value("ConnectionError", mavsdk::Shell::Result::ConnectionError)
        .value("NoResponse", mavsdk::Shell::Result::NoResponse)
        .value("Busy", mavsdk::Shell::Result::Busy);

    system.def(
        "shell",
        &mavsdk::System::shell,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}