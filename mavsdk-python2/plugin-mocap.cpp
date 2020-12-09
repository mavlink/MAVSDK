#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mocap/mocap.h>

namespace py = pybind11;
void init_mocap(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::Mocap> mocap(m, "Mocap");

    mocap.def("set_vision_position_estimate", &mavsdk::Mocap::set_vision_position_estimate)
        .def("set_attitude_position_mocap", &mavsdk::Mocap::set_attitude_position_mocap)
        .def("set_odometry", &mavsdk::Mocap::set_odometry);

    py::class_<mavsdk::Mocap::PositionBody> position_body(mocap, "PositionBody");
    position_body.def(py::init<>())
        .def_readwrite("x_m", &mavsdk::Mocap::PositionBody::x_m)
        .def_readwrite("y_m", &mavsdk::Mocap::PositionBody::y_m)
        .def_readwrite("z_m", &mavsdk::Mocap::PositionBody::z_m);

    py::class_<mavsdk::Mocap::AngleBody> angle_body(mocap, "AngleBody");
    angle_body.def(py::init<>())
        .def_readwrite("roll_rad", &mavsdk::Mocap::AngleBody::roll_rad)
        .def_readwrite("pitch_rad", &mavsdk::Mocap::AngleBody::pitch_rad)
        .def_readwrite("yaw_rad", &mavsdk::Mocap::AngleBody::yaw_rad);

    py::class_<mavsdk::Mocap::SpeedBody> speed_body(mocap, "SpeedBody");
    speed_body.def(py::init<>())
        .def_readwrite("x_m_s", &mavsdk::Mocap::SpeedBody::x_m_s)
        .def_readwrite("y_m_s", &mavsdk::Mocap::SpeedBody::y_m_s)
        .def_readwrite("z_m_s", &mavsdk::Mocap::SpeedBody::z_m_s);

    py::class_<mavsdk::Mocap::AngularVelocityBody> angular_velocity_body(
        mocap, "AngularVelocityBody");
    angular_velocity_body.def(py::init<>())
        .def_readwrite("roll_rad_s", &mavsdk::Mocap::AngularVelocityBody::roll_rad_s)
        .def_readwrite("pitch_rad_s", &mavsdk::Mocap::AngularVelocityBody::pitch_rad_s)
        .def_readwrite("yaw_rad_s", &mavsdk::Mocap::AngularVelocityBody::yaw_rad_s);

    py::class_<mavsdk::Mocap::Covariance> covariance(mocap, "Covariance");
    covariance.def(py::init<>())
        .def_readwrite("covariance_matrix", &mavsdk::Mocap::Covariance::covariance_matrix);

    py::class_<mavsdk::Mocap::Quaternion> quaternion(mocap, "Quaternion");
    quaternion.def(py::init<>())
        .def_readwrite("w", &mavsdk::Mocap::Quaternion::w)
        .def_readwrite("x", &mavsdk::Mocap::Quaternion::x)
        .def_readwrite("y", &mavsdk::Mocap::Quaternion::y)
        .def_readwrite("z", &mavsdk::Mocap::Quaternion::z);

    py::class_<mavsdk::Mocap::VisionPositionEstimate> vision_position_estimate(
        mocap, "VisionPositionEstimate");
    vision_position_estimate.def(py::init<>())
        .def_readwrite("time_usec", &mavsdk::Mocap::VisionPositionEstimate::time_usec)
        .def_readwrite("position_body", &mavsdk::Mocap::VisionPositionEstimate::position_body)
        .def_readwrite("angle_body", &mavsdk::Mocap::VisionPositionEstimate::angle_body)
        .def_readwrite("pose_covariance", &mavsdk::Mocap::VisionPositionEstimate::pose_covariance);

    py::class_<mavsdk::Mocap::AttitudePositionMocap> attitude_position_mocap(
        mocap, "AttitudePositionMocap");
    attitude_position_mocap.def(py::init<>())
        .def_readwrite("time_usec", &mavsdk::Mocap::AttitudePositionMocap::time_usec)
        .def_readwrite("q", &mavsdk::Mocap::AttitudePositionMocap::q)
        .def_readwrite("position_body", &mavsdk::Mocap::AttitudePositionMocap::position_body)
        .def_readwrite("pose_covariance", &mavsdk::Mocap::AttitudePositionMocap::pose_covariance);

    py::class_<mavsdk::Mocap::Odometry> odometry(mocap, "Odometry");
    odometry.def(py::init<>())
        .def_readwrite("time_usec", &mavsdk::Mocap::Odometry::time_usec)
        .def_readwrite("frame_id", &mavsdk::Mocap::Odometry::frame_id)
        .def_readwrite("position_body", &mavsdk::Mocap::Odometry::position_body)
        .def_readwrite("q", &mavsdk::Mocap::Odometry::q)
        .def_readwrite("speed_body", &mavsdk::Mocap::Odometry::speed_body)
        .def_readwrite("angular_velocity_body", &mavsdk::Mocap::Odometry::angular_velocity_body)
        .def_readwrite("pose_covariance", &mavsdk::Mocap::Odometry::pose_covariance)
        .def_readwrite("velocity_covariance", &mavsdk::Mocap::Odometry::velocity_covariance);

    py::enum_<mavsdk::Mocap::Odometry::MavFrame>(odometry, "MavFrame")
        .value("MocapNed", mavsdk::Mocap::Odometry::MavFrame::MocapNed)
        .value("LocalFrd", mavsdk::Mocap::Odometry::MavFrame::LocalFrd);

    py::enum_<mavsdk::Mocap::Result>(mocap, "Result")
        .value("Unknown", mavsdk::Mocap::Result::Unknown)
        .value("Success", mavsdk::Mocap::Result::Success)
        .value("NoSystem", mavsdk::Mocap::Result::NoSystem)
        .value("ConnectionError", mavsdk::Mocap::Result::ConnectionError)
        .value("InvalidRequestData", mavsdk::Mocap::Result::InvalidRequestData);

    system.def(
        "mocap",
        &mavsdk::System::mocap,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}