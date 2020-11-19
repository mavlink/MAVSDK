#pragma once

#include <atomic>
#include <mutex>

#include "plugins/telemetry/telemetry.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "system.h"

// Since not all vehicles support/require level calibration, this
// is disabled for now.
//#define LEVEL_CALIBRATION

namespace mavsdk {

class System;

class TelemetryImpl : public PluginImplBase {
public:
    explicit TelemetryImpl(System& system);
    explicit TelemetryImpl(std::shared_ptr<System> system);
    ~TelemetryImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Telemetry::Result set_rate_position_velocity_ned(double rate_hz);
    Telemetry::Result set_rate_position(double rate_hz);
    Telemetry::Result set_rate_home(double rate_hz);
    Telemetry::Result set_rate_in_air(double rate_hz);
    Telemetry::Result set_rate_landed_state(double rate_hz);
    Telemetry::Result set_rate_attitude(double rate_hz);
    Telemetry::Result set_rate_camera_attitude(double rate_hz);
    Telemetry::Result set_rate_velocity_ned(double rate_hz);
    Telemetry::Result set_rate_imu(double rate_hz);
    Telemetry::Result set_rate_fixedwing_metrics(double rate_hz);
    Telemetry::Result set_rate_ground_truth(double rate_hz);
    Telemetry::Result set_rate_gps_info(double rate_hz);
    Telemetry::Result set_rate_battery(double rate_hz);
    Telemetry::Result set_rate_rc_status(double rate_hz);
    Telemetry::Result set_rate_actuator_control_target(double rate_hz);
    Telemetry::Result set_rate_actuator_output_status(double rate_hz);
    Telemetry::Result set_rate_odometry(double rate_hz);
    Telemetry::Result set_rate_distance_sensor(double rate_hz);
    Telemetry::Result set_rate_unix_epoch_time(double rate_hz);

    void set_rate_position_velocity_ned_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_position_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_home_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_in_air_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_landed_state_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_attitude_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_camera_attitude_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_velocity_ned_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_imu_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_fixedwing_metrics_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_ground_truth_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_gps_info_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_battery_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_rc_status_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_actuator_control_target_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_actuator_output_status_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_odometry_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_distance_sensor_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_unix_epoch_time_async(double rate_hz, Telemetry::ResultCallback callback);

    void get_gps_global_origin_async(const Telemetry::GetGpsGlobalOriginCallback callback);
    std::pair<Telemetry::Result, Telemetry::GpsGlobalOrigin> get_gps_global_origin();

    Telemetry::PositionVelocityNed position_velocity_ned() const;
    Telemetry::Position position() const;
    Telemetry::Position home() const;
    bool in_air() const;
    bool armed() const;
    Telemetry::LandedState landed_state() const;
    Telemetry::StatusText status_text() const;
    Telemetry::EulerAngle attitude_euler() const;
    Telemetry::Quaternion attitude_quaternion() const;
    Telemetry::AngularVelocityBody attitude_angular_velocity_body() const;
    Telemetry::GroundTruth ground_truth() const;
    Telemetry::FixedwingMetrics fixedwing_metrics() const;
    Telemetry::EulerAngle camera_attitude_euler() const;
    Telemetry::Quaternion camera_attitude_quaternion() const;
    Telemetry::VelocityNed velocity_ned() const;
    Telemetry::Imu imu() const;
    Telemetry::GpsInfo gps_info() const;
    Telemetry::Battery battery() const;
    Telemetry::FlightMode flight_mode() const;
    Telemetry::Health health() const;
    bool health_all_ok() const;
    Telemetry::RcStatus rc_status() const;
    Telemetry::ActuatorControlTarget actuator_control_target() const;
    Telemetry::ActuatorOutputStatus actuator_output_status() const;
    Telemetry::Odometry odometry() const;
    Telemetry::DistanceSensor distance_sensor() const;
    uint64_t unix_epoch_time() const;

