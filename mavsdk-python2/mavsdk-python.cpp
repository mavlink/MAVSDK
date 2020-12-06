#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>

namespace py = pybind11;

void init_action(py::module_ &);
void init_calibration(py::module_ &);
void init_camera(py::module_ &);
void init_failure(py::module_ &);
void init_follow_me(py::module_ &);
void init_ftp(py::module_ &);
void init_geofence(py::module_ &);
void init_gimbal(py::module_ &);
void init_info(py::module_ &);
void init_log_files(py::module_ &);
void init_manual_control(py::module_ &);
void init_mission(py::module_ &);
void init_mission_raw(py::module_ &);
void init_mocap(py::module_ &);
void init_offboard(py::module_ &);
void init_param(py::module_ &);
void init_shell(py::module_ &);
void init_telemetry(py::module_ &);
void init_tune(py::module_ &);

PYBIND11_MODULE(mavsdk2, m) {
    py::class_<mavsdk::Mavsdk>(m, "Mavsdk")
        .def(py::init<>())
        .def("version", &mavsdk::Mavsdk::version)
        .def("add_any_connection", &mavsdk::Mavsdk::add_any_connection)
        .def("systems", &mavsdk::Mavsdk::systems, py::return_value_policy::reference);

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

    py::class_<mavsdk::System, std::shared_ptr<mavsdk::System>>(m, "System");

    init_action(m);
    init_calibration(m);
    init_camera(m);
    init_failure(m);
    init_follow_me(m);
    init_ftp(m);
    init_geofence(m);
    init_gimbal(m);
    init_info(m);
    init_log_files(m);
    init_manual_control(m);
    init_mission(m);
    init_mission_raw(m);
    init_mocap(m);
    init_offboard(m);
    init_param(m);
    init_shell(m);
    init_telemetry(m);
    init_tune(m);
}
