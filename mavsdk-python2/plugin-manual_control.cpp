#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/manual_control/manual_control.h>

namespace py = pybind11;
void init_manual_control(py::module_ &m) {
    py::class_<mavsdk::ManualControl> manual_control(m, "ManualControl");


manual_control.def(py::init<std::shared_ptr<mavsdk::System>>())
    .def("start_position_control", &mavsdk::ManualControl::start_position_control)
    .def("start_altitude_control", &mavsdk::ManualControl::start_altitude_control)
    .def("set_manual_control_input", &mavsdk::ManualControl::set_manual_control_input)
;






py::enum_<mavsdk::ManualControl::Result>(manual_control, "Result")
    .value("Unknown", mavsdk::ManualControl::Result::Unknown)
    .value("Success", mavsdk::ManualControl::Result::Success)
    .value("NoSystem", mavsdk::ManualControl::Result::NoSystem)
    .value("ConnectionError", mavsdk::ManualControl::Result::ConnectionError)
    .value("Busy", mavsdk::ManualControl::Result::Busy)
    .value("CommandDenied", mavsdk::ManualControl::Result::CommandDenied)
    .value("Timeout", mavsdk::ManualControl::Result::Timeout)
    .value("InputOutOfRange", mavsdk::ManualControl::Result::InputOutOfRange)
    .value("InputNotSet", mavsdk::ManualControl::Result::InputNotSet);


}