    void position_velocity_ned_async(Telemetry::PositionVelocityNedCallback& callback);
    void position_async(Telemetry::PositionCallback& callback);
    void home_async(Telemetry::PositionCallback& callback);
    void in_air_async(Telemetry::InAirCallback& callback);
    void status_text_async(Telemetry::StatusTextCallback& callback);
    void armed_async(Telemetry::ArmedCallback& callback);
    void attitude_quaternion_async(Telemetry::AttitudeQuaternionCallback& callback);
    void attitude_euler_async(Telemetry::AttitudeEulerCallback& callback);
    void
    attitude_angular_velocity_body_async(Telemetry::AttitudeAngularVelocityBodyCallback& callback);
    void fixedwing_metrics_async(Telemetry::FixedwingMetricsCallback& callback);
    void ground_truth_async(Telemetry::GroundTruthCallback& callback);
    void camera_attitude_quaternion_async(Telemetry::AttitudeQuaternionCallback& callback);
    void camera_attitude_euler_async(Telemetry::AttitudeEulerCallback& callback);
    void velocity_ned_async(Telemetry::VelocityNedCallback& callback);
    void imu_async(Telemetry::ImuCallback& callback);
    void gps_info_async(Telemetry::GpsInfoCallback& callback);
    void battery_async(Telemetry::BatteryCallback& callback);
    void flight_mode_async(Telemetry::FlightModeCallback& callback);
    void health_async(Telemetry::HealthCallback& callback);
    void health_all_ok_async(Telemetry::HealthAllOkCallback& callback);
    void landed_state_async(Telemetry::LandedStateCallback& callback);
    void rc_status_async(Telemetry::RcStatusCallback& callback);
    void unix_epoch_time_async(Telemetry::UnixEpochTimeCallback& callback);
    void actuator_control_target_async(Telemetry::ActuatorControlTargetCallback& callback);
    void actuator_output_status_async(Telemetry::ActuatorOutputStatusCallback& callback);
    void odometry_async(Telemetry::OdometryCallback& callback);
    void distance_sensor_async(Telemetry::DistanceSensorCallback& callback);

    TelemetryImpl(const TelemetryImpl&) = delete;
    TelemetryImpl& operator=(const TelemetryImpl&) = delete;

private:
    void set_position_velocity_ned(Telemetry::PositionVelocityNed position_velocity_ned);
    void set_position(Telemetry::Position position);
    void set_home_position(Telemetry::Position home_position);
    void set_in_air(bool in_air);
    void set_landed_state(Telemetry::LandedState landed_state);
    void set_status_text(Telemetry::StatusText status_text);
    void set_armed(bool armed);
    void set_attitude_quaternion(Telemetry::Quaternion quaternion);
    void set_attitude_angular_velocity_body(Telemetry::AngularVelocityBody angular_velocity_body);
    void set_fixedwing_metrics(Telemetry::FixedwingMetrics fixedwing_metrics);
    void set_ground_truth(Telemetry::GroundTruth ground_truth);
    void set_camera_attitude_euler_angle(Telemetry::EulerAngle euler_angle);
    void set_velocity_ned(Telemetry::VelocityNed velocity_ned);
    void set_imu_reading_ned(Telemetry::Imu imu);
    void set_gps_info(Telemetry::GpsInfo gps_info);
    void set_battery(Telemetry::Battery battery);
    void set_health_local_position(bool ok);
    void set_health_global_position(bool ok);
    void set_health_home_position(bool ok);
    void set_health_gyrometer_calibration(bool ok);
    void set_health_accelerometer_calibration(bool ok);
    void set_health_magnetometer_calibration(bool ok);
    void set_health_level_calibration(bool ok);
    void set_rc_status(bool available, float signal_strength_percent);
    void set_unix_epoch_time_us(uint64_t time_us);
    void set_actuator_control_target(uint8_t group, const std::vector<float>& controls);
    void set_actuator_output_status(uint32_t active, const std::vector<float>& actuators);
    void set_odometry(Telemetry::Odometry& odometry);
    void set_distance_sensor(Telemetry::DistanceSensor& distance_sensor);

