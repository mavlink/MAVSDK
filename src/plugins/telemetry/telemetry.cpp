#include <cmath>
#include <limits>

#include "plugins/telemetry/telemetry.h"
#include "telemetry_impl.h"

namespace mavsdk {

Telemetry::Telemetry(System& system) : PluginBase(), _impl{new TelemetryImpl(system)} {}

Telemetry::~Telemetry() {}

Telemetry::Result Telemetry::set_rate_position_velocity_ned(double rate_hz)
{
    return _impl->set_rate_position_velocity_ned(rate_hz);
}

Telemetry::Result Telemetry::set_rate_position(double rate_hz)
{
    return _impl->set_rate_position(rate_hz);
}

Telemetry::Result Telemetry::set_rate_home_position(double rate_hz)
{
    return _impl->set_rate_home_position(rate_hz);
}

Telemetry::Result Telemetry::set_rate_in_air(double rate_hz)
{
    return _impl->set_rate_in_air(rate_hz);
}

Telemetry::Result Telemetry::set_rate_attitude(double rate_hz)
{
    return _impl->set_rate_attitude(rate_hz);
}

Telemetry::Result Telemetry::set_rate_camera_attitude(double rate_hz)
{
    return _impl->set_rate_camera_attitude(rate_hz);
}

Telemetry::Result Telemetry::set_rate_ground_speed_ned(double rate_hz)
{
    return _impl->set_rate_ground_speed_ned(rate_hz);
}

Telemetry::Result Telemetry::set_rate_imu_reading_ned(double rate_hz)
{
    return _impl->set_rate_imu_reading_ned(rate_hz);
}

Telemetry::Result Telemetry::set_rate_fixedwing_metrics(double rate_hz)
{
    return _impl->set_rate_fixedwing_metrics(rate_hz);
}

Telemetry::Result Telemetry::set_rate_ground_truth(double rate_hz)
{
    return _impl->set_rate_ground_truth(rate_hz);
}

Telemetry::Result Telemetry::set_rate_gps_info(double rate_hz)
{
    return _impl->set_rate_gps_info(rate_hz);
}

Telemetry::Result Telemetry::set_rate_battery(double rate_hz)
{
    return _impl->set_rate_battery(rate_hz);
}

Telemetry::Result Telemetry::set_rate_rc_status(double rate_hz)
{
    return _impl->set_rate_rc_status(rate_hz);
}

Telemetry::Result Telemetry::set_rate_actuator_control_target(double rate_hz)
{
    return _impl->set_rate_actuator_control_target(rate_hz);
}

Telemetry::Result Telemetry::set_rate_actuator_output_status(double rate_hz)
{
    return _impl->set_rate_actuator_output_status(rate_hz);
}

Telemetry::Result Telemetry::set_rate_odometry(double rate_hz)
{
    return _impl->set_rate_odometry(rate_hz);
}

void Telemetry::set_rate_position_velocity_ned_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_position_velocity_ned_async(rate_hz, callback);
}

void Telemetry::set_rate_position_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_position_async(rate_hz, callback);
}

void Telemetry::set_rate_home_position_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_home_position_async(rate_hz, callback);
}

void Telemetry::set_rate_in_air_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_in_air_async(rate_hz, callback);
}

void Telemetry::set_rate_attitude_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_attitude_async(rate_hz, callback);
}

void Telemetry::set_rate_camera_attitude_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_camera_attitude_async(rate_hz, callback);
}

void Telemetry::set_rate_ground_speed_ned_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_ground_speed_ned_async(rate_hz, callback);
}

void Telemetry::set_rate_imu_reading_ned_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_imu_reading_ned_async(rate_hz, callback);
}

void Telemetry::set_rate_fixedwing_metrics_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_fixedwing_metrics_async(rate_hz, callback);
}

void Telemetry::set_rate_ground_truth_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_ground_truth_async(rate_hz, callback);
}

void Telemetry::set_rate_gps_info_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_gps_info_async(rate_hz, callback);
}

