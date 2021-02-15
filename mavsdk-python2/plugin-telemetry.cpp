#include <pybind11/stl.h>
#include <pybind11/pybind11.h>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

namespace py = pybind11;
void init_telemetry(py::module_& m, py::class_<mavsdk::System>& system)
{
    py::class_<mavsdk::Telemetry> telemetry(m, "Telemetry");

    telemetry.def("position", &mavsdk::Telemetry::position)
        .def("home", &mavsdk::Telemetry::home)
        .def("in_air", &mavsdk::Telemetry::in_air)
        .def("landed_state", &mavsdk::Telemetry::landed_state)
        .def("armed", &mavsdk::Telemetry::armed)
        .def("attitude_quaternion", &mavsdk::Telemetry::attitude_quaternion)
        .def("attitude_euler", &mavsdk::Telemetry::attitude_euler)
        .def("attitude_angular_velocity_body", &mavsdk::Telemetry::attitude_angular_velocity_body)
        .def("camera_attitude_quaternion", &mavsdk::Telemetry::camera_attitude_quaternion)
        .def("camera_attitude_euler", &mavsdk::Telemetry::camera_attitude_euler)
        .def("velocity_ned", &mavsdk::Telemetry::velocity_ned)
        .def("gps_info", &mavsdk::Telemetry::gps_info)
        .def("battery", &mavsdk::Telemetry::battery)
        .def("flight_mode", &mavsdk::Telemetry::flight_mode)
        .def("health", &mavsdk::Telemetry::health)
        .def("rc_status", &mavsdk::Telemetry::rc_status)
        .def("status_text", &mavsdk::Telemetry::status_text)
        .def("actuator_control_target", &mavsdk::Telemetry::actuator_control_target)
        .def("actuator_output_status", &mavsdk::Telemetry::actuator_output_status)
        .def("odometry", &mavsdk::Telemetry::odometry)
        .def("position_velocity_ned", &mavsdk::Telemetry::position_velocity_ned)
        .def("ground_truth", &mavsdk::Telemetry::ground_truth)
        .def("fixedwing_metrics", &mavsdk::Telemetry::fixedwing_metrics)
        .def("imu", &mavsdk::Telemetry::imu)
        .def("health_all_ok", &mavsdk::Telemetry::health_all_ok)
        .def("unix_epoch_time", &mavsdk::Telemetry::unix_epoch_time)
        .def("distance_sensor", &mavsdk::Telemetry::distance_sensor)
        .def("set_rate_position", &mavsdk::Telemetry::set_rate_position)
        .def("set_rate_home", &mavsdk::Telemetry::set_rate_home)
        .def("set_rate_in_air", &mavsdk::Telemetry::set_rate_in_air)
        .def("set_rate_landed_state", &mavsdk::Telemetry::set_rate_landed_state)
        .def("set_rate_attitude", &mavsdk::Telemetry::set_rate_attitude)
        .def("set_rate_camera_attitude", &mavsdk::Telemetry::set_rate_camera_attitude)
        .def("set_rate_velocity_ned", &mavsdk::Telemetry::set_rate_velocity_ned)
        .def("set_rate_gps_info", &mavsdk::Telemetry::set_rate_gps_info)
        .def("set_rate_battery", &mavsdk::Telemetry::set_rate_battery)
        .def("set_rate_rc_status", &mavsdk::Telemetry::set_rate_rc_status)
        .def(
            "set_rate_actuator_control_target",
            &mavsdk::Telemetry::set_rate_actuator_control_target)
        .def("set_rate_actuator_output_status", &mavsdk::Telemetry::set_rate_actuator_output_status)
        .def("set_rate_odometry", &mavsdk::Telemetry::set_rate_odometry)
        .def("set_rate_position_velocity_ned", &mavsdk::Telemetry::set_rate_position_velocity_ned)
        .def("set_rate_ground_truth", &mavsdk::Telemetry::set_rate_ground_truth)
        .def("set_rate_fixedwing_metrics", &mavsdk::Telemetry::set_rate_fixedwing_metrics)
        .def("set_rate_imu", &mavsdk::Telemetry::set_rate_imu)
        .def("set_rate_unix_epoch_time", &mavsdk::Telemetry::set_rate_unix_epoch_time)
        .def("set_rate_distance_sensor", &mavsdk::Telemetry::set_rate_distance_sensor)

        ;

    py::enum_<mavsdk::Telemetry::FixType>(telemetry, "FixType")
        .value("NoGps", mavsdk::Telemetry::FixType::NoGps)
        .value("NoFix", mavsdk::Telemetry::FixType::NoFix)
        .value("Fix2D", mavsdk::Telemetry::FixType::Fix2D)
        .value("Fix3D", mavsdk::Telemetry::FixType::Fix3D)
        .value("FixDgps", mavsdk::Telemetry::FixType::FixDgps)
        .value("RtkFloat", mavsdk::Telemetry::FixType::RtkFloat)
        .value("RtkFixed", mavsdk::Telemetry::FixType::RtkFixed);

    py::enum_<mavsdk::Telemetry::FlightMode>(telemetry, "FlightMode")
        .value("Unknown", mavsdk::Telemetry::FlightMode::Unknown)
        .value("Ready", mavsdk::Telemetry::FlightMode::Ready)
        .value("Takeoff", mavsdk::Telemetry::FlightMode::Takeoff)
        .value("Hold", mavsdk::Telemetry::FlightMode::Hold)
        .value("Mission", mavsdk::Telemetry::FlightMode::Mission)
        .value("ReturnToLaunch", mavsdk::Telemetry::FlightMode::ReturnToLaunch)
        .value("Land", mavsdk::Telemetry::FlightMode::Land)
        .value("Offboard", mavsdk::Telemetry::FlightMode::Offboard)
        .value("FollowMe", mavsdk::Telemetry::FlightMode::FollowMe)
        .value("Manual", mavsdk::Telemetry::FlightMode::Manual)
        .value("Altctl", mavsdk::Telemetry::FlightMode::Altctl)
        .value("Posctl", mavsdk::Telemetry::FlightMode::Posctl)
        .value("Acro", mavsdk::Telemetry::FlightMode::Acro)
        .value("Stabilized", mavsdk::Telemetry::FlightMode::Stabilized)
        .value("Rattitude", mavsdk::Telemetry::FlightMode::Rattitude);

    py::enum_<mavsdk::Telemetry::StatusTextType>(telemetry, "StatusTextType")
        .value("Debug", mavsdk::Telemetry::StatusTextType::Debug)
        .value("Info", mavsdk::Telemetry::StatusTextType::Info)
        .value("Notice", mavsdk::Telemetry::StatusTextType::Notice)
        .value("Warning", mavsdk::Telemetry::StatusTextType::Warning)
        .value("Error", mavsdk::Telemetry::StatusTextType::Error)
        .value("Critical", mavsdk::Telemetry::StatusTextType::Critical)
        .value("Alert", mavsdk::Telemetry::StatusTextType::Alert)
        .value("Emergency", mavsdk::Telemetry::StatusTextType::Emergency);

    py::enum_<mavsdk::Telemetry::LandedState>(telemetry, "LandedState")
        .value("Unknown", mavsdk::Telemetry::LandedState::Unknown)
        .value("OnGround", mavsdk::Telemetry::LandedState::OnGround)
        .value("InAir", mavsdk::Telemetry::LandedState::InAir)
        .value("TakingOff", mavsdk::Telemetry::LandedState::TakingOff)
        .value("Landing", mavsdk::Telemetry::LandedState::Landing);

    py::class_<mavsdk::Telemetry::Position> position(telemetry, "Position");
    position.def(py::init<>())
        .def_readwrite("latitude_deg", &mavsdk::Telemetry::Position::latitude_deg)
        .def_readwrite("longitude_deg", &mavsdk::Telemetry::Position::longitude_deg)
        .def_readwrite("absolute_altitude_m", &mavsdk::Telemetry::Position::absolute_altitude_m)
        .def_readwrite("relative_altitude_m", &mavsdk::Telemetry::Position::relative_altitude_m);

    py::class_<mavsdk::Telemetry::Quaternion> quaternion(telemetry, "Quaternion");
    quaternion.def(py::init<>())
        .def_readwrite("w", &mavsdk::Telemetry::Quaternion::w)
        .def_readwrite("x", &mavsdk::Telemetry::Quaternion::x)
        .def_readwrite("y", &mavsdk::Telemetry::Quaternion::y)
        .def_readwrite("z", &mavsdk::Telemetry::Quaternion::z)
        .def_readwrite("timestamp_us", &mavsdk::Telemetry::Quaternion::timestamp_us);

    py::class_<mavsdk::Telemetry::EulerAngle> euler_angle(telemetry, "EulerAngle");
    euler_angle.def(py::init<>())
        .def_readwrite("roll_deg", &mavsdk::Telemetry::EulerAngle::roll_deg)
        .def_readwrite("pitch_deg", &mavsdk::Telemetry::EulerAngle::pitch_deg)
        .def_readwrite("yaw_deg", &mavsdk::Telemetry::EulerAngle::yaw_deg)
        .def_readwrite("timestamp_us", &mavsdk::Telemetry::EulerAngle::timestamp_us);

    py::class_<mavsdk::Telemetry::AngularVelocityBody> angular_velocity_body(
        telemetry, "AngularVelocityBody");
    angular_velocity_body.def(py::init<>())
        .def_readwrite("roll_rad_s", &mavsdk::Telemetry::AngularVelocityBody::roll_rad_s)
        .def_readwrite("pitch_rad_s", &mavsdk::Telemetry::AngularVelocityBody::pitch_rad_s)
        .def_readwrite("yaw_rad_s", &mavsdk::Telemetry::AngularVelocityBody::yaw_rad_s);

    py::class_<mavsdk::Telemetry::GpsInfo> gps_info(telemetry, "GpsInfo");
    gps_info.def(py::init<>())
        .def_readwrite("num_satellites", &mavsdk::Telemetry::GpsInfo::num_satellites)
        .def_readwrite("fix_type", &mavsdk::Telemetry::GpsInfo::fix_type);

    py::class_<mavsdk::Telemetry::Battery> battery(telemetry, "Battery");
    battery.def(py::init<>())
        .def_readwrite("voltage_v", &mavsdk::Telemetry::Battery::voltage_v)
        .def_readwrite("remaining_percent", &mavsdk::Telemetry::Battery::remaining_percent);

    py::class_<mavsdk::Telemetry::Health> health(telemetry, "Health");
    health.def(py::init<>())
        .def_readwrite(
            "is_gyrometer_calibration_ok", &mavsdk::Telemetry::Health::is_gyrometer_calibration_ok)
        .def_readwrite(
            "is_accelerometer_calibration_ok",
            &mavsdk::Telemetry::Health::is_accelerometer_calibration_ok)
        .def_readwrite(
            "is_magnetometer_calibration_ok",
            &mavsdk::Telemetry::Health::is_magnetometer_calibration_ok)
        .def_readwrite(
            "is_level_calibration_ok", &mavsdk::Telemetry::Health::is_level_calibration_ok)
        .def_readwrite("is_local_position_ok", &mavsdk::Telemetry::Health::is_local_position_ok)
        .def_readwrite("is_global_position_ok", &mavsdk::Telemetry::Health::is_global_position_ok)
        .def_readwrite("is_home_position_ok", &mavsdk::Telemetry::Health::is_home_position_ok);

    py::class_<mavsdk::Telemetry::RcStatus> rc_status(telemetry, "RcStatus");
    rc_status.def(py::init<>())
        .def_readwrite("was_available_once", &mavsdk::Telemetry::RcStatus::was_available_once)
        .def_readwrite("is_available", &mavsdk::Telemetry::RcStatus::is_available)
        .def_readwrite(
            "signal_strength_percent", &mavsdk::Telemetry::RcStatus::signal_strength_percent);

    py::class_<mavsdk::Telemetry::StatusText> status_text(telemetry, "StatusText");
    status_text.def(py::init<>())
        .def_readwrite("type", &mavsdk::Telemetry::StatusText::type)
        .def_readwrite("text", &mavsdk::Telemetry::StatusText::text);

    py::class_<mavsdk::Telemetry::ActuatorControlTarget> actuator_control_target(
        telemetry, "ActuatorControlTarget");
    actuator_control_target.def(py::init<>())
        .def_readwrite("group", &mavsdk::Telemetry::ActuatorControlTarget::group)
        .def_readwrite("controls", &mavsdk::Telemetry::ActuatorControlTarget::controls);

    py::class_<mavsdk::Telemetry::ActuatorOutputStatus> actuator_output_status(
        telemetry, "ActuatorOutputStatus");
    actuator_output_status.def(py::init<>())
        .def_readwrite("active", &mavsdk::Telemetry::ActuatorOutputStatus::active)
        .def_readwrite("actuator", &mavsdk::Telemetry::ActuatorOutputStatus::actuator);

    py::class_<mavsdk::Telemetry::Covariance> covariance(telemetry, "Covariance");
    covariance.def(py::init<>())
        .def_readwrite("covariance_matrix", &mavsdk::Telemetry::Covariance::covariance_matrix);

    py::class_<mavsdk::Telemetry::VelocityBody> velocity_body(telemetry, "VelocityBody");
    velocity_body.def(py::init<>())
        .def_readwrite("x_m_s", &mavsdk::Telemetry::VelocityBody::x_m_s)
        .def_readwrite("y_m_s", &mavsdk::Telemetry::VelocityBody::y_m_s)
        .def_readwrite("z_m_s", &mavsdk::Telemetry::VelocityBody::z_m_s);

    py::class_<mavsdk::Telemetry::PositionBody> position_body(telemetry, "PositionBody");
    position_body.def(py::init<>())
        .def_readwrite("x_m", &mavsdk::Telemetry::PositionBody::x_m)
        .def_readwrite("y_m", &mavsdk::Telemetry::PositionBody::y_m)
        .def_readwrite("z_m", &mavsdk::Telemetry::PositionBody::z_m);

    py::class_<mavsdk::Telemetry::Odometry> odometry(telemetry, "Odometry");
    odometry.def(py::init<>())
        .def_readwrite("time_usec", &mavsdk::Telemetry::Odometry::time_usec)
        .def_readwrite("frame_id", &mavsdk::Telemetry::Odometry::frame_id)
        .def_readwrite("child_frame_id", &mavsdk::Telemetry::Odometry::child_frame_id)
        .def_readwrite("position_body", &mavsdk::Telemetry::Odometry::position_body)
        .def_readwrite("q", &mavsdk::Telemetry::Odometry::q)
        .def_readwrite("velocity_body", &mavsdk::Telemetry::Odometry::velocity_body)
        .def_readwrite("angular_velocity_body", &mavsdk::Telemetry::Odometry::angular_velocity_body)
        .def_readwrite("pose_covariance", &mavsdk::Telemetry::Odometry::pose_covariance)
        .def_readwrite("velocity_covariance", &mavsdk::Telemetry::Odometry::velocity_covariance);

    py::enum_<mavsdk::Telemetry::Odometry::MavFrame>(odometry, "MavFrame")
        .value("Undef", mavsdk::Telemetry::Odometry::MavFrame::Undef)
        .value("BodyNed", mavsdk::Telemetry::Odometry::MavFrame::BodyNed)
        .value("VisionNed", mavsdk::Telemetry::Odometry::MavFrame::VisionNed)
        .value("EstimNed", mavsdk::Telemetry::Odometry::MavFrame::EstimNed);

    py::class_<mavsdk::Telemetry::DistanceSensor> distance_sensor(telemetry, "DistanceSensor");
    distance_sensor.def(py::init<>())
        .def_readwrite("minimum_distance_m", &mavsdk::Telemetry::DistanceSensor::minimum_distance_m)
        .def_readwrite("maximum_distance_m", &mavsdk::Telemetry::DistanceSensor::maximum_distance_m)
        .def_readwrite(
            "current_distance_m", &mavsdk::Telemetry::DistanceSensor::current_distance_m);

    py::class_<mavsdk::Telemetry::PositionNed> position_ned(telemetry, "PositionNed");
    position_ned.def(py::init<>())
        .def_readwrite("north_m", &mavsdk::Telemetry::PositionNed::north_m)
        .def_readwrite("east_m", &mavsdk::Telemetry::PositionNed::east_m)
        .def_readwrite("down_m", &mavsdk::Telemetry::PositionNed::down_m);

    py::class_<mavsdk::Telemetry::VelocityNed> velocity_ned(telemetry, "VelocityNed");
    velocity_ned.def(py::init<>())
        .def_readwrite("north_m_s", &mavsdk::Telemetry::VelocityNed::north_m_s)
        .def_readwrite("east_m_s", &mavsdk::Telemetry::VelocityNed::east_m_s)
        .def_readwrite("down_m_s", &mavsdk::Telemetry::VelocityNed::down_m_s);

    py::class_<mavsdk::Telemetry::PositionVelocityNed> position_velocity_ned(
        telemetry, "PositionVelocityNed");
    position_velocity_ned.def(py::init<>())
        .def_readwrite("position", &mavsdk::Telemetry::PositionVelocityNed::position)
        .def_readwrite("velocity", &mavsdk::Telemetry::PositionVelocityNed::velocity);

    py::class_<mavsdk::Telemetry::GroundTruth> ground_truth(telemetry, "GroundTruth");
    ground_truth.def(py::init<>())
        .def_readwrite("latitude_deg", &mavsdk::Telemetry::GroundTruth::latitude_deg)
        .def_readwrite("longitude_deg", &mavsdk::Telemetry::GroundTruth::longitude_deg)
        .def_readwrite("absolute_altitude_m", &mavsdk::Telemetry::GroundTruth::absolute_altitude_m);

    py::class_<mavsdk::Telemetry::FixedwingMetrics> fixedwing_metrics(
        telemetry, "FixedwingMetrics");
    fixedwing_metrics.def(py::init<>())
        .def_readwrite("airspeed_m_s", &mavsdk::Telemetry::FixedwingMetrics::airspeed_m_s)
        .def_readwrite(
            "throttle_percentage", &mavsdk::Telemetry::FixedwingMetrics::throttle_percentage)
        .def_readwrite("climb_rate_m_s", &mavsdk::Telemetry::FixedwingMetrics::climb_rate_m_s);

    py::class_<mavsdk::Telemetry::AccelerationFrd> acceleration_frd(telemetry, "AccelerationFrd");
    acceleration_frd.def(py::init<>())
        .def_readwrite("forward_m_s2", &mavsdk::Telemetry::AccelerationFrd::forward_m_s2)
        .def_readwrite("right_m_s2", &mavsdk::Telemetry::AccelerationFrd::right_m_s2)
        .def_readwrite("down_m_s2", &mavsdk::Telemetry::AccelerationFrd::down_m_s2);

    py::class_<mavsdk::Telemetry::AngularVelocityFrd> angular_velocity_frd(
        telemetry, "AngularVelocityFrd");
    angular_velocity_frd.def(py::init<>())
        .def_readwrite("forward_rad_s", &mavsdk::Telemetry::AngularVelocityFrd::forward_rad_s)
        .def_readwrite("right_rad_s", &mavsdk::Telemetry::AngularVelocityFrd::right_rad_s)
        .def_readwrite("down_rad_s", &mavsdk::Telemetry::AngularVelocityFrd::down_rad_s);

    py::class_<mavsdk::Telemetry::MagneticFieldFrd> magnetic_field_frd(
        telemetry, "MagneticFieldFrd");
    magnetic_field_frd.def(py::init<>())
        .def_readwrite("forward_gauss", &mavsdk::Telemetry::MagneticFieldFrd::forward_gauss)
        .def_readwrite("right_gauss", &mavsdk::Telemetry::MagneticFieldFrd::right_gauss)
        .def_readwrite("down_gauss", &mavsdk::Telemetry::MagneticFieldFrd::down_gauss);

    py::class_<mavsdk::Telemetry::Imu> imu(telemetry, "Imu");
    imu.def(py::init<>())
        .def_readwrite("acceleration_frd", &mavsdk::Telemetry::Imu::acceleration_frd)
        .def_readwrite("angular_velocity_frd", &mavsdk::Telemetry::Imu::angular_velocity_frd)
        .def_readwrite("magnetic_field_frd", &mavsdk::Telemetry::Imu::magnetic_field_frd)
        .def_readwrite("temperature_degc", &mavsdk::Telemetry::Imu::temperature_degc);

    py::class_<mavsdk::Telemetry::GpsGlobalOrigin> gps_global_origin(telemetry, "GpsGlobalOrigin");
    gps_global_origin.def(py::init<>())
        .def_readwrite("latitude_deg", &mavsdk::Telemetry::GpsGlobalOrigin::latitude_deg)
        .def_readwrite("longitude_deg", &mavsdk::Telemetry::GpsGlobalOrigin::longitude_deg)
        .def_readwrite("altitude_m", &mavsdk::Telemetry::GpsGlobalOrigin::altitude_m);

    py::enum_<mavsdk::Telemetry::Result>(telemetry, "Result")
        .value("Unknown", mavsdk::Telemetry::Result::Unknown)
        .value("Success", mavsdk::Telemetry::Result::Success)
        .value("NoSystem", mavsdk::Telemetry::Result::NoSystem)
        .value("ConnectionError", mavsdk::Telemetry::Result::ConnectionError)
        .value("Busy", mavsdk::Telemetry::Result::Busy)
        .value("CommandDenied", mavsdk::Telemetry::Result::CommandDenied)
        .value("Timeout", mavsdk::Telemetry::Result::Timeout);

    system.def(
        "telemetry",
        &mavsdk::System::telemetry,
        py::return_value_policy::reference_internal,
        py::keep_alive<1, 0>());
}