    void process_position_velocity_ned(const mavlink_message_t& message);
    void process_global_position_int(const mavlink_message_t& message);
    void process_home_position(const mavlink_message_t& message);
    void process_attitude(const mavlink_message_t& message);
    void process_attitude_quaternion(const mavlink_message_t& message);
    void process_gimbal_device_attitude_status(const mavlink_message_t& message);
    void process_mount_orientation(const mavlink_message_t& message);
    void process_imu_reading_ned(const mavlink_message_t& message);
    void process_gps_raw_int(const mavlink_message_t& message);
    void process_ground_truth(const mavlink_message_t& message);
    void process_extended_sys_state(const mavlink_message_t& message);
    void process_fixedwing_metrics(const mavlink_message_t& message);
    void process_sys_status(const mavlink_message_t& message);
    void process_heartbeat(const mavlink_message_t& message);
    void process_statustext(const mavlink_message_t& message);
    void process_rc_channels(const mavlink_message_t& message);
    void process_unix_epoch_time(const mavlink_message_t& message);
    void process_actuator_control_target(const mavlink_message_t& message);
    void process_actuator_output_status(const mavlink_message_t& message);
    void process_odometry(const mavlink_message_t& message);
    void process_distance_sensor(const mavlink_message_t& message);
    void receive_param_cal_gyro(MAVLinkParameters::Result result, int value);
    void receive_param_cal_accel(MAVLinkParameters::Result result, int value);
    void receive_param_cal_mag(MAVLinkParameters::Result result, int value);

    void process_parameter_update(const std::string& name);
#ifdef LEVEL_CALIBRATION
    void receive_param_cal_level(MAVLinkParameters::Result result, float value);
#endif
    void receive_param_hitl(MAVLinkParameters::Result result, int value);

    void receive_rc_channels_timeout();
    void receive_gps_raw_timeout();
    void receive_unix_epoch_timeout();

    static Telemetry::Result
    telemetry_result_from_command_result(MavlinkCommandSender::Result command_result);

    static void command_result_callback(
        MavlinkCommandSender::Result command_result, const Telemetry::ResultCallback& callback);

    static Telemetry::LandedState to_landed_state(mavlink_extended_sys_state_t extended_sys_state);

    static Telemetry::FlightMode
    telemetry_flight_mode_from_flight_mode(SystemImpl::FlightMode flight_mode);

    // Make all fields thread-safe using mutexs
    // The mutexs are mutable so that the lock can get aqcuired in
    // methods marked const.
    mutable std::mutex _position_mutex{};
    Telemetry::Position _position{};

    mutable std::mutex _position_velocity_ned_mutex{};
    Telemetry::PositionVelocityNed _position_velocity_ned{};

    mutable std::mutex _home_position_mutex{};
    Telemetry::Position _home_position{};

    // If possible, just use atomic instead of a mutex.
    std::atomic_bool _in_air{false};
    std::atomic_bool _armed{false};

    mutable std::mutex _status_text_mutex{};
    Telemetry::StatusText _status_text{};

    mutable std::mutex _attitude_quaternion_mutex{};
    Telemetry::Quaternion _attitude_quaternion{};

    mutable std::mutex _camera_attitude_euler_angle_mutex{};
    Telemetry::EulerAngle _camera_attitude_euler_angle{};

    mutable std::mutex _attitude_angular_velocity_body_mutex{};
    Telemetry::AngularVelocityBody _attitude_angular_velocity_body{};

    mutable std::mutex _ground_truth_mutex{};
    Telemetry::GroundTruth _ground_truth{};

    mutable std::mutex _fixedwing_metrics_mutex{};
    Telemetry::FixedwingMetrics _fixedwing_metrics{};

    mutable std::mutex _velocity_ned_mutex{};
    Telemetry::VelocityNed _velocity_ned{};

    mutable std::mutex _imu_reading_ned_mutex{};
    Telemetry::Imu _imu_reading_ned{};