void Telemetry::set_rate_battery_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_battery_async(rate_hz, callback);
}

void Telemetry::set_rate_rc_status_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_rc_status_async(rate_hz, callback);
}

void Telemetry::set_unix_epoch_time_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_unix_epoch_time_async(rate_hz, callback);
}

void Telemetry::set_rate_actuator_control_target_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_actuator_control_target_async(rate_hz, callback);
}

void Telemetry::set_rate_actuator_output_status_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_actuator_output_status_async(rate_hz, callback);
}

void Telemetry::set_rate_odometry_async(double rate_hz, result_callback_t callback)
{
    _impl->set_rate_odometry_async(rate_hz, callback);
}

Telemetry::PositionVelocityNED Telemetry::position_velocity_ned() const
{
    return _impl->get_position_velocity_ned();
}

Telemetry::Position Telemetry::position() const
{
    return _impl->get_position();
}

Telemetry::Position Telemetry::home_position() const
{
    return _impl->get_home_position();
}

bool Telemetry::in_air() const
{
    return _impl->in_air();
}

Telemetry::LandedState Telemetry::landed_state() const
{
    return _impl->get_landed_state();
}

void Telemetry::landed_state_async(landed_state_callback_t callback)
{
    _impl->landed_state_async(callback);
}

std::string Telemetry::landed_state_str(LandedState landed_state)
{
    switch (landed_state) {
        case LandedState::ON_GROUND:
            return "On ground";
        case LandedState::IN_AIR:
            return "In air";
        case LandedState::TAKING_OFF:
            return "Taking off";
        case LandedState::LANDING:
            return "Landing";
        case LandedState::UNKNOWN:
        default:
            return "Unknown";
    }
}

Telemetry::StatusText Telemetry::status_text() const
{
    return _impl->get_status_text();
}

bool Telemetry::armed() const
{
    return _impl->armed();
}

Telemetry::Quaternion Telemetry::attitude_quaternion() const
{
    return _impl->get_attitude_quaternion();
}

Telemetry::EulerAngle Telemetry::attitude_euler_angle() const
{
    return _impl->get_attitude_euler_angle();
}

Telemetry::AngularVelocityBody Telemetry::attitude_angular_velocity_body() const
{
    return _impl->get_attitude_angular_velocity_body();
}

Telemetry::FixedwingMetrics Telemetry::fixedwing_metrics() const
{
    return _impl->get_fixedwing_metrics();
}

Telemetry::GroundTruth Telemetry::ground_truth() const
{
    return _impl->get_ground_truth();
}

Telemetry::Quaternion Telemetry::camera_attitude_quaternion() const
{
    return _impl->get_camera_attitude_quaternion();
}

Telemetry::EulerAngle Telemetry::camera_attitude_euler_angle() const
{
    return _impl->get_camera_attitude_euler_angle();
}

Telemetry::GroundSpeedNED Telemetry::ground_speed_ned() const
{
    return _impl->get_ground_speed_ned();
}

Telemetry::IMUReadingNED Telemetry::imu_reading_ned() const
{
    return _impl->get_imu_reading_ned();
}

Telemetry::GPSInfo Telemetry::gps_info() const
{
    return _impl->get_gps_info();
}

Telemetry::Battery Telemetry::battery() const
{
    return _impl->get_battery();
}

Telemetry::FlightMode Telemetry::flight_mode() const
{
    return _impl->get_flight_mode();
}

Telemetry::Health Telemetry::health() const
{
    return _impl->get_health();
}

bool Telemetry::health_all_ok() const
{
    return _impl->get_health_all_ok();
}

Telemetry::RCStatus Telemetry::rc_status() const
{
    return _impl->get_rc_status();
}

Telemetry::ActuatorControlTarget Telemetry::actuator_control_target() const
{
    return _impl->get_actuator_control_target();
}

Telemetry::ActuatorOutputStatus Telemetry::actuator_output_status() const
{
    return _impl->get_actuator_output_status();
}

