#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/offboard/offboard.h>

namespace py = pybind11;
void init_offboard(py::module_ &m) {
    py::class_<mavsdk::Offboard> offboard(m, "Offboard");


offboard.def(py::init<std::shared_ptr<mavsdk::System>>())
    .def("start", &mavsdk::Offboard::start)
    .def("stop", &mavsdk::Offboard::stop)

    .def("set_attitude", &mavsdk::Offboard::set_attitude)
    .def("set_actuator_control", &mavsdk::Offboard::set_actuator_control)
    .def("set_attitude_rate", &mavsdk::Offboard::set_attitude_rate)
    .def("set_position_ned", &mavsdk::Offboard::set_position_ned)
    .def("set_velocity_body", &mavsdk::Offboard::set_velocity_body)
    .def("set_velocity_ned", &mavsdk::Offboard::set_velocity_ned)
    .def("set_position_velocity_ned", &mavsdk::Offboard::set_position_velocity_ned)
;





py::class_<mavsdk::Offboard::Attitude> attitude(offboard, "Attitude");
attitude
    .def(py::init<>())
    .def_readwrite("roll_deg", &mavsdk::Offboard::Attitude::roll_deg)
    .def_readwrite("pitch_deg", &mavsdk::Offboard::Attitude::pitch_deg)
    .def_readwrite("yaw_deg", &mavsdk::Offboard::Attitude::yaw_deg)
    .def_readwrite("thrust_value", &mavsdk::Offboard::Attitude::thrust_value);




py::class_<mavsdk::Offboard::ActuatorControlGroup> actuator_control_group(offboard, "ActuatorControlGroup");
actuator_control_group
    .def(py::init<>())
    .def_readwrite("controls", &mavsdk::Offboard::ActuatorControlGroup::controls);




py::class_<mavsdk::Offboard::ActuatorControl> actuator_control(offboard, "ActuatorControl");
actuator_control
    .def(py::init<>())
    .def_readwrite("groups", &mavsdk::Offboard::ActuatorControl::groups);




py::class_<mavsdk::Offboard::AttitudeRate> attitude_rate(offboard, "AttitudeRate");
attitude_rate
    .def(py::init<>())
    .def_readwrite("roll_deg_s", &mavsdk::Offboard::AttitudeRate::roll_deg_s)
    .def_readwrite("pitch_deg_s", &mavsdk::Offboard::AttitudeRate::pitch_deg_s)
    .def_readwrite("yaw_deg_s", &mavsdk::Offboard::AttitudeRate::yaw_deg_s)
    .def_readwrite("thrust_value", &mavsdk::Offboard::AttitudeRate::thrust_value);




py::class_<mavsdk::Offboard::PositionNedYaw> position_ned_yaw(offboard, "PositionNedYaw");
position_ned_yaw
    .def(py::init<>())
    .def_readwrite("north_m", &mavsdk::Offboard::PositionNedYaw::north_m)
    .def_readwrite("east_m", &mavsdk::Offboard::PositionNedYaw::east_m)
    .def_readwrite("down_m", &mavsdk::Offboard::PositionNedYaw::down_m)
    .def_readwrite("yaw_deg", &mavsdk::Offboard::PositionNedYaw::yaw_deg);




py::class_<mavsdk::Offboard::VelocityBodyYawspeed> velocity_body_yawspeed(offboard, "VelocityBodyYawspeed");
velocity_body_yawspeed
    .def(py::init<>())
    .def_readwrite("forward_m_s", &mavsdk::Offboard::VelocityBodyYawspeed::forward_m_s)
    .def_readwrite("right_m_s", &mavsdk::Offboard::VelocityBodyYawspeed::right_m_s)
    .def_readwrite("down_m_s", &mavsdk::Offboard::VelocityBodyYawspeed::down_m_s)
    .def_readwrite("yawspeed_deg_s", &mavsdk::Offboard::VelocityBodyYawspeed::yawspeed_deg_s);




py::class_<mavsdk::Offboard::VelocityNedYaw> velocity_ned_yaw(offboard, "VelocityNedYaw");
velocity_ned_yaw
    .def(py::init<>())
    .def_readwrite("north_m_s", &mavsdk::Offboard::VelocityNedYaw::north_m_s)
    .def_readwrite("east_m_s", &mavsdk::Offboard::VelocityNedYaw::east_m_s)
    .def_readwrite("down_m_s", &mavsdk::Offboard::VelocityNedYaw::down_m_s)
    .def_readwrite("yaw_deg", &mavsdk::Offboard::VelocityNedYaw::yaw_deg);





py::enum_<mavsdk::Offboard::Result>(offboard, "Result")
    .value("Unknown", mavsdk::Offboard::Result::Unknown)
    .value("Success", mavsdk::Offboard::Result::Success)
    .value("NoSystem", mavsdk::Offboard::Result::NoSystem)
    .value("ConnectionError", mavsdk::Offboard::Result::ConnectionError)
    .value("Busy", mavsdk::Offboard::Result::Busy)
    .value("CommandDenied", mavsdk::Offboard::Result::CommandDenied)
    .value("Timeout", mavsdk::Offboard::Result::Timeout)
    .value("NoSetpointSet", mavsdk::Offboard::Result::NoSetpointSet);


}