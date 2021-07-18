#include "telemetry_impl.h"
#include "system.h"
#include "math_conversions.h"
#include "global_include.h"
#include <cmath>
#include <functional>
#include <string>
#include <array>
#include <cassert>

namespace mavsdk {

TelemetryImpl::TelemetryImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

TelemetryImpl::TelemetryImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

TelemetryImpl::~TelemetryImpl()
{
    _parent->unregister_plugin(this);
}

void TelemetryImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOCAL_POSITION_NED,
        std::bind(&TelemetryImpl::process_position_velocity_ned, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GLOBAL_POSITION_INT,
        std::bind(&TelemetryImpl::process_global_position_int, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HOME_POSITION,
        std::bind(&TelemetryImpl::process_home_position, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_ATTITUDE, std::bind(&TelemetryImpl::process_attitude, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_ATTITUDE_QUATERNION,
        std::bind(&TelemetryImpl::process_attitude_quaternion, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_MOUNT_ORIENTATION,
        std::bind(&TelemetryImpl::process_mount_orientation, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GIMBAL_DEVICE_ATTITUDE_STATUS,
        std::bind(&TelemetryImpl::process_gimbal_device_attitude_status, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GPS_RAW_INT, std::bind(&TelemetryImpl::process_gps_raw_int, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_EXTENDED_SYS_STATE,
        std::bind(&TelemetryImpl::process_extended_sys_state, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_SYS_STATUS, std::bind(&TelemetryImpl::process_sys_status, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_BATTERY_STATUS,
        std::bind(&TelemetryImpl::process_battery_status, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HEARTBEAT, std::bind(&TelemetryImpl::process_heartbeat, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_RC_CHANNELS, std::bind(&TelemetryImpl::process_rc_channels, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_ACTUATOR_CONTROL_TARGET,
        std::bind(&TelemetryImpl::process_actuator_control_target, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_ACTUATOR_OUTPUT_STATUS,
        std::bind(&TelemetryImpl::process_actuator_output_status, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_ODOMETRY, std::bind(&TelemetryImpl::process_odometry, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_DISTANCE_SENSOR,
        std::bind(&TelemetryImpl::process_distance_sensor, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_SCALED_PRESSURE,
        std::bind(&TelemetryImpl::process_scaled_pressure, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_UTM_GLOBAL_POSITION,
        std::bind(&TelemetryImpl::process_unix_epoch_time, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HIGHRES_IMU,
        std::bind(&TelemetryImpl::process_imu_reading_ned, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_SCALED_IMU, std::bind(&TelemetryImpl::process_scaled_imu, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_RAW_IMU, std::bind(&TelemetryImpl::process_raw_imu, this, _1), this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_VFR_HUD,
        std::bind(&TelemetryImpl::process_fixedwing_metrics, this, _1),
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_HIL_STATE_QUATERNION,
        std::bind(&TelemetryImpl::process_ground_truth, this, _1),
        this);

    _parent->register_param_changed_handler(
        std::bind(&TelemetryImpl::process_parameter_update, this, _1), this);

    _parent->register_statustext_handler(
        [this](const MavlinkStatustextHandler::Statustext& statustext) {
            receive_statustext(statustext);
        },
        this);
}

void TelemetryImpl::deinit()
{
    _parent->unregister_statustext_handler(this);
    _parent->unregister_timeout_handler(_gps_raw_timeout_cookie);
    _parent->unregister_timeout_handler(_unix_epoch_timeout_cookie);
    _parent->unregister_param_changed_handler(this);
    _parent->unregister_all_mavlink_message_handlers(this);
}

void TelemetryImpl::enable()
{
    _parent->register_timeout_handler(
        std::bind(&TelemetryImpl::receive_gps_raw_timeout, this), 2.0, &_gps_raw_timeout_cookie);

    _parent->register_timeout_handler(
        std::bind(&TelemetryImpl::receive_unix_epoch_timeout, this),
        2.0,
        &_unix_epoch_timeout_cookie);

    // FIXME: The calibration check should eventually be better than this.
    //        For now, we just do the same as QGC does.

#ifndef ARDUPILOT
    if (_parent->has_autopilot()) {
        _parent->get_param_int_async(
            std::string("CAL_GYRO0_ID"),
            std::bind(
                &TelemetryImpl::receive_param_cal_gyro,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            this);

        _parent->get_param_int_async(
            std::string("CAL_ACC0_ID"),
            std::bind(
                &TelemetryImpl::receive_param_cal_accel,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            this);

        _parent->get_param_int_async(
            std::string("CAL_MAG0_ID"),
            std::bind(
                &TelemetryImpl::receive_param_cal_mag,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            this);

        _parent->get_param_int_async(
            std::string("SYS_HITL"),
            std::bind(
                &TelemetryImpl::receive_param_hitl,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            this);
    }
#endif // ifndef ARDUPILOT
}

void TelemetryImpl::disable() {}

Telemetry::Result TelemetryImpl::set_rate_position_velocity_ned(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_LOCAL_POSITION_NED, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_position(double rate_hz)
{
    _position_rate_hz = rate_hz;
    double max_rate_hz = std::max(_position_rate_hz, _velocity_ned_rate_hz);

    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_GLOBAL_POSITION_INT, max_rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_home(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_HOME_POSITION, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_in_air(double rate_hz)
{
    return set_rate_landed_state(rate_hz);
}

Telemetry::Result TelemetryImpl::set_rate_landed_state(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_EXTENDED_SYS_STATE, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_attitude(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_ATTITUDE_QUATERNION, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_camera_attitude(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_MOUNT_ORIENTATION, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_velocity_ned(double rate_hz)
{
    _velocity_ned_rate_hz = rate_hz;
    double max_rate_hz = std::max(_position_rate_hz, _velocity_ned_rate_hz);

    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_GLOBAL_POSITION_INT, max_rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_imu(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_HIGHRES_IMU, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_scaled_imu(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_SCALED_IMU, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_raw_imu(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_RAW_IMU, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_fixedwing_metrics(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_VFR_HUD, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_ground_truth(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_HIL_STATE_QUATERNION, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_gps_info(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_GPS_RAW_INT, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_battery(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_SYS_STATUS, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_rc_status(double rate_hz)
{
    UNUSED(rate_hz);
    LogWarn() << "System status is usually fixed at 1 Hz";
    return Telemetry::Result::Unsupported;
}

Telemetry::Result TelemetryImpl::set_rate_actuator_control_target(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_ACTUATOR_CONTROL_TARGET, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_actuator_output_status(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_ACTUATOR_OUTPUT_STATUS, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_odometry(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_ODOMETRY, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_distance_sensor(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_DISTANCE_SENSOR, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_scaled_pressure(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_SCALED_PRESSURE, rate_hz));
}

Telemetry::Result TelemetryImpl::set_rate_unix_epoch_time(double rate_hz)
{
    return telemetry_result_from_command_result(
        _parent->set_msg_rate(MAVLINK_MSG_ID_UTM_GLOBAL_POSITION, rate_hz));
}

void TelemetryImpl::set_rate_position_velocity_ned_async(
    double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_LOCAL_POSITION_NED,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_position_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _position_rate_hz = rate_hz;
    double max_rate_hz = std::max(_position_rate_hz, _velocity_ned_rate_hz);

    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_GLOBAL_POSITION_INT,
        max_rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_home_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_HOME_POSITION,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_in_air_async(double rate_hz, Telemetry::ResultCallback callback)
{
    set_rate_landed_state_async(rate_hz, callback);
}

void TelemetryImpl::set_rate_landed_state_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_EXTENDED_SYS_STATE,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_attitude_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_ATTITUDE_QUATERNION,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_camera_attitude_async(
    double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_MOUNT_ORIENTATION,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_velocity_ned_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _velocity_ned_rate_hz = rate_hz;
    double max_rate_hz = std::max(_position_rate_hz, _velocity_ned_rate_hz);

    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_GLOBAL_POSITION_INT,
        max_rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_imu_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_HIGHRES_IMU,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_scaled_imu_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_SCALED_IMU,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_raw_imu_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_RAW_IMU,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_fixedwing_metrics_async(
    double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_VFR_HUD,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_ground_truth_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_HIL_STATE_QUATERNION,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_gps_info_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_GPS_RAW_INT,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_battery_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_SYS_STATUS,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_rc_status_async(double rate_hz, Telemetry::ResultCallback callback)
{
    UNUSED(rate_hz);
    LogWarn() << "System status is usually fixed at 1 Hz";
    _parent->call_user_callback([callback]() { callback(Telemetry::Result::Unsupported); });
}

void TelemetryImpl::set_rate_unix_epoch_time_async(
    double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_UTM_GLOBAL_POSITION,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_actuator_control_target_async(
    double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_ACTUATOR_CONTROL_TARGET,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_actuator_output_status_async(
    double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_ACTUATOR_OUTPUT_STATUS,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_odometry_async(double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_ODOMETRY,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_distance_sensor_async(
    double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_DISTANCE_SENSOR,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

void TelemetryImpl::set_rate_scaled_pressure_async(
    double rate_hz, Telemetry::ResultCallback callback)
{
    _parent->set_msg_rate_async(
        MAVLINK_MSG_ID_SCALED_PRESSURE,
        rate_hz,
        std::bind(&TelemetryImpl::command_result_callback, std::placeholders::_1, callback));
}

Telemetry::Result
TelemetryImpl::telemetry_result_from_command_result(MavlinkCommandSender::Result command_result)
{
    switch (command_result) {
        case MavlinkCommandSender::Result::Success:
            return Telemetry::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return Telemetry::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            return Telemetry::Result::ConnectionError;
        case MavlinkCommandSender::Result::Busy:
            return Telemetry::Result::Busy;
        case MavlinkCommandSender::Result::CommandDenied:
            return Telemetry::Result::CommandDenied;
        case MavlinkCommandSender::Result::Timeout:
            return Telemetry::Result::Timeout;
        case MavlinkCommandSender::Result::Unsupported:
            return Telemetry::Result::Unsupported;
        default:
            return Telemetry::Result::Unknown;
    }
}

void TelemetryImpl::command_result_callback(
    MavlinkCommandSender::Result command_result, const Telemetry::ResultCallback& callback)
{
    Telemetry::Result action_result = telemetry_result_from_command_result(command_result);

    callback(action_result);
}

void TelemetryImpl::process_position_velocity_ned(const mavlink_message_t& message)
{
    mavlink_local_position_ned_t local_position;
    mavlink_msg_local_position_ned_decode(&message, &local_position);

    Telemetry::PositionVelocityNed position_velocity;
    position_velocity.position.north_m = local_position.x;
    position_velocity.position.east_m = local_position.y;
    position_velocity.position.down_m = local_position.z;
    position_velocity.velocity.north_m_s = local_position.vx;
    position_velocity.velocity.east_m_s = local_position.vy;
    position_velocity.velocity.down_m_s = local_position.vz;

    set_position_velocity_ned(position_velocity);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_position_velocity_ned_subscription) {
        auto callback = _position_velocity_ned_subscription;
        auto arg = position_velocity_ned();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    set_health_local_position(true);
}

void TelemetryImpl::process_global_position_int(const mavlink_message_t& message)
{
    mavlink_global_position_int_t global_position_int;
    mavlink_msg_global_position_int_decode(&message, &global_position_int);

    {
        Telemetry::Position position;
        position.latitude_deg = global_position_int.lat * 1e-7;
        position.longitude_deg = global_position_int.lon * 1e-7;
        position.absolute_altitude_m = global_position_int.alt * 1e-3f;
        position.relative_altitude_m = global_position_int.relative_alt * 1e-3f;
        set_position(position);
    }

    {
        Telemetry::VelocityNed velocity;
        velocity.north_m_s = global_position_int.vx * 1e-2f;
        velocity.east_m_s = global_position_int.vy * 1e-2f;
        velocity.down_m_s = global_position_int.vz * 1e-2f;
        set_velocity_ned(velocity);
    }

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_position_subscription) {
        auto callback = _position_subscription;
        auto arg = position();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (_velocity_ned_subscription) {
        auto callback = _velocity_ned_subscription;
        auto arg = velocity_ned();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_home_position(const mavlink_message_t& message)
{
    mavlink_home_position_t home_position;
    mavlink_msg_home_position_decode(&message, &home_position);
    Telemetry::Position new_pos;
    new_pos.latitude_deg = home_position.latitude * 1e-7;
    new_pos.longitude_deg = home_position.longitude * 1e-7;
    new_pos.absolute_altitude_m = home_position.altitude * 1e-3f;
    new_pos.relative_altitude_m = 0.0f; // 0 by definition.

    set_home_position(new_pos);

    set_health_home_position(true);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_home_position_subscription) {
        auto callback = _home_position_subscription;
        auto arg = home();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_attitude(const mavlink_message_t& message)
{
    mavlink_attitude_t attitude;
    mavlink_msg_attitude_decode(&message, &attitude);

    Telemetry::EulerAngle euler_angle;
    euler_angle.roll_deg = to_deg_from_rad(attitude.roll);
    euler_angle.pitch_deg = to_deg_from_rad(attitude.pitch);
    euler_angle.yaw_deg = to_deg_from_rad(attitude.yaw);
    euler_angle.timestamp_us = static_cast<uint64_t>(attitude.time_boot_ms) * 1000;

    Telemetry::AngularVelocityBody angular_velocity_body;
    angular_velocity_body.roll_rad_s = attitude.rollspeed;
    angular_velocity_body.pitch_rad_s = attitude.pitchspeed;
    angular_velocity_body.yaw_rad_s = attitude.yawspeed;
    set_attitude_angular_velocity_body(angular_velocity_body);

    auto quaternion = mavsdk::to_quaternion_from_euler_angle(euler_angle);
    set_attitude_quaternion(quaternion);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_attitude_quaternion_angle_subscription) {
        auto callback = _attitude_quaternion_angle_subscription;
        auto arg = attitude_quaternion();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (_attitude_euler_angle_subscription) {
        auto callback = _attitude_euler_angle_subscription;
        auto arg = attitude_euler();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (_attitude_angular_velocity_body_subscription) {
        auto callback = _attitude_angular_velocity_body_subscription;
        auto arg = attitude_angular_velocity_body();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_attitude_quaternion(const mavlink_message_t& message)
{
    mavlink_attitude_quaternion_t mavlink_attitude_quaternion;
    mavlink_msg_attitude_quaternion_decode(&message, &mavlink_attitude_quaternion);

    Telemetry::Quaternion quaternion;
    quaternion.w = mavlink_attitude_quaternion.q1;
    quaternion.x = mavlink_attitude_quaternion.q2;
    quaternion.y = mavlink_attitude_quaternion.q3;
    quaternion.z = mavlink_attitude_quaternion.q4;
    quaternion.timestamp_us =
        static_cast<uint64_t>(mavlink_attitude_quaternion.time_boot_ms) * 1000;

    Telemetry::AngularVelocityBody angular_velocity_body;
    angular_velocity_body.roll_rad_s = mavlink_attitude_quaternion.rollspeed;
    angular_velocity_body.pitch_rad_s = mavlink_attitude_quaternion.pitchspeed;
    angular_velocity_body.yaw_rad_s = mavlink_attitude_quaternion.yawspeed;

    set_attitude_quaternion(quaternion);

    set_attitude_angular_velocity_body(angular_velocity_body);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_attitude_quaternion_angle_subscription) {
        auto callback = _attitude_quaternion_angle_subscription;
        auto arg = attitude_quaternion();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (_attitude_euler_angle_subscription) {
        auto callback = _attitude_euler_angle_subscription;
        auto arg = attitude_euler();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (_attitude_angular_velocity_body_subscription) {
        auto callback = _attitude_angular_velocity_body_subscription;
        auto arg = attitude_angular_velocity_body();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_mount_orientation(const mavlink_message_t& message)
{
    // TODO: remove this one once we move all the way to gimbal v2 protocol
    mavlink_mount_orientation_t mount_orientation;
    mavlink_msg_mount_orientation_decode(&message, &mount_orientation);

    Telemetry::EulerAngle euler_angle;
    euler_angle.roll_deg = mount_orientation.roll;
    euler_angle.pitch_deg = mount_orientation.pitch;
    euler_angle.yaw_deg = mount_orientation.yaw_absolute;

    set_camera_attitude_euler_angle(euler_angle);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_camera_attitude_quaternion_subscription) {
        auto callback = _camera_attitude_quaternion_subscription;
        auto arg = camera_attitude_quaternion();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (_camera_attitude_euler_angle_subscription) {
        auto callback = _camera_attitude_euler_angle_subscription;
        auto arg = camera_attitude_euler();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_gimbal_device_attitude_status(const mavlink_message_t& message)
{
    // We accept both MOUNT_ORIENTATION and GIMBAL_DEVICE_ATTITUDE_STATUS for now,
    // in order to support both gimbal v1 and v2 protocols.

    mavlink_gimbal_device_attitude_status_t attitude_status;
    mavlink_msg_gimbal_device_attitude_status_decode(&message, &attitude_status);

    Telemetry::Quaternion q;
    q.w = attitude_status.q[0];
    q.x = attitude_status.q[1];
    q.y = attitude_status.q[2];
    q.z = attitude_status.q[3];

    Telemetry::EulerAngle euler_angle = to_euler_angle_from_quaternion(q);

    set_camera_attitude_euler_angle(euler_angle);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_camera_attitude_quaternion_subscription) {
        auto callback = _camera_attitude_quaternion_subscription;
        auto arg = camera_attitude_quaternion();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (_camera_attitude_euler_angle_subscription) {
        auto callback = _camera_attitude_euler_angle_subscription;
        auto arg = camera_attitude_euler();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_imu_reading_ned(const mavlink_message_t& message)
{
    mavlink_highres_imu_t highres_imu;
    mavlink_msg_highres_imu_decode(&message, &highres_imu);
    Telemetry::Imu new_imu;
    new_imu.acceleration_frd.forward_m_s2 = highres_imu.xacc;
    new_imu.acceleration_frd.right_m_s2 = highres_imu.yacc;
    new_imu.acceleration_frd.down_m_s2 = highres_imu.zacc;
    new_imu.angular_velocity_frd.forward_rad_s = highres_imu.xgyro;
    new_imu.angular_velocity_frd.right_rad_s = highres_imu.ygyro;
    new_imu.angular_velocity_frd.down_rad_s = highres_imu.zgyro;
    new_imu.magnetic_field_frd.forward_gauss = highres_imu.xmag;
    new_imu.magnetic_field_frd.right_gauss = highres_imu.ymag;
    new_imu.magnetic_field_frd.down_gauss = highres_imu.zmag;
    new_imu.temperature_degc = highres_imu.temperature;
    new_imu.timestamp_us = highres_imu.time_usec;

    set_imu_reading_ned(new_imu);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_imu_reading_ned_subscription) {
        auto callback = _imu_reading_ned_subscription;
        auto arg = imu();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_scaled_imu(const mavlink_message_t& message)
{
    mavlink_scaled_imu_t scaled_imu_reading;
    mavlink_msg_scaled_imu_decode(&message, &scaled_imu_reading);
    Telemetry::Imu new_imu;
    new_imu.acceleration_frd.forward_m_s2 = scaled_imu_reading.xacc;
    new_imu.acceleration_frd.right_m_s2 = scaled_imu_reading.yacc;
    new_imu.acceleration_frd.down_m_s2 = scaled_imu_reading.zacc;
    new_imu.angular_velocity_frd.forward_rad_s = scaled_imu_reading.xgyro;
    new_imu.angular_velocity_frd.right_rad_s = scaled_imu_reading.ygyro;
    new_imu.angular_velocity_frd.down_rad_s = scaled_imu_reading.zgyro;
    new_imu.magnetic_field_frd.forward_gauss = scaled_imu_reading.xmag;
    new_imu.magnetic_field_frd.right_gauss = scaled_imu_reading.ymag;
    new_imu.magnetic_field_frd.down_gauss = scaled_imu_reading.zmag;
    new_imu.temperature_degc = static_cast<float>(scaled_imu_reading.temperature) * 1e-2f;
    new_imu.timestamp_us = static_cast<uint64_t>(scaled_imu_reading.time_boot_ms) * 1000;

    set_scaled_imu(new_imu);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_scaled_imu_subscription) {
        auto callback = _scaled_imu_subscription;
        auto arg = scaled_imu();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_raw_imu(const mavlink_message_t& message)
{
    mavlink_raw_imu_t raw_imu_reading;
    mavlink_msg_raw_imu_decode(&message, &raw_imu_reading);
    Telemetry::Imu new_imu;
    new_imu.acceleration_frd.forward_m_s2 = raw_imu_reading.xacc;
    new_imu.acceleration_frd.right_m_s2 = raw_imu_reading.yacc;
    new_imu.acceleration_frd.down_m_s2 = raw_imu_reading.zacc;
    new_imu.angular_velocity_frd.forward_rad_s = raw_imu_reading.xgyro;
    new_imu.angular_velocity_frd.right_rad_s = raw_imu_reading.ygyro;
    new_imu.angular_velocity_frd.down_rad_s = raw_imu_reading.zgyro;
    new_imu.magnetic_field_frd.forward_gauss = raw_imu_reading.xmag;
    new_imu.magnetic_field_frd.right_gauss = raw_imu_reading.ymag;
    new_imu.magnetic_field_frd.down_gauss = raw_imu_reading.zmag;
    new_imu.temperature_degc = static_cast<float>(raw_imu_reading.temperature) * 1e-2f;
    new_imu.timestamp_us = raw_imu_reading.time_usec;

    set_raw_imu(new_imu);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_raw_imu_subscription) {
        auto callback = _raw_imu_subscription;
        auto arg = raw_imu();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_gps_raw_int(const mavlink_message_t& message)
{
    mavlink_gps_raw_int_t gps_raw_int;
    mavlink_msg_gps_raw_int_decode(&message, &gps_raw_int);

    Telemetry::FixType fix_type;
    switch (gps_raw_int.fix_type) {
        case 0:
            fix_type = Telemetry::FixType::NoGps;
            break;
        case 1:
            fix_type = Telemetry::FixType::NoFix;
            break;
        case 2:
            fix_type = Telemetry::FixType::Fix2D;
            break;
        case 3:
            fix_type = Telemetry::FixType::Fix3D;
            break;
        case 4:
            fix_type = Telemetry::FixType::FixDgps;
            break;
        case 5:
            fix_type = Telemetry::FixType::RtkFloat;
            break;
        case 6:
            fix_type = Telemetry::FixType::RtkFixed;
            break;

        default:
            LogErr() << "Received unknown GPS fix type!";
            fix_type = Telemetry::FixType::NoGps;
            break;
    }

    Telemetry::GpsInfo new_gps_info;
    new_gps_info.num_satellites = gps_raw_int.satellites_visible;
    new_gps_info.fix_type = fix_type;
    set_gps_info(new_gps_info);

    Telemetry::RawGps raw_gps_info;
    raw_gps_info.timestamp_us = gps_raw_int.time_usec;
    raw_gps_info.latitude_deg = gps_raw_int.lat * 1e-7;
    raw_gps_info.longitude_deg = gps_raw_int.lon * 1e-7;
    raw_gps_info.absolute_altitude_m = gps_raw_int.alt * 1e-3f;
    raw_gps_info.hdop = static_cast<float>(gps_raw_int.eph) * 1e-2f;
    raw_gps_info.vdop = static_cast<float>(gps_raw_int.epv) * 1e-2f;
    raw_gps_info.velocity_m_s = static_cast<float>(gps_raw_int.vel) * 1e-2f;
    raw_gps_info.cog_deg = static_cast<float>(gps_raw_int.cog) * 1e-2f;
    raw_gps_info.altitude_ellipsoid_m = static_cast<float>(gps_raw_int.alt_ellipsoid) * 1e-3f;
    raw_gps_info.horizontal_uncertainty_m = static_cast<float>(gps_raw_int.h_acc) * 1e-3f;
    raw_gps_info.vertical_uncertainty_m = static_cast<float>(gps_raw_int.v_acc) * 1e-3f;
    raw_gps_info.velocity_uncertainty_m_s = static_cast<float>(gps_raw_int.vel_acc) * 1e-3f;
    raw_gps_info.heading_uncertainty_deg = static_cast<float>(gps_raw_int.hdg_acc) * 1e-5f;
    raw_gps_info.yaw_deg = static_cast<float>(gps_raw_int.yaw) * 1e-2f;
    set_raw_gps(raw_gps_info);

    // TODO: This is just an interim hack, we will have to look at
    //       estimator flags in order to decide if the position
    //       estimate is good enough.
    const bool gps_ok = ((gps_raw_int.fix_type >= 3) && (gps_raw_int.satellites_visible >= 8));

    set_health_global_position(gps_ok);

    {
        std::lock_guard<std::mutex> lock(_subscription_mutex);
        if (_gps_info_subscription) {
            auto callback = _gps_info_subscription;
            auto arg = gps_info();
            _parent->call_user_callback([callback, arg]() { callback(arg); });
        }
        if (_raw_gps_subscription) {
            auto callback = _raw_gps_subscription;
            auto arg = raw_gps();
            _parent->call_user_callback([callback, arg]() { callback(arg); });
        }
    }

    _parent->refresh_timeout_handler(_gps_raw_timeout_cookie);
}

void TelemetryImpl::process_ground_truth(const mavlink_message_t& message)
{
    mavlink_hil_state_quaternion_t hil_state_quaternion;
    mavlink_msg_hil_state_quaternion_decode(&message, &hil_state_quaternion);

    Telemetry::GroundTruth new_ground_truth;
    new_ground_truth.latitude_deg = hil_state_quaternion.lat * 1e-7;
    new_ground_truth.longitude_deg = hil_state_quaternion.lon * 1e-7;
    new_ground_truth.absolute_altitude_m = hil_state_quaternion.alt * 1e-3f;

    set_ground_truth(new_ground_truth);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_ground_truth_subscription) {
        auto callback = _ground_truth_subscription;
        auto arg = ground_truth();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_extended_sys_state(const mavlink_message_t& message)
{
    mavlink_extended_sys_state_t extended_sys_state;
    mavlink_msg_extended_sys_state_decode(&message, &extended_sys_state);

    {
        Telemetry::LandedState landed_state = to_landed_state(extended_sys_state);
        set_landed_state(landed_state);
    }

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_landed_state_subscription) {
        auto callback = _landed_state_subscription;
        auto arg = landed_state();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (extended_sys_state.landed_state == MAV_LANDED_STATE_IN_AIR ||
        extended_sys_state.landed_state == MAV_LANDED_STATE_TAKEOFF ||
        extended_sys_state.landed_state == MAV_LANDED_STATE_LANDING) {
        set_in_air(true);
    } else if (extended_sys_state.landed_state == MAV_LANDED_STATE_ON_GROUND) {
        set_in_air(false);
    }
    // If landed_state is undefined, we use what we have received last.

    if (_in_air_subscription) {
        auto callback = _in_air_subscription;
        auto arg = in_air();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}
void TelemetryImpl::process_fixedwing_metrics(const mavlink_message_t& message)
{
    mavlink_vfr_hud_t vfr_hud;
    mavlink_msg_vfr_hud_decode(&message, &vfr_hud);

    Telemetry::FixedwingMetrics new_fixedwing_metrics;
    new_fixedwing_metrics.airspeed_m_s = vfr_hud.airspeed;
    new_fixedwing_metrics.throttle_percentage = vfr_hud.throttle * 1e-2f;
    new_fixedwing_metrics.climb_rate_m_s = vfr_hud.climb;

    set_fixedwing_metrics(new_fixedwing_metrics);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_fixedwing_metrics_subscription) {
        auto callback = _fixedwing_metrics_subscription;
        auto arg = fixedwing_metrics();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_sys_status(const mavlink_message_t& message)
{
    mavlink_sys_status_t sys_status;
    mavlink_msg_sys_status_decode(&message, &sys_status);

    if (!_has_bat_status) {
        Telemetry::Battery new_battery;
        new_battery.voltage_v = sys_status.voltage_battery * 1e-3f;
        // FIXME: it is strange calling it percent when the range goes from 0 to 1.
        new_battery.remaining_percent = sys_status.battery_remaining * 1e-2f;

        set_battery(new_battery);

        {
            std::lock_guard<std::mutex> lock(_subscription_mutex);
            if (_battery_subscription) {
                auto callback = _battery_subscription;
                auto arg = battery();
                _parent->call_user_callback([callback, arg]() { callback(arg); });
            }
        }
    }

    const bool rc_ok =
        sys_status.onboard_control_sensors_health & MAV_SYS_STATUS_SENSOR_RC_RECEIVER;

    set_rc_status({rc_ok}, std::nullopt);

    {
        std::lock_guard<std::mutex> lock(_subscription_mutex);
        if (_rc_status_subscription) {
            auto callback = _rc_status_subscription;
            auto arg = rc_status();
            _parent->call_user_callback([callback, arg]() { callback(arg); });
        }
    }

    const bool armable = sys_status.onboard_control_sensors_health & MAV_SYS_STATUS_PREARM_CHECK;

    set_health_armable(armable);
    if (_health_all_ok_subscription) {
        auto callback = _health_all_ok_subscription;
        auto arg = health_all_ok();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_battery_status(const mavlink_message_t& message)
{
    mavlink_battery_status_t bat_status;
    mavlink_msg_battery_status_decode(&message, &bat_status);

    _has_bat_status = true;

    Telemetry::Battery new_battery;
    new_battery.id = bat_status.id;
    new_battery.voltage_v = bat_status.voltages[0] * 1e-3f;
    // FIXME: it is strange calling it percent when the range goes from 0 to 1.
    new_battery.remaining_percent = bat_status.battery_remaining * 1e-2f;

    set_battery(new_battery);

    {
        std::lock_guard<std::mutex> lock(_subscription_mutex);
        if (_battery_subscription) {
            auto callback = _battery_subscription;
            auto arg = battery();
            _parent->call_user_callback([callback, arg]() { callback(arg); });
        }
    }
}

void TelemetryImpl::process_heartbeat(const mavlink_message_t& message)
{
    if (message.compid != MAV_COMP_ID_AUTOPILOT1) {
        return;
    }

    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);

    set_armed(((heartbeat.base_mode & MAV_MODE_FLAG_SAFETY_ARMED) ? true : false));

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_armed_subscription) {
        auto callback = _armed_subscription;
        auto arg = armed();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (_flight_mode_subscription) {
        auto callback = _flight_mode_subscription;
        // The flight mode is already parsed in SystemImpl, so we can take it
        // from there.  This assumes that SystemImpl gets called first because
        // it's earlier in the callback list.
        auto arg = telemetry_flight_mode_from_flight_mode(_parent->get_flight_mode());
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    if (_health_subscription) {
        auto callback = _health_subscription;
        auto arg = health();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
    if (_health_all_ok_subscription) {
        auto callback = _health_all_ok_subscription;
        auto arg = health_all_ok();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::receive_statustext(const MavlinkStatustextHandler::Statustext& statustext)
{
    Telemetry::StatusText new_status_text;

    switch (statustext.severity) {
        case MAV_SEVERITY_EMERGENCY:
            new_status_text.type = Telemetry::StatusTextType::Emergency;
            break;
        case MAV_SEVERITY_ALERT:
            new_status_text.type = Telemetry::StatusTextType::Alert;
            break;
        case MAV_SEVERITY_CRITICAL:
            new_status_text.type = Telemetry::StatusTextType::Critical;
            break;
        case MAV_SEVERITY_ERROR:
            new_status_text.type = Telemetry::StatusTextType::Error;
            break;
        case MAV_SEVERITY_WARNING:
            new_status_text.type = Telemetry::StatusTextType::Warning;
            break;
        case MAV_SEVERITY_NOTICE:
            new_status_text.type = Telemetry::StatusTextType::Notice;
            break;
        case MAV_SEVERITY_INFO:
            new_status_text.type = Telemetry::StatusTextType::Info;
            break;
        case MAV_SEVERITY_DEBUG:
            new_status_text.type = Telemetry::StatusTextType::Debug;
            break;
        default:
            LogWarn() << "Unknown StatusText severity";
            new_status_text.type = Telemetry::StatusTextType::Info;
            break;
    }

    new_status_text.text = statustext.text;

    set_status_text(new_status_text);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_status_text_subscription) {
        auto callback = _status_text_subscription;
        auto arg = status_text();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_rc_channels(const mavlink_message_t& message)
{
    mavlink_rc_channels_t rc_channels;
    mavlink_msg_rc_channels_decode(&message, &rc_channels);

    if (rc_channels.rssi != UINT8_MAX) {
        set_rc_status(std::nullopt, {rc_channels.rssi});

        std::lock_guard<std::mutex> lock(_subscription_mutex);
        if (_rc_status_subscription) {
            auto callback = _rc_status_subscription;
            auto arg = rc_status();
            _parent->call_user_callback([callback, arg]() { callback(arg); });
        }
    }

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_rc_status_subscription) {
        auto callback = _rc_status_subscription;
        auto arg = rc_status();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    _parent->refresh_timeout_handler(_rc_channels_timeout_cookie);
}

void TelemetryImpl::process_unix_epoch_time(const mavlink_message_t& message)
{
    mavlink_utm_global_position_t utm_global_position;
    mavlink_msg_utm_global_position_decode(&message, &utm_global_position);

    set_unix_epoch_time_us(utm_global_position.time);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_unix_epoch_time_subscription) {
        auto callback = _unix_epoch_time_subscription;
        auto arg = unix_epoch_time();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }

    _parent->refresh_timeout_handler(_unix_epoch_timeout_cookie);
}

void TelemetryImpl::process_actuator_control_target(const mavlink_message_t& message)
{
    mavlink_set_actuator_control_target_t target;
    mavlink_msg_set_actuator_control_target_decode(&message, &target);

    uint32_t group = target.group_mlx;
    std::vector<float> controls;

    const unsigned control_size = sizeof(target.controls) / sizeof(target.controls[0]);
    // Can't use std::copy because target is packed.
    for (std::size_t i = 0; i < control_size; ++i) {
        controls.push_back(target.controls[i]);
    }

    set_actuator_control_target(group, controls);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_actuator_control_target_subscription) {
        auto callback = _actuator_control_target_subscription;
        auto arg = actuator_control_target();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_actuator_output_status(const mavlink_message_t& message)
{
    mavlink_actuator_output_status_t status;
    mavlink_msg_actuator_output_status_decode(&message, &status);

    uint32_t active = status.active;
    std::vector<float> actuators;

    const unsigned actuators_size = sizeof(status.actuator) / sizeof(status.actuator[0]);
    // Can't use std::copy because status is packed.
    for (std::size_t i = 0; i < actuators_size; ++i) {
        actuators.push_back(status.actuator[i]);
    }

    set_actuator_output_status(active, actuators);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_actuator_output_status_subscription) {
        auto callback = _actuator_output_status_subscription;
        auto arg = actuator_output_status();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_odometry(const mavlink_message_t& message)
{
    mavlink_odometry_t odometry_msg;
    mavlink_msg_odometry_decode(&message, &odometry_msg);

    Telemetry::Odometry odometry_struct{};

    odometry_struct.time_usec = odometry_msg.time_usec;
    odometry_struct.frame_id = static_cast<Telemetry::Odometry::MavFrame>(odometry_msg.frame_id);
    odometry_struct.child_frame_id =
        static_cast<Telemetry::Odometry::MavFrame>(odometry_msg.child_frame_id);

    odometry_struct.position_body.x_m = odometry_msg.x;
    odometry_struct.position_body.y_m = odometry_msg.y;
    odometry_struct.position_body.z_m = odometry_msg.z;

    odometry_struct.q.w = odometry_msg.q[0];
    odometry_struct.q.x = odometry_msg.q[1];
    odometry_struct.q.y = odometry_msg.q[2];
    odometry_struct.q.z = odometry_msg.q[3];

    odometry_struct.velocity_body.x_m_s = odometry_msg.vx;
    odometry_struct.velocity_body.y_m_s = odometry_msg.vy;
    odometry_struct.velocity_body.z_m_s = odometry_msg.vz;

    odometry_struct.angular_velocity_body.roll_rad_s = odometry_msg.rollspeed;
    odometry_struct.angular_velocity_body.pitch_rad_s = odometry_msg.pitchspeed;
    odometry_struct.angular_velocity_body.yaw_rad_s = odometry_msg.yawspeed;

    const std::size_t len_pose_covariance =
        sizeof(odometry_msg.pose_covariance) / sizeof(odometry_msg.pose_covariance[0]);
    for (std::size_t i = 0; i < len_pose_covariance; ++i) {
        odometry_struct.pose_covariance.covariance_matrix.push_back(
            odometry_msg.pose_covariance[i]);
    }

    const std::size_t len_velocity_covariance =
        sizeof(odometry_msg.velocity_covariance) / sizeof(odometry_msg.velocity_covariance[0]);
    for (std::size_t i = 0; i < len_velocity_covariance; ++i) {
        odometry_struct.velocity_covariance.covariance_matrix.push_back(
            odometry_msg.velocity_covariance[i]);
    }

    set_odometry(odometry_struct);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_odometry_subscription) {
        auto callback = _odometry_subscription;
        auto arg = odometry();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_distance_sensor(const mavlink_message_t& message)
{
    mavlink_distance_sensor_t distance_sensor_msg;
    mavlink_msg_distance_sensor_decode(&message, &distance_sensor_msg);

    Telemetry::DistanceSensor distance_sensor_struct{};

    distance_sensor_struct.minimum_distance_m = distance_sensor_msg.min_distance;
    distance_sensor_struct.maximum_distance_m = distance_sensor_msg.max_distance;
    distance_sensor_struct.current_distance_m = distance_sensor_msg.current_distance;

    set_distance_sensor(distance_sensor_struct);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_distance_sensor_subscription) {
        auto callback = _distance_sensor_subscription;
        auto arg = distance_sensor();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void TelemetryImpl::process_scaled_pressure(const mavlink_message_t& message)
{
    mavlink_scaled_pressure_t scaled_pressure_msg;
    mavlink_msg_scaled_pressure_decode(&message, &scaled_pressure_msg);

    Telemetry::ScaledPressure scaled_pressure_struct{};

    scaled_pressure_struct.timestamp_us =
        static_cast<uint64_t>(scaled_pressure_msg.time_boot_ms) * 1000;
    scaled_pressure_struct.absolute_pressure_hpa = scaled_pressure_msg.press_abs;
    scaled_pressure_struct.differential_pressure_hpa = scaled_pressure_msg.press_diff;
    scaled_pressure_struct.temperature_deg =
        static_cast<float>(scaled_pressure_msg.temperature) * 1e-2f;
    scaled_pressure_struct.differential_pressure_temperature_deg =
        static_cast<float>(scaled_pressure_msg.temperature_press_diff) * 1e-2f;

    set_scaled_pressure(scaled_pressure_struct);

    std::lock_guard<std::mutex> lock(_subscription_mutex);
    if (_scaled_pressure_subscription) {
        auto callback = _scaled_pressure_subscription;
        auto arg = scaled_pressure();
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

Telemetry::LandedState
TelemetryImpl::to_landed_state(mavlink_extended_sys_state_t extended_sys_state)
{
    switch (extended_sys_state.landed_state) {
        case MAV_LANDED_STATE_IN_AIR:
            return Telemetry::LandedState::InAir;
        case MAV_LANDED_STATE_TAKEOFF:
            return Telemetry::LandedState::TakingOff;
        case MAV_LANDED_STATE_LANDING:
            return Telemetry::LandedState::Landing;
        case MAV_LANDED_STATE_ON_GROUND:
            return Telemetry::LandedState::OnGround;
        default:
            return Telemetry::LandedState::Unknown;
    }
}

Telemetry::FlightMode
TelemetryImpl::telemetry_flight_mode_from_flight_mode(SystemImpl::FlightMode flight_mode)
{
    switch (flight_mode) {
        case SystemImpl::FlightMode::Ready:
            return Telemetry::FlightMode::Ready;
        case SystemImpl::FlightMode::Takeoff:
            return Telemetry::FlightMode::Takeoff;
        case SystemImpl::FlightMode::Hold:
            return Telemetry::FlightMode::Hold;
        case SystemImpl::FlightMode::Mission:
            return Telemetry::FlightMode::Mission;
        case SystemImpl::FlightMode::ReturnToLaunch:
            return Telemetry::FlightMode::ReturnToLaunch;
        case SystemImpl::FlightMode::Land:
            return Telemetry::FlightMode::Land;
        case SystemImpl::FlightMode::Offboard:
            return Telemetry::FlightMode::Offboard;
        case SystemImpl::FlightMode::FollowMe:
            return Telemetry::FlightMode::FollowMe;
        case SystemImpl::FlightMode::Manual:
            return Telemetry::FlightMode::Manual;
        case SystemImpl::FlightMode::Posctl:
            return Telemetry::FlightMode::Posctl;
        case SystemImpl::FlightMode::Altctl:
            return Telemetry::FlightMode::Altctl;
        case SystemImpl::FlightMode::Rattitude:
            return Telemetry::FlightMode::Rattitude;
        case SystemImpl::FlightMode::Acro:
            return Telemetry::FlightMode::Acro;
        case SystemImpl::FlightMode::Stabilized:
            return Telemetry::FlightMode::Stabilized;
        default:
            return Telemetry::FlightMode::Unknown;
    }
}

void TelemetryImpl::receive_param_cal_gyro(MAVLinkParameters::Result result, int value)
{
    if (result != MAVLinkParameters::Result::Success) {
        LogErr() << "Error: Param for gyro cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_health_gyrometer_calibration(ok);
}

void TelemetryImpl::receive_param_cal_accel(MAVLinkParameters::Result result, int value)
{
    if (result != MAVLinkParameters::Result::Success) {
        LogErr() << "Error: Param for accel cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_health_accelerometer_calibration(ok);
}

void TelemetryImpl::receive_param_cal_mag(MAVLinkParameters::Result result, int value)
{
    if (result != MAVLinkParameters::Result::Success) {
        LogErr() << "Error: Param for mag cal failed.";
        return;
    }

    bool ok = (value != 0);
    set_health_magnetometer_calibration(ok);
}

void TelemetryImpl::receive_param_hitl(MAVLinkParameters::Result result, int value)
{
    if (result != MAVLinkParameters::Result::Success) {
        LogErr() << "Error: Param to determine hitl failed.";
        return;
    }

    _hitl_enabled = (value > 0);

    // assume sensor calibration ok in hitl
    if (_hitl_enabled) {
        set_health_accelerometer_calibration(true);
        set_health_gyrometer_calibration(true);
        set_health_magnetometer_calibration(true);
    }
}

void TelemetryImpl::receive_gps_raw_timeout()
{
    const bool position_ok = false;
    set_health_local_position(position_ok);
    set_health_global_position(position_ok);
}

void TelemetryImpl::receive_unix_epoch_timeout()
{
    const uint64_t unix_epoch = 0;
    set_unix_epoch_time_us(unix_epoch);
}

Telemetry::PositionVelocityNed TelemetryImpl::position_velocity_ned() const
{
    std::lock_guard<std::mutex> lock(_position_velocity_ned_mutex);
    return _position_velocity_ned;
}

void TelemetryImpl::set_position_velocity_ned(Telemetry::PositionVelocityNed position_velocity_ned)
{
    std::lock_guard<std::mutex> lock(_position_velocity_ned_mutex);
    _position_velocity_ned = position_velocity_ned;
}

Telemetry::Position TelemetryImpl::position() const
{
    std::lock_guard<std::mutex> lock(_position_mutex);
    return _position;
}

void TelemetryImpl::set_position(Telemetry::Position position)
{
    std::lock_guard<std::mutex> lock(_position_mutex);
    _position = position;
}

Telemetry::Position TelemetryImpl::home() const
{
    std::lock_guard<std::mutex> lock(_home_position_mutex);
    return _home_position;
}

void TelemetryImpl::set_home_position(Telemetry::Position home_position)
{
    std::lock_guard<std::mutex> lock(_home_position_mutex);
    _home_position = home_position;
}

bool TelemetryImpl::armed() const
{
    return _armed;
}

bool TelemetryImpl::in_air() const
{
    return _in_air;
}

void TelemetryImpl::set_in_air(bool in_air_new)
{
    _in_air = in_air_new;
}

void TelemetryImpl::set_status_text(Telemetry::StatusText status_text)
{
    std::lock_guard<std::mutex> lock(_status_text_mutex);
    _status_text = status_text;
}

Telemetry::StatusText TelemetryImpl::status_text() const
{
    std::lock_guard<std::mutex> lock(_status_text_mutex);
    return _status_text;
}

void TelemetryImpl::set_armed(bool armed_new)
{
    _armed = armed_new;
}

Telemetry::Quaternion TelemetryImpl::attitude_quaternion() const
{
    std::lock_guard<std::mutex> lock(_attitude_quaternion_mutex);
    return _attitude_quaternion;
}

Telemetry::AngularVelocityBody TelemetryImpl::attitude_angular_velocity_body() const
{
    std::lock_guard<std::mutex> lock(_attitude_angular_velocity_body_mutex);
    return _attitude_angular_velocity_body;
}

Telemetry::GroundTruth TelemetryImpl::ground_truth() const
{
    std::lock_guard<std::mutex> lock(_ground_truth_mutex);
    return _ground_truth;
}

Telemetry::FixedwingMetrics TelemetryImpl::fixedwing_metrics() const
{
    std::lock_guard<std::mutex> lock(_fixedwing_metrics_mutex);
    return _fixedwing_metrics;
}

Telemetry::EulerAngle TelemetryImpl::attitude_euler() const
{
    std::lock_guard<std::mutex> lock(_attitude_quaternion_mutex);
    Telemetry::EulerAngle euler = to_euler_angle_from_quaternion(_attitude_quaternion);

    return euler;
}

void TelemetryImpl::set_attitude_quaternion(Telemetry::Quaternion quaternion)
{
    std::lock_guard<std::mutex> lock(_attitude_quaternion_mutex);
    _attitude_quaternion = quaternion;
}

void TelemetryImpl::set_attitude_angular_velocity_body(
    Telemetry::AngularVelocityBody angular_velocity_body)
{
    std::lock_guard<std::mutex> lock(_attitude_quaternion_mutex);
    _attitude_angular_velocity_body = angular_velocity_body;
}

void TelemetryImpl::set_ground_truth(Telemetry::GroundTruth ground_truth)
{
    std::lock_guard<std::mutex> lock(_ground_truth_mutex);
    _ground_truth = ground_truth;
}

void TelemetryImpl::set_fixedwing_metrics(Telemetry::FixedwingMetrics fixedwing_metrics)
{
    std::lock_guard<std::mutex> lock(_fixedwing_metrics_mutex);
    _fixedwing_metrics = fixedwing_metrics;
}

Telemetry::Quaternion TelemetryImpl::camera_attitude_quaternion() const
{
    std::lock_guard<std::mutex> lock(_camera_attitude_euler_angle_mutex);
    Telemetry::Quaternion quaternion = to_quaternion_from_euler_angle(_camera_attitude_euler_angle);

    return quaternion;
}

Telemetry::EulerAngle TelemetryImpl::camera_attitude_euler() const
{
    std::lock_guard<std::mutex> lock(_camera_attitude_euler_angle_mutex);

    return _camera_attitude_euler_angle;
}

void TelemetryImpl::set_camera_attitude_euler_angle(Telemetry::EulerAngle euler_angle)
{
    std::lock_guard<std::mutex> lock(_camera_attitude_euler_angle_mutex);
    _camera_attitude_euler_angle = euler_angle;
}

Telemetry::VelocityNed TelemetryImpl::velocity_ned() const
{
    std::lock_guard<std::mutex> lock(_velocity_ned_mutex);
    return _velocity_ned;
}

void TelemetryImpl::set_velocity_ned(Telemetry::VelocityNed velocity_ned)
{
    std::lock_guard<std::mutex> lock(_velocity_ned_mutex);
    _velocity_ned = velocity_ned;
}

Telemetry::Imu TelemetryImpl::imu() const
{
    std::lock_guard<std::mutex> lock(_imu_reading_ned_mutex);
    return _imu_reading_ned;
}

void TelemetryImpl::set_imu_reading_ned(Telemetry::Imu imu_reading_ned)
{
    std::lock_guard<std::mutex> lock(_imu_reading_ned_mutex);
    _imu_reading_ned = imu_reading_ned;
}

Telemetry::Imu TelemetryImpl::scaled_imu() const
{
    std::lock_guard<std::mutex> lock(_scaled_imu_mutex);
    return _scaled_imu;
}

void TelemetryImpl::set_scaled_imu(Telemetry::Imu scaled_imu)
{
    std::lock_guard<std::mutex> lock(_scaled_imu_mutex);
    _scaled_imu = scaled_imu;
}

Telemetry::Imu TelemetryImpl::raw_imu() const
{
    std::lock_guard<std::mutex> lock(_raw_imu_mutex);
    return _raw_imu;
}

void TelemetryImpl::set_raw_imu(Telemetry::Imu raw_imu)
{
    std::lock_guard<std::mutex> lock(_raw_imu_mutex);
    _raw_imu = raw_imu;
}

Telemetry::GpsInfo TelemetryImpl::gps_info() const
{
    std::lock_guard<std::mutex> lock(_gps_info_mutex);
    return _gps_info;
}

void TelemetryImpl::set_gps_info(Telemetry::GpsInfo gps_info)
{
    std::lock_guard<std::mutex> lock(_gps_info_mutex);
    _gps_info = gps_info;
}

Telemetry::RawGps TelemetryImpl::raw_gps() const
{
    std::lock_guard<std::mutex> lock(_raw_gps_mutex);
    return _raw_gps;
}

void TelemetryImpl::set_raw_gps(Telemetry::RawGps raw_gps)
{
    std::lock_guard<std::mutex> lock(_raw_gps_mutex);
    _raw_gps = raw_gps;
}

Telemetry::Battery TelemetryImpl::battery() const
{
    std::lock_guard<std::mutex> lock(_battery_mutex);
    return _battery;
}

void TelemetryImpl::set_battery(Telemetry::Battery battery)
{
    std::lock_guard<std::mutex> lock(_battery_mutex);
    _battery = battery;
}

Telemetry::FlightMode TelemetryImpl::flight_mode() const
{
    return telemetry_flight_mode_from_flight_mode(_parent->get_flight_mode());
}

Telemetry::Health TelemetryImpl::health() const
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    return _health;
}

bool TelemetryImpl::health_all_ok() const
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    if (_health.is_gyrometer_calibration_ok && _health.is_accelerometer_calibration_ok &&
        _health.is_magnetometer_calibration_ok && _health.is_local_position_ok &&
        _health.is_global_position_ok && _health.is_home_position_ok) {
        return true;
    } else {
        return false;
    }
}

Telemetry::RcStatus TelemetryImpl::rc_status() const
{
    std::lock_guard<std::mutex> lock(_rc_status_mutex);
    return _rc_status;
}

uint64_t TelemetryImpl::unix_epoch_time() const
{
    std::lock_guard<std::mutex> lock(_unix_epoch_time_mutex);
    return _unix_epoch_time_us;
}

Telemetry::ActuatorControlTarget TelemetryImpl::actuator_control_target() const
{
    std::lock_guard<std::mutex> lock(_actuator_control_target_mutex);
    return _actuator_control_target;
}

Telemetry::ActuatorOutputStatus TelemetryImpl::actuator_output_status() const
{
    std::lock_guard<std::mutex> lock(_actuator_output_status_mutex);
    return _actuator_output_status;
}

Telemetry::Odometry TelemetryImpl::odometry() const
{
    std::lock_guard<std::mutex> lock(_odometry_mutex);
    return _odometry;
}

Telemetry::DistanceSensor TelemetryImpl::distance_sensor() const
{
    std::lock_guard<std::mutex> lock(_distance_sensor_mutex);
    return _distance_sensor;
}

Telemetry::ScaledPressure TelemetryImpl::scaled_pressure() const
{
    std::lock_guard<std::mutex> lock(_scaled_pressure_mutex);
    return _scaled_pressure;
}

void TelemetryImpl::set_health_local_position(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.is_local_position_ok = ok;
}

void TelemetryImpl::set_health_global_position(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.is_global_position_ok = ok;
}

void TelemetryImpl::set_health_home_position(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.is_home_position_ok = ok;
}

void TelemetryImpl::set_health_gyrometer_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.is_gyrometer_calibration_ok = (ok || _hitl_enabled);
}

void TelemetryImpl::set_health_accelerometer_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.is_accelerometer_calibration_ok = (ok || _hitl_enabled);
}

void TelemetryImpl::set_health_magnetometer_calibration(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.is_magnetometer_calibration_ok = (ok || _hitl_enabled);
}

void TelemetryImpl::set_health_armable(bool ok)
{
    std::lock_guard<std::mutex> lock(_health_mutex);
    _health.is_armable = ok;
}

Telemetry::LandedState TelemetryImpl::landed_state() const
{
    std::lock_guard<std::mutex> lock(_landed_state_mutex);
    return _landed_state;
}

void TelemetryImpl::set_landed_state(Telemetry::LandedState landed_state)
{
    std::lock_guard<std::mutex> lock(_landed_state_mutex);
    _landed_state = landed_state;
}

void TelemetryImpl::set_rc_status(
    std::optional<bool> maybe_available, std::optional<float> maybe_signal_strength_percent)
{
    std::lock_guard<std::mutex> lock(_rc_status_mutex);

    if (maybe_available) {
        _rc_status.is_available = maybe_available.value();
        if (maybe_available.value()) {
            _rc_status.was_available_once = true;
        }
    }

    if (maybe_signal_strength_percent) {
        _rc_status.signal_strength_percent = maybe_signal_strength_percent.value();
    }
}

void TelemetryImpl::set_unix_epoch_time_us(uint64_t time_us)
{
    std::lock_guard<std::mutex> lock(_unix_epoch_time_mutex);
    _unix_epoch_time_us = time_us;
}

void TelemetryImpl::set_actuator_control_target(uint8_t group, const std::vector<float>& controls)
{
    std::lock_guard<std::mutex> lock(_actuator_control_target_mutex);
    _actuator_control_target.group = group;
    _actuator_control_target.controls = controls;
}

void TelemetryImpl::set_actuator_output_status(uint32_t active, const std::vector<float>& actuators)
{
    std::lock_guard<std::mutex> lock(_actuator_output_status_mutex);
    _actuator_output_status.active = active;
    _actuator_output_status.actuator = actuators;
}

void TelemetryImpl::set_odometry(Telemetry::Odometry& odometry)
{
    std::lock_guard<std::mutex> lock(_odometry_mutex);
    _odometry = odometry;
}

void TelemetryImpl::set_distance_sensor(Telemetry::DistanceSensor& distance_sensor)
{
    std::lock_guard<std::mutex> lock(_distance_sensor_mutex);
    _distance_sensor = distance_sensor;
}

void TelemetryImpl::set_scaled_pressure(Telemetry::ScaledPressure& scaled_pressure)
{
    std::lock_guard<std::mutex> lock(_scaled_pressure_mutex);
    _scaled_pressure = scaled_pressure;
}

void TelemetryImpl::subscribe_position_velocity_ned(
    Telemetry::PositionVelocityNedCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _position_velocity_ned_subscription = callback;
}

void TelemetryImpl::subscribe_position(Telemetry::PositionCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _position_subscription = callback;
}

void TelemetryImpl::subscribe_home(Telemetry::PositionCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _home_position_subscription = callback;
}

void TelemetryImpl::subscribe_in_air(Telemetry::InAirCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _in_air_subscription = callback;
}

void TelemetryImpl::subscribe_status_text(Telemetry::StatusTextCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _status_text_subscription = callback;
}

void TelemetryImpl::subscribe_armed(Telemetry::ArmedCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _armed_subscription = callback;
}

void TelemetryImpl::subscribe_attitude_quaternion(Telemetry::AttitudeQuaternionCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _attitude_quaternion_angle_subscription = callback;
}

void TelemetryImpl::subscribe_attitude_euler(Telemetry::AttitudeEulerCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _attitude_euler_angle_subscription = callback;
}

void TelemetryImpl::subscribe_attitude_angular_velocity_body(
    Telemetry::AttitudeAngularVelocityBodyCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _attitude_angular_velocity_body_subscription = callback;
}

void TelemetryImpl::subscribe_fixedwing_metrics(Telemetry::FixedwingMetricsCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _fixedwing_metrics_subscription = callback;
}

void TelemetryImpl::subscribe_ground_truth(Telemetry::GroundTruthCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _ground_truth_subscription = callback;
}

void TelemetryImpl::subscribe_camera_attitude_quaternion(
    Telemetry::AttitudeQuaternionCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _camera_attitude_quaternion_subscription = callback;
}

void TelemetryImpl::subscribe_camera_attitude_euler(Telemetry::AttitudeEulerCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _camera_attitude_euler_angle_subscription = callback;
}

void TelemetryImpl::subscribe_velocity_ned(Telemetry::VelocityNedCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _velocity_ned_subscription = callback;
}

void TelemetryImpl::subscribe_imu(Telemetry::ImuCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _imu_reading_ned_subscription = callback;
}

void TelemetryImpl::subscribe_scaled_imu(Telemetry::ScaledImuCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _scaled_imu_subscription = callback;
}

void TelemetryImpl::subscribe_raw_imu(Telemetry::RawImuCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _raw_imu_subscription = callback;
}

void TelemetryImpl::subscribe_gps_info(Telemetry::GpsInfoCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _gps_info_subscription = callback;
}

void TelemetryImpl::subscribe_raw_gps(Telemetry::RawGpsCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _raw_gps_subscription = callback;
}

void TelemetryImpl::subscribe_battery(Telemetry::BatteryCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _battery_subscription = callback;
}

void TelemetryImpl::subscribe_flight_mode(Telemetry::FlightModeCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _flight_mode_subscription = callback;
}

void TelemetryImpl::subscribe_health(Telemetry::HealthCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _health_subscription = callback;
}

void TelemetryImpl::subscribe_health_all_ok(Telemetry::HealthAllOkCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _health_all_ok_subscription = callback;
}

void TelemetryImpl::subscribe_landed_state(Telemetry::LandedStateCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _landed_state_subscription = callback;
}

void TelemetryImpl::subscribe_rc_status(Telemetry::RcStatusCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _rc_status_subscription = callback;
}

void TelemetryImpl::subscribe_unix_epoch_time(Telemetry::UnixEpochTimeCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _unix_epoch_time_subscription = callback;
}

void TelemetryImpl::subscribe_actuator_control_target(
    Telemetry::ActuatorControlTargetCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _actuator_control_target_subscription = callback;
}

void TelemetryImpl::subscribe_actuator_output_status(
    Telemetry::ActuatorOutputStatusCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _actuator_output_status_subscription = callback;
}

void TelemetryImpl::subscribe_odometry(Telemetry::OdometryCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _odometry_subscription = callback;
}

void TelemetryImpl::subscribe_distance_sensor(Telemetry::DistanceSensorCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _distance_sensor_subscription = callback;
}

void TelemetryImpl::subscribe_scaled_pressure(Telemetry::ScaledPressureCallback& callback)
{
    std::lock_guard<std::mutex> lock(_subscription_mutex);
    _scaled_pressure_subscription = callback;
}

void TelemetryImpl::get_gps_global_origin_async(
    const Telemetry::GetGpsGlobalOriginCallback callback)
{
    void* message_cookie{};
    void* timeout_cookie{};
    auto response_received = std::make_shared<bool>(false);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_GPS_GLOBAL_ORIGIN,
        [this, callback, &message_cookie, &timeout_cookie, response_received](
            const mavlink_message_t& mavlink_message) {
            if (*response_received) {
                return;
            }
            *response_received = true;

            _parent->unregister_timeout_handler(timeout_cookie);
            _parent->call_user_callback([this, &message_cookie]() {
                _parent->unregister_mavlink_message_handler(
                    MAVLINK_MSG_ID_GPS_GLOBAL_ORIGIN, &message_cookie);
            });

            mavlink_gps_global_origin_t mavlink_gps_global_origin;
            mavlink_msg_gps_global_origin_decode(&mavlink_message, &mavlink_gps_global_origin);

            Telemetry::GpsGlobalOrigin gps_global_origin;
            gps_global_origin.latitude_deg = mavlink_gps_global_origin.latitude * 1e-7;
            gps_global_origin.longitude_deg = mavlink_gps_global_origin.longitude * 1e-7;
            gps_global_origin.altitude_m = mavlink_gps_global_origin.altitude * 1e-3f;

            _parent->call_user_callback([callback, gps_global_origin]() {
                callback(Telemetry::Result::Success, gps_global_origin);
            });
        },
        &message_cookie);

    MavlinkCommandSender::CommandLong command_request_message;
    command_request_message.command = MAV_CMD_REQUEST_MESSAGE;
    command_request_message.params.param1 = MAVLINK_MSG_ID_GPS_GLOBAL_ORIGIN;

    _parent->send_command_async(command_request_message, nullptr);
    _parent->register_timeout_handler(
        [this, callback, response_received]() {
            if (!*response_received) {
                *response_received = true;
                _parent->call_user_callback([callback]() {
                    callback(Telemetry::Result::Timeout, Telemetry::GpsGlobalOrigin{});
                });
            }
        },
        5, // timeout of 5 seconds
        &timeout_cookie);
}

std::pair<Telemetry::Result, Telemetry::GpsGlobalOrigin> TelemetryImpl::get_gps_global_origin()
{
    auto prom = std::promise<std::pair<Telemetry::Result, Telemetry::GpsGlobalOrigin>>();
    auto fut = prom.get_future();

    get_gps_global_origin_async(
        [&prom](Telemetry::Result result, Telemetry::GpsGlobalOrigin gps_global_origin) {
            prom.set_value(std::make_pair(result, gps_global_origin));
        });

    return fut.get();
}

void TelemetryImpl::process_parameter_update(const std::string& name)
{
    if (name.compare("CAL_GYRO0_ID") == 0) {
        _parent->get_param_int_async(
            std::string("CAL_GYRO0_ID"),
            std::bind(
                &TelemetryImpl::receive_param_cal_gyro,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            this);

    } else if (name.compare("CAL_ACC0_ID") == 0) {
        _parent->get_param_int_async(
            std::string("CAL_ACC0_ID"),
            std::bind(
                &TelemetryImpl::receive_param_cal_accel,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            this);

    } else if (name.compare("CAL_MAG0_ID") == 0) {
        _parent->get_param_int_async(
            std::string("CAL_MAG0_ID"),
            std::bind(
                &TelemetryImpl::receive_param_cal_mag,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            this);

    } else if (name.compare("SYS_HITL") == 0) {
        _parent->get_param_int_async(
            std::string("SYS_HITL"),
            std::bind(
                &TelemetryImpl::receive_param_hitl,
                this,
                std::placeholders::_1,
                std::placeholders::_2),
            this);
    }
}

} // namespace mavsdk