void Telemetry::position_velocity_ned_async(position_velocity_ned_callback_t callback)
{
    return _impl->position_velocity_ned_async(callback);
}

void Telemetry::position_async(position_callback_t callback)
{
    return _impl->position_async(callback);
}

void Telemetry::home_position_async(position_callback_t callback)
{
    return _impl->home_position_async(callback);
}

void Telemetry::in_air_async(in_air_callback_t callback)
{
    return _impl->in_air_async(callback);
}

void Telemetry::status_text_async(status_text_callback_t callback)
{
    return _impl->status_text_async(callback);
}

void Telemetry::armed_async(armed_callback_t callback)
{
    return _impl->armed_async(callback);
}

void Telemetry::attitude_quaternion_async(attitude_quaternion_callback_t callback)
{
    return _impl->attitude_quaternion_async(callback);
}

void Telemetry::attitude_euler_angle_async(attitude_euler_angle_callback_t callback)
{
    return _impl->attitude_euler_angle_async(callback);
}

void Telemetry::attitude_angular_velocity_body_async(
    attitude_angular_velocity_body_callback_t callback)
{
    return _impl->attitude_angular_velocity_body_async(callback);
}

void Telemetry::fixedwing_metrics_async(fixedwing_metrics_callback_t callback)
{
    return _impl->fixedwing_metrics_async(callback);
}

void Telemetry::ground_truth_async(ground_truth_callback_t callback)
{
    return _impl->ground_truth_async(callback);
}

void Telemetry::camera_attitude_quaternion_async(attitude_quaternion_callback_t callback)
{
    return _impl->camera_attitude_quaternion_async(callback);
}

void Telemetry::camera_attitude_euler_angle_async(attitude_euler_angle_callback_t callback)
{
    return _impl->camera_attitude_euler_angle_async(callback);
}

void Telemetry::ground_speed_ned_async(ground_speed_ned_callback_t callback)
{
    return _impl->ground_speed_ned_async(callback);
}

void Telemetry::imu_reading_ned_async(imu_reading_ned_callback_t callback)
{
    return _impl->imu_reading_ned_async(callback);
}

void Telemetry::gps_info_async(gps_info_callback_t callback)
{
    return _impl->gps_info_async(callback);
}

void Telemetry::battery_async(battery_callback_t callback)
{
    return _impl->battery_async(callback);
}

void Telemetry::flight_mode_async(flight_mode_callback_t callback)
{
    return _impl->flight_mode_async(callback);
}

void Telemetry::actuator_control_target_async(actuator_control_target_callback_t callback)
{
    return _impl->actuator_control_target_async(callback);
}

void Telemetry::actuator_output_status_async(actuator_output_status_callback_t callback)
{
    return _impl->actuator_output_status_async(callback);
}

void Telemetry::odometry_async(odometry_callback_t callback)
{
    return _impl->odometry_async(callback);
}

std::string Telemetry::flight_mode_str(FlightMode flight_mode)
{
    switch (flight_mode) {
        case FlightMode::READY:
            return "Ready";
        case FlightMode::TAKEOFF:
            return "Takeoff";
        case FlightMode::HOLD:
            return "Hold";
        case FlightMode::MISSION:
            return "Mission";
        case FlightMode::RETURN_TO_LAUNCH:
            return "Return to launch";
        case FlightMode::LAND:
            return "Land";
        case FlightMode::OFFBOARD:
            return "Offboard";
        case FlightMode::FOLLOW_ME:
            return "FollowMe";
        case FlightMode::MANUAL:
            return "Manual";
        case FlightMode::POSCTL:
            return "Position";
        case FlightMode::ALTCTL:
            return "Altitude";
        case FlightMode::RATTITUDE:
            return "Rattitude";
        case FlightMode::ACRO:
            return "Acro";
        case FlightMode::STABILIZED:
            return "Stabilized";
        case FlightMode::UNKNOWN:
            return "Unknown";
        default:
            return "Unknown";
    }
}

