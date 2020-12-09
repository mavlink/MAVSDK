#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>

namespace py = pybind11;

void init_action(py::module_&, py::class_<mavsdk::System>&);
void init_calibration(py::module_&, py::class_<mavsdk::System>&);
void init_camera(py::module_&, py::class_<mavsdk::System>&);
void init_failure(py::module_&, py::class_<mavsdk::System>&);
void init_follow_me(py::module_&, py::class_<mavsdk::System>&);
void init_ftp(py::module_&, py::class_<mavsdk::System>&);
void init_geofence(py::module_&, py::class_<mavsdk::System>&);
void init_gimbal(py::module_&, py::class_<mavsdk::System>&);
void init_info(py::module_&, py::class_<mavsdk::System>&);
void init_log_files(py::module_&, py::class_<mavsdk::System>&);
void init_manual_control(py::module_&, py::class_<mavsdk::System>&);
void init_mission(py::module_&, py::class_<mavsdk::System>&);
void init_mission_raw(py::module_&, py::class_<mavsdk::System>&);
void init_mocap(py::module_&, py::class_<mavsdk::System>&);
void init_offboard(py::module_&, py::class_<mavsdk::System>&);
void init_param(py::module_&, py::class_<mavsdk::System>&);
void init_shell(py::module_&, py::class_<mavsdk::System>&);
void init_telemetry(py::module_&, py::class_<mavsdk::System>&);
void init_tune(py::module_&, py::class_<mavsdk::System>&);

PYBIND11_MODULE(mavsdk2, m)
{
    py::class_<mavsdk::Mavsdk>(m, "Mavsdk")
        .def(py::init<>())
        .def("version", &mavsdk::Mavsdk::version)
        .def("add_any_connection", &mavsdk::Mavsdk::add_any_connection)
        .def("systems", &mavsdk::Mavsdk::systems, py::keep_alive<1, 0>());

    py::enum_<mavsdk::ConnectionResult>(m, "ConnectionResult")
        .value("Success", mavsdk::ConnectionResult::Success)
        .value("Timeout", mavsdk::ConnectionResult::Timeout)
        .value("SocketError", mavsdk::ConnectionResult::SocketError)
        .value("BindError", mavsdk::ConnectionResult::BindError)
        .value("SocketConnectionError", mavsdk::ConnectionResult::SocketConnectionError)
        .value("ConnectionError", mavsdk::ConnectionResult::ConnectionError)
        .value("NotImplemented", mavsdk::ConnectionResult::NotImplemented)
        .value("SystemNotConnected", mavsdk::ConnectionResult::SystemNotConnected)
        .value("SystemBusy", mavsdk::ConnectionResult::SystemBusy)
        .value("CommandDenied", mavsdk::ConnectionResult::CommandDenied)
        .value("DestinationIpUnknown", mavsdk::ConnectionResult::DestinationIpUnknown)
        .value("ConnectionsExhausted", mavsdk::ConnectionResult::ConnectionsExhausted)
        .value("ConnectionUrlInvalid", mavsdk::ConnectionResult::ConnectionUrlInvalid)
        .value("BaudrateUnknown", mavsdk::ConnectionResult::BaudrateUnknown);

    py::class_<mavsdk::System> system(m, "System");

    init_action(m, system);
    init_calibration(m, system);
    init_camera(m, system);
    init_failure(m, system);
    init_follow_me(m, system);
    init_ftp(m, system);
    init_geofence(m, system);
    init_gimbal(m, system);
    init_info(m, system);
    init_log_files(m, system);
    init_manual_control(m, system);
    init_mission(m, system);
    init_mission_raw(m, system);
    init_mocap(m, system);
    init_offboard(m, system);
    init_param(m, system);
    init_shell(m, system);
    init_telemetry(m, system);
    init_tune(m, system);
}
