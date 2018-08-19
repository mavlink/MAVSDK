#include "dronecode_sdk.h"
#include "plugins/action/include/plugins/action/action.h"

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

namespace dronecode_sdk {

namespace py = pybind11;

PYBIND11_MODULE(pydronecode_sdk, m)
{
    m.doc() = "pybind11 Dronecode SDK plugin";

    py::class_<DronecodeSDK>(m, "DronecodeSDK")
        .def(py::init<>())
        .def("register_on_discover", &DronecodeSDK::register_on_discover)
        .def("add_udp_connection",
             (ConnectionResult(DronecodeSDK::*)(const std::string &, int)) &
                 DronecodeSDK::add_udp_connection,
             "Connect to UDP and specify URI and port")
        .def("add_udp_connection",
             (ConnectionResult(DronecodeSDK::*)(int)) & DronecodeSDK::add_udp_connection,
             "Connect to UDP and specify port")

        .def("system",
             (System & (DronecodeSDK::*)() const) & DronecodeSDK::system,
             py::return_value_policy::reference)
        .def("system",
             (System & (DronecodeSDK::*)(uint64_t) const) & DronecodeSDK::system,
             py::return_value_policy::reference);

    py::enum_<ConnectionResult>(m, "ConnectionResult")
        .value("SUCCESS", ConnectionResult::SUCCESS)
        .value("TIMEOUT", ConnectionResult::TIMEOUT)
        .value("SOCKET_ERROR", ConnectionResult::SOCKET_ERROR)
        .value("BIND_ERROR", ConnectionResult::BIND_ERROR)
        .value("SOCKET_CONNECTION_ERROR", ConnectionResult::SOCKET_CONNECTION_ERROR)
        .value("CONNECTION_ERROR", ConnectionResult::CONNECTION_ERROR)
        .value("NOT_IMPLEMENTED", ConnectionResult::NOT_IMPLEMENTED)
        .value("SYSTEM_NOT_CONNECTED", ConnectionResult::SYSTEM_NOT_CONNECTED)
        .value("SYSTEM_BUSY", ConnectionResult::SYSTEM_BUSY)
        .value("COMMAND_DENIED", ConnectionResult::COMMAND_DENIED)
        .value("DESTINATION_IP_UNKNOWN", ConnectionResult::DESTINATION_IP_UNKNOWN)
        .value("CONNECTIONS_EXHAUSTED", ConnectionResult::CONNECTIONS_EXHAUSTED)
        .value("CONNECTION_URL_INVALID", ConnectionResult::CONNECTION_URL_INVALID)
        .export_values();

    py::class_<System>(m, "System");

    py::class_<PluginBase> plugin_base(m, "PluginBase");

    py::class_<Action>(m, "Action", plugin_base)
        .def(py::init<System &>())
        .def("arm", &Action::arm)
        .def("arm_async", &Action::arm_async)
        .def("disarm", &Action::disarm)
        .def("disarm_async", &Action::disarm_async)
        .def("takeoff", &Action::takeoff)
        .def("takeoff_async", &Action::takeoff_async)
        .def("land", &Action::land)
        .def("land_async", &Action::land_async);

    py::enum_<ActionResult>(m, "Result")
        .value("UNKNOWN", ActionResult::UNKNOWN)
        .value("SUCCESS", ActionResult::SUCCESS)
        .value("NO_SYSTEM", ActionResult::NO_SYSTEM)
        .value("CONNECTION_ERROR", ActionResult::CONNECTION_ERROR)
        .value("BUSY", ActionResult::BUSY)
        .value("COMMAND_DENIED", ActionResult::COMMAND_DENIED)
        .value("COMMAND_DENIED_LANDED_STATE_UNKNOWN",
               ActionResult::COMMAND_DENIED_LANDED_STATE_UNKNOWN)
        .value("COMMAND_DENIED_NOT_LANDED", ActionResult::COMMAND_DENIED_NOT_LANDED)
        .value("TIMEOUT", ActionResult::TIMEOUT)
        .value("VTOL_TRANSITION_SUPPORT_UNKNOWN", ActionResult::VTOL_TRANSITION_SUPPORT_UNKNOWN)
        .value("NO_VTOL_TRANSITION_SUPPORT", ActionResult::NO_VTOL_TRANSITION_SUPPORT)
        .value("PARAMETER_ERROR", ActionResult::PARAMETER_ERROR)
        .export_values();
}

} // namespace dronecode_sdk