void Telemetry::health_async(health_callback_t callback)
{
    return _impl->health_async(callback);
}

void Telemetry::health_all_ok_async(health_all_ok_callback_t callback)
{
    return _impl->health_all_ok_async(callback);
}

void Telemetry::rc_status_async(rc_status_callback_t callback)
{
    return _impl->rc_status_async(callback);
}

void Telemetry::unix_epoch_time_async(unix_epoch_time_callback_t callback)
{
    return _impl->unix_epoch_time_async(callback);
}

const char* Telemetry::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::NO_SYSTEM:
            return "No system";
        case Result::CONNECTION_ERROR:
            return "Connection error";
        case Result::BUSY:
            return "Busy";
        case Result::COMMAND_DENIED:
            return "Command denied";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

bool operator==(
    const Telemetry::PositionVelocityNED& lhs, const Telemetry::PositionVelocityNED& rhs)
{
    return std::fabs(lhs.position.north_m - rhs.position.north_m) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.position.east_m - rhs.position.east_m) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.position.down_m - rhs.position.down_m) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.velocity.north_m_s - rhs.velocity.north_m_s) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.velocity.east_m_s - rhs.velocity.east_m_s) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.velocity.down_m_s - rhs.velocity.down_m_s) <=
               std::numeric_limits<float>::epsilon();
}

bool operator==(const Telemetry::Position& lhs, const Telemetry::Position& rhs)
{
    return std::abs(lhs.latitude_deg - rhs.latitude_deg) <=
               std::numeric_limits<double>::epsilon() &&
           std::abs(lhs.longitude_deg - rhs.longitude_deg) <=
               std::numeric_limits<double>::epsilon() &&
           std::abs(lhs.absolute_altitude_m - rhs.absolute_altitude_m) <=
               std::numeric_limits<float>::epsilon() &&
           std::abs(lhs.relative_altitude_m - rhs.relative_altitude_m) <=
               std::numeric_limits<float>::epsilon();
}

std::ostream& operator<<(std::ostream& str, Telemetry::Position const& position)
{
    return str << "[lat: " << position.latitude_deg << ", lon: " << position.longitude_deg
               << ", abs_alt: " << position.absolute_altitude_m
               << ", rel_alt: " << position.relative_altitude_m << "]";
}

std::ostream& operator<<(std::ostream& str, Telemetry::PositionNED const& position_ned)
{
    return str << "[position_north_m: " << position_ned.north_m
               << ", position_east_m: " << position_ned.east_m
               << ", position_down_m: " << position_ned.down_m << "]";
}

std::ostream& operator<<(std::ostream& str, Telemetry::VelocityNED const& velocity_ned)
{
    return str << "[velocity_north_m_s: " << velocity_ned.north_m_s
               << ", velocity_east_m_s: " << velocity_ned.east_m_s
               << ", velocity_down_m_s: " << velocity_ned.down_m_s << "]";
}

std::ostream&
operator<<(std::ostream& str, Telemetry::PositionVelocityNED const& position_velocity_ned)
{
    return str << "[position_north_m: " << position_velocity_ned.position.north_m
               << ", position_east_m: " << position_velocity_ned.position.east_m
               << ", position_down_m: " << position_velocity_ned.position.down_m << "] "
               << "[velocity_north_m_s: " << position_velocity_ned.velocity.north_m_s
               << ", velocity_east_m_s: " << position_velocity_ned.velocity.east_m_s
               << ", velocity_down_m_s: " << position_velocity_ned.velocity.down_m_s << "]";
}

bool operator==(const Telemetry::Health& lhs, const Telemetry::Health& rhs)
{
    return lhs.gyrometer_calibration_ok == rhs.gyrometer_calibration_ok &&
           lhs.accelerometer_calibration_ok == rhs.accelerometer_calibration_ok &&
           lhs.magnetometer_calibration_ok == rhs.magnetometer_calibration_ok &&
           lhs.level_calibration_ok == rhs.level_calibration_ok &&
           lhs.local_position_ok == rhs.local_position_ok &&
           lhs.global_position_ok == rhs.global_position_ok &&
           lhs.home_position_ok == rhs.home_position_ok;
}

