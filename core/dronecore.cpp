#include "dronecore.h"
#include "dronecore_impl.h"
#include "global_include.h"
#include "connection.h"
#include "udp_connection.h"

#ifdef PYBIND11
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#endif

namespace dronecore {

DroneCore::DroneCore() :
    _impl(nullptr)
{
    _impl = new DroneCoreImpl();
}

DroneCore::~DroneCore()
{
    delete _impl;
    _impl = nullptr;
}

DroneCore::ConnectionResult DroneCore::add_udp_connection()
{
    return add_udp_connection(0);
}

DroneCore::ConnectionResult DroneCore::add_udp_connection(int local_port_number)
{
    Connection *new_connection = new UdpConnection(_impl, local_port_number);
    DroneCore::ConnectionResult ret = new_connection->start();

    if (ret != DroneCore::ConnectionResult::SUCCESS) {
        delete new_connection;
        return ret;
    }

    _impl->add_connection(new_connection);
    return DroneCore::ConnectionResult::SUCCESS;
}

const std::vector<uint64_t> &DroneCore::device_uuids() const
{
    return _impl->get_device_uuids();
}

Device &DroneCore::device() const
{
    return _impl->get_device();
}

Device &DroneCore::device(uint64_t uuid) const
{
    return _impl->get_device(uuid);
}

void DroneCore::register_on_discover(event_callback_t callback)
{
    _impl->register_on_discover(callback);
}

void DroneCore::register_on_timeout(event_callback_t callback)
{
    _impl->register_on_timeout(callback);
}

const char *DroneCore::connection_result_str(ConnectionResult result)
{
    switch (result) {
        case ConnectionResult::SUCCESS:
            return "Success";
        case ConnectionResult::TIMEOUT:
            return "Timeout";
        case ConnectionResult::SOCKET_ERROR:
            return "Socket error";
        case ConnectionResult::BIND_ERROR:
            return "Bind error";
        case ConnectionResult::CONNECTION_ERROR:
            return "Connection error";
        case ConnectionResult::NOT_IMPLEMENTED:
            return "Not implemented";
        case ConnectionResult::DEVICE_NOT_CONNECTED:
            return "Device not connected";
        case ConnectionResult::DEVICE_BUSY:
            return "Device busy";
        case ConnectionResult::COMMAND_DENIED:
            return "Command denied";
        case ConnectionResult::DESTINATION_IP_UNKNOWN:
            return "Destination IP unknown";
        case ConnectionResult::CONNECTIONS_EXHAUSTED:
            return "Connections exhausted";
        default:
            return "Unknown";
    }
}

#ifdef PYBIND11

namespace py = pybind11;

PYBIND11_PLUGIN(pydronecore) {
    py::module m("pydronecore", "pybind11 dronecore plugin");

    py::class_<DroneCore>(m, "DroneCore")
        .def(py::init<>())
        .def("register_on_discover", &DroneCore::register_on_discover)
        .def("add_udp_connection", (DroneCore::ConnectionResult (DroneCore::*)()) &DroneCore::add_udp_connection)
        .def("add_udp_connection", (DroneCore::ConnectionResult (DroneCore::*)(int)) &DroneCore::add_udp_connection)
        .def("device", (Device& (DroneCore::*)() const) &DroneCore::device, py::return_value_policy::reference)
        .def("device", (Device& (DroneCore::*)(uint64_t) const) &DroneCore::device, py::return_value_policy::reference);

    py::enum_<DroneCore::ConnectionResult>(m, "ConnectionResult")
        .value("SUCCESS", DroneCore::ConnectionResult::SUCCESS)
        .value("TIMEOUT", DroneCore::ConnectionResult::TIMEOUT)
        .value("SOCKET_ERROR", DroneCore::ConnectionResult::SOCKET_ERROR)
        .value("BIND_ERROR", DroneCore::ConnectionResult::BIND_ERROR)
        .value("CONNECTION_ERROR", DroneCore::ConnectionResult::CONNECTION_ERROR)
        .value("NOT_IMPLEMENTED", DroneCore::ConnectionResult::NOT_IMPLEMENTED)
        .value("DEVICE_NOT_CONNECTED", DroneCore::ConnectionResult::DEVICE_NOT_CONNECTED)
        .value("DEVICE_BUSY", DroneCore::ConnectionResult::DEVICE_BUSY)
        .value("COMMAND_DENIED", DroneCore::ConnectionResult::COMMAND_DENIED)
        .value("DESTINATION_IP_UNKNOWN", DroneCore::ConnectionResult::DESTINATION_IP_UNKNOWN)
        .value("CONNECTIONS_EXHAUSTED", DroneCore::ConnectionResult::CONNECTIONS_EXHAUSTED)
        .export_values();

    py::class_<DevicePluginContainer>(m, "DevicePluginContainer")
        .def("action", &DevicePluginContainer::action, py::return_value_policy::reference);

    py::class_<Device, DevicePluginContainer>(m, "Device");

    py::class_<Action> (m, "Action")
        .def("arm", &Action::arm)
        .def("arm_async", &Action::arm_async)
        .def("disarm", &Action::disarm)
        .def("disarm_async", &Action::disarm_async)
        .def("takeoff", &Action::takeoff)
        .def("takeoff_async", &Action::takeoff_async)
        .def("land", &Action::land)
        .def("land_async", &Action::land_async);

    py::enum_<Action::Result>(m, "Result")
        .value("SUCCESS", Action::Result::SUCCESS)
        .value("NO_DEVICE", Action::Result::NO_DEVICE)
        .value("CONNECTION_ERROR", Action::Result::CONNECTION_ERROR)
        .value("BUSY", Action::Result::BUSY)
        .value("COMMAND_DENIED", Action::Result::COMMAND_DENIED)
        .value("COMMAND_DENIED_LANDED_STATE_UNKNOWN", Action::Result::COMMAND_DENIED_LANDED_STATE_UNKNOWN)
        .value("COMMAND_DENIED_NOT_LANDED", Action::Result::COMMAND_DENIED_NOT_LANDED)
        .value("TIMEOUT", Action::Result::TIMEOUT)
        .value("UNKNOWN", Action::Result::UNKNOWN)
        .export_values();

    return m.ptr();
}
#endif

} // namespace dronecore