    mutable std::mutex _gps_info_mutex{};
    Telemetry::GpsInfo _gps_info{};

    mutable std::mutex _battery_mutex{};
    Telemetry::Battery _battery{};

    mutable std::mutex _health_mutex{};
    Telemetry::Health _health{};

    mutable std::mutex _landed_state_mutex{};
    Telemetry::LandedState _landed_state{Telemetry::LandedState::Unknown};

    mutable std::mutex _rc_status_mutex{};
    Telemetry::RcStatus _rc_status{};

    mutable std::mutex _unix_epoch_time_mutex{};
    uint64_t _unix_epoch_time_us{};

    mutable std::mutex _actuator_control_target_mutex{};
    Telemetry::ActuatorControlTarget _actuator_control_target{};

    mutable std::mutex _actuator_output_status_mutex{};
    Telemetry::ActuatorOutputStatus _actuator_output_status{};

    mutable std::mutex _odometry_mutex{};
    Telemetry::Odometry _odometry{};

    mutable std::mutex _distance_sensor_mutex{};
    Telemetry::DistanceSensor _distance_sensor{};

    std::atomic<bool> _hitl_enabled{false};

    std::mutex _subscription_mutex{};
    Telemetry::PositionVelocityNedCallback _position_velocity_ned_subscription{nullptr};
    Telemetry::PositionCallback _position_subscription{nullptr};
    Telemetry::PositionCallback _home_position_subscription{nullptr};
    Telemetry::InAirCallback _in_air_subscription{nullptr};
    Telemetry::StatusTextCallback _status_text_subscription{nullptr};
    Telemetry::ArmedCallback _armed_subscription{nullptr};
    Telemetry::AttitudeQuaternionCallback _attitude_quaternion_angle_subscription{nullptr};
    Telemetry::AttitudeAngularVelocityBodyCallback _attitude_angular_velocity_body_subscription{
        nullptr};
    Telemetry::GroundTruthCallback _ground_truth_subscription{nullptr};
    Telemetry::FixedwingMetricsCallback _fixedwing_metrics_subscription{nullptr};
    Telemetry::AttitudeEulerCallback _attitude_euler_angle_subscription{nullptr};
    Telemetry::AttitudeQuaternionCallback _camera_attitude_quaternion_subscription{nullptr};
    Telemetry::AttitudeEulerCallback _camera_attitude_euler_angle_subscription{nullptr};
    Telemetry::VelocityNedCallback _velocity_ned_subscription{nullptr};
    Telemetry::ImuCallback _imu_reading_ned_subscription{nullptr};
    Telemetry::GpsInfoCallback _gps_info_subscription{nullptr};
    Telemetry::BatteryCallback _battery_subscription{nullptr};
    Telemetry::FlightModeCallback _flight_mode_subscription{nullptr};
    Telemetry::HealthCallback _health_subscription{nullptr};
    Telemetry::HealthAllOkCallback _health_all_ok_subscription{nullptr};
    Telemetry::LandedStateCallback _landed_state_subscription{nullptr};
    Telemetry::RcStatusCallback _rc_status_subscription{nullptr};
    Telemetry::UnixEpochTimeCallback _unix_epoch_time_subscription{nullptr};
    Telemetry::ActuatorControlTargetCallback _actuator_control_target_subscription{nullptr};
    Telemetry::ActuatorOutputStatusCallback _actuator_output_status_subscription{nullptr};
    Telemetry::OdometryCallback _odometry_subscription{nullptr};
    Telemetry::DistanceSensorCallback _distance_sensor_subscription{nullptr};

    // The velocity (former ground speed) and position are coupled to the same message, therefore,
    // we just use the faster between the two.
    double _velocity_ned_rate_hz{0.0};
    double _position_rate_hz{-1.0};

    void* _rc_channels_timeout_cookie{nullptr};
    void* _gps_raw_timeout_cookie{nullptr};
    void* _unix_epoch_timeout_cookie{nullptr};
};
} // namespace mavsdk