std::ostream& operator<<(std::ostream& str, Telemetry::Health const& health)
{
    return str << "[gyrometer_calibration_ok: " << health.gyrometer_calibration_ok
               << ", accelerometer_calibration_ok: " << health.accelerometer_calibration_ok
               << ", magnetometer_calibration_ok: " << health.magnetometer_calibration_ok
               << ", level_calibration_ok: " << health.level_calibration_ok
               << ", local_position_ok: " << health.local_position_ok
               << ", global_position_ok: " << health.global_position_ok
               << ", home_position_ok: " << health.home_position_ok << "]";
}

bool operator==(const Telemetry::IMUReadingNED& lhs, const Telemetry::IMUReadingNED& rhs)
{
    return std::fabs(lhs.acceleration.north_m_s2 - rhs.acceleration.north_m_s2) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.acceleration.east_m_s2 - rhs.acceleration.east_m_s2) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.acceleration.down_m_s2 - rhs.acceleration.down_m_s2) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.angular_velocity.north_rad_s - rhs.angular_velocity.north_rad_s) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.angular_velocity.east_rad_s - rhs.angular_velocity.east_rad_s) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.angular_velocity.down_rad_s - rhs.angular_velocity.down_rad_s) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.magnetic_field.north_gauss - rhs.magnetic_field.north_gauss) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.magnetic_field.east_gauss - rhs.magnetic_field.east_gauss) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.magnetic_field.down_gauss - rhs.magnetic_field.down_gauss) <=
               std::numeric_limits<float>::epsilon() &&
           std::fabs(lhs.temperature_degC - rhs.temperature_degC) <=
               std::numeric_limits<float>::epsilon();
}

std::ostream& operator<<(std::ostream& str, Telemetry::AccelerationNED const& acceleration_ned)
{
    return str << "[acceleration_north_m_s2: " << acceleration_ned.north_m_s2
               << ", acceleration_east_m_s2: " << acceleration_ned.east_m_s2
               << ", acceleration_down_m_s2: " << acceleration_ned.down_m_s2 << "]";
}

std::ostream&
operator<<(std::ostream& str, Telemetry::AngularVelocityNED const& angular_velocity_ned)
{
    return str << "[angular_velocity_north_rad_s: " << angular_velocity_ned.north_rad_s
               << ", angular_velocity_east_rad_s: " << angular_velocity_ned.east_rad_s
               << ", angular_velocity_down_rad_s: " << angular_velocity_ned.down_rad_s << "]";
}

std::ostream& operator<<(std::ostream& str, Telemetry::MagneticFieldNED const& magnetic_field_ned)
{
    return str << "[magnetic_field_north_gauss: " << magnetic_field_ned.north_gauss
               << ", magnetic_field_east_gauss: " << magnetic_field_ned.east_gauss
               << ", magnetic_field_down_gauss: " << magnetic_field_ned.down_gauss << "]";
}

std::ostream& operator<<(std::ostream& str, Telemetry::IMUReadingNED const& imu_reading_ned)
{
    return str << "[acceleration_north_m_s2: " << imu_reading_ned.acceleration.north_m_s2
               << ", acceleration_east_m_s2: " << imu_reading_ned.acceleration.east_m_s2
               << ", acceleration_down_m_s2: " << imu_reading_ned.acceleration.down_m_s2 << "] "
               << "[angular_velocity_north_rad_s: " << imu_reading_ned.angular_velocity.north_rad_s
               << ", angular_velocity_east_rad_s: " << imu_reading_ned.angular_velocity.east_rad_s
               << ", angular_velocity_down_rad_s: " << imu_reading_ned.angular_velocity.down_rad_s
               << "] "
               << "[magnetic_field_north_gauss: " << imu_reading_ned.magnetic_field.north_gauss
               << ", magnetic_field_east_gauss: " << imu_reading_ned.magnetic_field.east_gauss
               << ", magnetic_field_down_gauss: " << imu_reading_ned.magnetic_field.down_gauss
               << "] "
               << "[temperature_degC: " << imu_reading_ned.temperature_degC << "]";
}

bool operator==(const Telemetry::GPSInfo& lhs, const Telemetry::GPSInfo& rhs)
{
    return lhs.num_satellites == rhs.num_satellites && lhs.fix_type == rhs.fix_type;
}

std::ostream& operator<<(std::ostream& str, Telemetry::GPSInfo const& gps_info)
{
    return str << "[num_sat: " << gps_info.num_satellites << ", fix_type: " << gps_info.fix_type
               << "]";
}

bool operator==(const Telemetry::Battery& lhs, const Telemetry::Battery& rhs)
{
    return lhs.voltage_v == rhs.voltage_v && lhs.remaining_percent == rhs.remaining_percent;
}

std::ostream& operator<<(std::ostream& str, Telemetry::Battery const& battery)
{
    return str << "[voltage_v: " << battery.voltage_v
               << ", remaining_percent: " << battery.remaining_percent << "]";
}

bool operator==(const Telemetry::Quaternion& lhs, const Telemetry::Quaternion& rhs)
{
    return lhs.w == rhs.w && lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

bool operator!=(const Telemetry::Quaternion& lhs, const Telemetry::Quaternion& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& str, Telemetry::Quaternion const& quaternion)
{
    return str << "[w: " << quaternion.w << ", x: " << quaternion.x << ", y: " << quaternion.y
               << ", z: " << quaternion.z << "]";
}

bool operator==(const Telemetry::EulerAngle& lhs, const Telemetry::EulerAngle& rhs)
{
    return lhs.roll_deg == rhs.roll_deg && lhs.pitch_deg == rhs.pitch_deg &&
           lhs.yaw_deg == rhs.yaw_deg;
}

std::ostream& operator<<(std::ostream& str, Telemetry::EulerAngle const& euler_angle)
{
    return str << "[roll_deg: " << euler_angle.roll_deg << ", pitch_deg: " << euler_angle.pitch_deg
               << ", yaw_deg: " << euler_angle.yaw_deg << "]";
}

bool operator==(
    const Telemetry::AngularVelocityBody& lhs, const Telemetry::AngularVelocityBody& rhs)
{
    return lhs.roll_rad_s == rhs.roll_rad_s && lhs.pitch_rad_s == rhs.pitch_rad_s &&
           lhs.yaw_rad_s == rhs.yaw_rad_s;
}

bool operator!=(
    const Telemetry::AngularVelocityBody& lhs, const Telemetry::AngularVelocityBody& rhs)
{
    return !(lhs == rhs);
}

std::ostream&
operator<<(std::ostream& str, Telemetry::AngularVelocityBody const& angular_velocity_body)
{
    return str << "[angular_velocity_body_roll_rad_s: " << angular_velocity_body.roll_rad_s
               << ", angular_velocity_body_pitch_rad_s: " << angular_velocity_body.pitch_rad_s
               << ", angular_velocity_body_yaw_rad_s: " << angular_velocity_body.yaw_rad_s << "]";
}

std::ostream& operator<<(std::ostream& str, Telemetry::FixedwingMetrics const& fixedwing_metrics)
{
    return str << "Airspeed: " << fixedwing_metrics.airspeed_m_s << " m/s, "
               << "Throttle: " << fixedwing_metrics.throttle_percentage << " %, "
               << "Climb: " << fixedwing_metrics.climb_rate_m_s << " m/s";
}

std::ostream& operator<<(std::ostream& str, Telemetry::GroundTruth const& ground_truth)
{
    return str << "Latitude: " << ground_truth.latitude_deg << " deg, "
               << "Longitude: " << ground_truth.longitude_deg << " deg, "
               << "Altitude: " << ground_truth.absolute_altitude_m << " m";
}

bool operator==(const Telemetry::GroundSpeedNED& lhs, const Telemetry::GroundSpeedNED& rhs)
{
    return lhs.velocity_north_m_s == rhs.velocity_north_m_s &&
           lhs.velocity_east_m_s == rhs.velocity_east_m_s &&
           lhs.velocity_down_m_s == rhs.velocity_down_m_s;
}

std::ostream& operator<<(std::ostream& str, Telemetry::GroundSpeedNED const& ground_speed)
{
    return str << "[velocity_north_m_s: " << ground_speed.velocity_north_m_s
               << ", velocity_east_m_s: " << ground_speed.velocity_east_m_s
               << ", velocity_down_m_s: " << ground_speed.velocity_down_m_s << "]";
}

bool operator==(const Telemetry::RCStatus& lhs, const Telemetry::RCStatus& rhs)
{
    return lhs.available_once == rhs.available_once && lhs.available == rhs.available &&
           lhs.signal_strength_percent == rhs.signal_strength_percent;
}

std::ostream& operator<<(std::ostream& str, Telemetry::RCStatus const& rc_status)
{
    return str << "[was_available_once: " << rc_status.available_once
               << ", is_available: " << rc_status.available
               << ", signal_strength_percent: " << rc_status.signal_strength_percent << "]";
}

bool operator==(const Telemetry::StatusText& lhs, const Telemetry::StatusText& rhs)
{
    return lhs.text == rhs.text && lhs.type == rhs.type;
}

std::ostream& operator<<(std::ostream& str, Telemetry::StatusText const& status_text)
{
    return str << "[statustext: " << status_text.text << "]";
}

bool operator==(
    const Telemetry::ActuatorControlTarget& lhs, const Telemetry::ActuatorControlTarget& rhs)
{
    if (lhs.group != rhs.group)
        return false;
    for (int i = 0; i < 8; i++) {
        if (lhs.controls[i] != rhs.controls[i])
            return false;
    }
    return true;
}

std::ostream&
operator<<(std::ostream& str, Telemetry::ActuatorControlTarget const& actuator_control_target)
{
    str << "Group:  " << static_cast<int>(actuator_control_target.group) << ", Controls: [";
    for (int i = 0; i < 8; i++) {
        str << actuator_control_target.controls[i];
        if (i != 7) {
            str << ", ";
        } else {
            str << "]";
        }
    }
    return str;
}

bool operator==(
    const Telemetry::ActuatorOutputStatus& lhs, const Telemetry::ActuatorOutputStatus& rhs)
{
    if (lhs.active != rhs.active)
        return false;
    for (unsigned i = 0; i < lhs.active; i++) {
        if (lhs.actuator[i] != rhs.actuator[i])
            return false;
    }
    return true;
}

std::ostream&
operator<<(std::ostream& str, Telemetry::ActuatorOutputStatus const& actuator_output_status)
{
    str << "Active:  " << actuator_output_status.active << ", Actuators: [";
    for (unsigned i = 0; i < actuator_output_status.active; i++) {
        str << actuator_output_status.actuator[i];
        if (i != (actuator_output_status.active - 1)) {
            str << ", ";
        } else {
            str << "]" << std::endl;
        }
    }
    return str;
}

bool operator==(const Telemetry::PositionBody& lhs, const Telemetry::PositionBody& rhs)
{
    return (
        std::abs(lhs.x_m - rhs.x_m) > std::numeric_limits<float>::epsilon() ||
        std::abs(lhs.y_m - rhs.y_m) > std::numeric_limits<float>::epsilon() ||
        std::abs(lhs.z_m - rhs.z_m) > std::numeric_limits<float>::epsilon());
}

bool operator!=(const Telemetry::PositionBody& lhs, const Telemetry::PositionBody& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& str, Telemetry::PositionBody const& position_body)
{
    return str << "[x_m: " << position_body.x_m << ", y_m: " << position_body.y_m
               << ", z_m: " << position_body.z_m << "]";
}

bool operator==(const Telemetry::SpeedBody& lhs, const Telemetry::SpeedBody& rhs)
{
    return (
        std::abs(lhs.x_m_s - rhs.x_m_s) > std::numeric_limits<float>::epsilon() ||
        std::abs(lhs.y_m_s - rhs.y_m_s) > std::numeric_limits<float>::epsilon() ||
        std::abs(lhs.z_m_s - rhs.z_m_s) > std::numeric_limits<float>::epsilon());
}

bool operator!=(const Telemetry::SpeedBody& lhs, const Telemetry::SpeedBody& rhs)
{
    return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& str, Telemetry::SpeedBody const& speed_body)
{
    return str << "[x_m_s: " << speed_body.x_m_s << ", y_m_s: " << speed_body.y_m_s
               << ", z_m_s: " << speed_body.z_m_s << "]";
}

bool operator==(const Telemetry::Odometry& lhs, const Telemetry::Odometry& rhs)
{
    // FixMe: Should we check time_usec, reset_counter equality?
    if (lhs.time_usec != rhs.time_usec || lhs.frame_id != rhs.frame_id ||
        lhs.child_frame_id != rhs.child_frame_id || lhs.reset_counter != rhs.reset_counter ||
        lhs.position_body != rhs.position_body || lhs.q != rhs.q ||
        lhs.velocity_body != rhs.velocity_body ||
        lhs.angular_velocity_body != rhs.angular_velocity_body)
        return false;

    const bool lhs_pose_cov_nan = std::isnan(lhs.pose_covariance[0]);
    const bool rhs_pose_cov_nan = std::isnan(rhs.pose_covariance[0]);

    if (lhs_pose_cov_nan != rhs_pose_cov_nan)
        return false;

    if (!lhs_pose_cov_nan) {
        for (int i = 0; i < 21; i++) {
            if (std::abs(lhs.pose_covariance[i] - rhs.pose_covariance[i]) >
                std::numeric_limits<float>::epsilon())
                return false;
        }
    }

    const bool lhs_velocity_cov_nan = std::isnan(lhs.velocity_covariance[0]);
    const bool rhs_velocity_cov_nan = std::isnan(rhs.velocity_covariance[0]);

    if (lhs_velocity_cov_nan != rhs_velocity_cov_nan)
        return false;

    if (!lhs_velocity_cov_nan) {
        for (int i = 0; i < 21; i++) {
            if (std::abs(lhs.velocity_covariance[i] - rhs.velocity_covariance[i]) >
                std::numeric_limits<float>::epsilon())
                return false;
        }
    }

    return true;
}

std::ostream& operator<<(std::ostream& str, Telemetry::Odometry const& odometry)
{
    str << "[time_usec: " << odometry.time_usec
        << ", frame_id: " << static_cast<int>(odometry.frame_id)
        << ", child_frame_id: " << static_cast<int>(odometry.child_frame_id);
    str << ", position_body: " << odometry.position_body << ", q: " << odometry.q
        << ", velocity_body: " << odometry.velocity_body
        << ", angular_velocity_body: " << odometry.angular_velocity_body;
    str << ", pose_covariance: [";
    for (unsigned i = 0; i < 21; i++) {
        str << odometry.pose_covariance[i];
        if (i != 20) {
            str << ", ";
        } else {
            str << "]";
        }
    }
    str << ", velocity_covariance: [";
    for (unsigned i = 0; i < 21; i++) {
        str << odometry.velocity_covariance[i];
        if (i != 20) {
            str << ", ";
        } else {
            str << "]";
        }
    }
    str << ", reset_counter: " << static_cast<int>(odometry.reset_counter);
    return str;
}

std::ostream& operator<<(std::ostream& str, Telemetry::FlightMode const& flight_mode)
{
    return str << Telemetry::flight_mode_str(flight_mode);
}

std::ostream& operator<<(std::ostream& str, Telemetry::LandedState const& landed_state)
{
    return str << Telemetry::landed_state_str(landed_state);
}

} // namespace mavsdk
