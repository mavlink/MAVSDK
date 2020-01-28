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
    TelemetryImpl(System& system);
    ~TelemetryImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Telemetry::Result set_rate_position_velocity_ned(double rate_hz);
    Telemetry::Result set_rate_position(double rate_hz);
    Telemetry::Result set_rate_home_position(double rate_hz);
    Telemetry::Result set_rate_in_air(double rate_hz);
    Telemetry::Result set_rate_attitude(double rate_hz);
    Telemetry::Result set_rate_camera_attitude(double rate_hz);
    Telemetry::Result set_rate_ground_speed_ned(double rate_hz);
    Telemetry::Result set_rate_imu_reading_ned(double rate_hz);
    Telemetry::Result set_rate_fixedwing_metrics(double rate_hz);
    Telemetry::Result set_rate_ground_truth(double rate_hz);
    Telemetry::Result set_rate_gps_info(double rate_hz);
    Telemetry::Result set_rate_battery(double rate_hz);
    Telemetry::Result set_rate_rc_status(double rate_hz);
    Telemetry::Result set_rate_actuator_control_target(double rate_hz);
    Telemetry::Result set_rate_actuator_output_status(double rate_hz);
    Telemetry::Result set_rate_odometry(double rate_hz);

    void
    set_rate_position_velocity_ned_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_position_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_home_position_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_in_air_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_attitude_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_camera_attitude_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_ground_speed_ned_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_imu_reading_ned_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_fixedwing_metrics_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_ground_truth_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_gps_info_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_battery_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_rc_status_async(double rate_hz, Telemetry::result_callback_t callback);
    void
    set_rate_actuator_control_target_async(double rate_hz, Telemetry::result_callback_t callback);
    void
    set_rate_actuator_output_status_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_odometry_async(double rate_hz, Telemetry::result_callback_t callback);
    void set_rate_unix_epoch_time_async(double rate_hz, Telemetry::result_callback_t callback);

    Telemetry::PositionVelocityNED get_position_velocity_ned() const;
    Telemetry::Position get_position() const;
    Telemetry::Position get_home_position() const;
    bool in_air() const;
    bool armed() const;
    Telemetry::LandedState get_landed_state() const;
    Telemetry::StatusText get_status_text() const;
    Telemetry::EulerAngle get_attitude_euler_angle() const;
    Telemetry::Quaternion get_attitude_quaternion() const;
    Telemetry::AngularVelocityBody get_attitude_angular_velocity_body() const;
    Telemetry::GroundTruth get_ground_truth() const;
    Telemetry::FixedwingMetrics get_fixedwing_metrics() const;
    Telemetry::EulerAngle get_camera_attitude_euler_angle() const;
    Telemetry::Quaternion get_camera_attitude_quaternion() const;
    Telemetry::GroundSpeedNED get_ground_speed_ned() const;
    Telemetry::IMUReadingNED get_imu_reading_ned() const;
    Telemetry::GPSInfo get_gps_info() const;
    Telemetry::Battery get_battery() const;
    Telemetry::FlightMode get_flight_mode() const;
    Telemetry::Health get_health() const;
    bool get_health_all_ok() const;
    Telemetry::RCStatus get_rc_status() const;
    Telemetry::ActuatorControlTarget get_actuator_control_target() const;
    Telemetry::ActuatorOutputStatus get_actuator_output_status() const;
    Telemetry::Odometry get_odometry() const;
    uint64_t get_unix_epoch_time_us() const;

    void position_velocity_ned_async(Telemetry::position_velocity_ned_callback_t& callback);
    void position_async(Telemetry::position_callback_t& callback);
    void home_position_async(Telemetry::position_callback_t& callback);
    void in_air_async(Telemetry::in_air_callback_t& callback);
    void status_text_async(Telemetry::status_text_callback_t& callback);
    void armed_async(Telemetry::armed_callback_t& callback);
    void attitude_quaternion_async(Telemetry::attitude_quaternion_callback_t& callback);
    void attitude_euler_angle_async(Telemetry::attitude_euler_angle_callback_t& callback);
    void attitude_angular_velocity_body_async(
        Telemetry::attitude_angular_velocity_body_callback_t& callback);
    void fixedwing_metrics_async(Telemetry::fixedwing_metrics_callback_t& callback);
    void ground_truth_async(Telemetry::ground_truth_callback_t& callback);
    void camera_attitude_quaternion_async(Telemetry::attitude_quaternion_callback_t& callback);
    void camera_attitude_euler_angle_async(Telemetry::attitude_euler_angle_callback_t& callback);
    void ground_speed_ned_async(Telemetry::ground_speed_ned_callback_t& callback);
    void imu_reading_ned_async(Telemetry::imu_reading_ned_callback_t& callback);
    void gps_info_async(Telemetry::gps_info_callback_t& callback);
    void battery_async(Telemetry::battery_callback_t& callback);
    void flight_mode_async(Telemetry::flight_mode_callback_t& callback);
    void health_async(Telemetry::health_callback_t& callback);
    void health_all_ok_async(Telemetry::health_all_ok_callback_t& callback);
    void landed_state_async(Telemetry::landed_state_callback_t& callback);
    void rc_status_async(Telemetry::rc_status_callback_t& callback);
    void unix_epoch_time_async(Telemetry::unix_epoch_time_callback_t& callback);
    void actuator_control_target_async(Telemetry::actuator_control_target_callback_t& callback);
    void actuator_output_status_async(Telemetry::actuator_output_status_callback_t& callback);
    void odometry_async(Telemetry::odometry_callback_t& callback);

    TelemetryImpl(const TelemetryImpl&) = delete;
    TelemetryImpl& operator=(const TelemetryImpl&) = delete;

private:
    void set_position_velocity_ned(Telemetry::PositionVelocityNED position_velocity_ned);
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
    void set_ground_speed_ned(Telemetry::GroundSpeedNED ground_speed_ned);
    void set_imu_reading_ned(Telemetry::IMUReadingNED imu_reading_ned);
    void set_gps_info(Telemetry::GPSInfo gps_info);
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
    void set_actuator_control_target(uint8_t group, const std::array<float, 8>& controls);
    void set_actuator_output_status(uint32_t active, const std::array<float, 32>& actuators);
    void set_odometry(Telemetry::Odometry& odometry);

    void process_position_velocity_ned(const mavlink_message_t& message);
    void process_global_position_int(const mavlink_message_t& message);
    void process_home_position(const mavlink_message_t& message);
    void process_attitude(const mavlink_message_t& message);
    void process_attitude_quaternion(const mavlink_message_t& message);
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
    telemetry_result_from_command_result(MAVLinkCommands::Result command_result);

    static void command_result_callback(
        MAVLinkCommands::Result command_result, const Telemetry::result_callback_t& callback);

    static Telemetry::LandedState to_landed_state(mavlink_extended_sys_state_t extended_sys_state);

    static Telemetry::FlightMode
    telemetry_flight_mode_from_flight_mode(SystemImpl::FlightMode flight_mode);

    // Make all fields thread-safe using mutexs
    // The mutexs are mutable so that the lock can get aqcuired in
    // methods marked const.
    mutable std::mutex _position_mutex{};
    Telemetry::Position _position{double(NAN), double(NAN), NAN, NAN};

    mutable std::mutex _position_velocity_ned_mutex{};
    Telemetry::PositionVelocityNED _position_velocity_ned{{NAN, NAN, NAN}, {NAN, NAN, NAN}};

    mutable std::mutex _home_position_mutex{};
    Telemetry::Position _home_position{double(NAN), double(NAN), NAN, NAN};

    // If possible, just use atomic instead of a mutex.
    std::atomic_bool _in_air{false};
    std::atomic_bool _armed{false};

    mutable std::mutex _status_text_mutex{};
    Telemetry::StatusText _status_text{Telemetry::StatusText::StatusType::INFO, ""};

    mutable std::mutex _attitude_quaternion_mutex{};
    Telemetry::Quaternion _attitude_quaternion{NAN, NAN, NAN, NAN};

    mutable std::mutex _camera_attitude_euler_angle_mutex{};
    Telemetry::EulerAngle _camera_attitude_euler_angle{NAN, NAN, NAN};

    mutable std::mutex _attitude_angular_velocity_body_mutex{};
    Telemetry::AngularVelocityBody _attitude_angular_velocity_body{NAN, NAN, NAN};

    mutable std::mutex _ground_truth_mutex{};
    Telemetry::GroundTruth _ground_truth{double(NAN), double(NAN), NAN};

    mutable std::mutex _fixedwing_metrics_mutex{};
    Telemetry::FixedwingMetrics _fixedwing_metrics{NAN, NAN, NAN};

    mutable std::mutex _ground_speed_ned_mutex{};
    Telemetry::GroundSpeedNED _ground_speed_ned{NAN, NAN, NAN};

    mutable std::mutex _imu_reading_ned_mutex{};
    Telemetry::IMUReadingNED _imu_reading_ned{
        {NAN, NAN, NAN}, {NAN, NAN, NAN}, {NAN, NAN, NAN}, NAN};

    mutable std::mutex _gps_info_mutex{};
    Telemetry::GPSInfo _gps_info{0, 0};

    mutable std::mutex _battery_mutex{};
    Telemetry::Battery _battery{NAN, NAN};

    mutable std::mutex _health_mutex{};
    Telemetry::Health _health{false, false, false, false, false, false, false};

    mutable std::mutex _landed_state_mutex{};
    Telemetry::LandedState _landed_state{Telemetry::LandedState::UNKNOWN};

    mutable std::mutex _rc_status_mutex{};
    Telemetry::RCStatus _rc_status{false, false, 0.0f};

    mutable std::mutex _unix_epoch_time_mutex{};
    uint64_t _unix_epoch_time_us{};

    mutable std::mutex _actuator_control_target_mutex{};
    Telemetry::ActuatorControlTarget _actuator_control_target{0, {0.0f}};

    mutable std::mutex _actuator_output_status_mutex{};
    Telemetry::ActuatorOutputStatus _actuator_output_status{0, {0.0f}};

    mutable std::mutex _odometry_mutex{};
    Telemetry::Odometry _odometry{};

    std::atomic<bool> _hitl_enabled{false};

    Telemetry::position_velocity_ned_callback_t _position_velocity_ned_subscription{nullptr};
    Telemetry::position_callback_t _position_subscription{nullptr};
    Telemetry::position_callback_t _home_position_subscription{nullptr};
    Telemetry::in_air_callback_t _in_air_subscription{nullptr};
    Telemetry::status_text_callback_t _status_text_subscription{nullptr};
    Telemetry::armed_callback_t _armed_subscription{nullptr};
    Telemetry::attitude_quaternion_callback_t _attitude_quaternion_subscription{nullptr};
    Telemetry::attitude_angular_velocity_body_callback_t
        _attitude_angular_velocity_body_subscription{nullptr};
    Telemetry::ground_truth_callback_t _ground_truth_subscription{nullptr};
    Telemetry::fixedwing_metrics_callback_t _fixedwing_metrics_subscription{nullptr};
    Telemetry::attitude_euler_angle_callback_t _attitude_euler_angle_subscription{nullptr};
    Telemetry::attitude_quaternion_callback_t _camera_attitude_quaternion_subscription{nullptr};
    Telemetry::attitude_euler_angle_callback_t _camera_attitude_euler_angle_subscription{nullptr};
    Telemetry::ground_speed_ned_callback_t _ground_speed_ned_subscription{nullptr};
    Telemetry::imu_reading_ned_callback_t _imu_reading_ned_subscription{nullptr};
    Telemetry::gps_info_callback_t _gps_info_subscription{nullptr};
    Telemetry::battery_callback_t _battery_subscription{nullptr};
    Telemetry::flight_mode_callback_t _flight_mode_subscription{nullptr};
    Telemetry::health_callback_t _health_subscription{nullptr};
    Telemetry::health_all_ok_callback_t _health_all_ok_subscription{nullptr};
    Telemetry::landed_state_callback_t _landed_state_subscription{nullptr};
    Telemetry::rc_status_callback_t _rc_status_subscription{nullptr};
    Telemetry::unix_epoch_time_callback_t _unix_epoch_time_subscription{nullptr};
    Telemetry::actuator_control_target_callback_t _actuator_control_target_subscription{nullptr};
    Telemetry::actuator_output_status_callback_t _actuator_output_status_subscription{nullptr};
    Telemetry::odometry_callback_t _odometry_subscription{nullptr};

    // The ground speed and position are coupled to the same message, therefore, we just use
    // the faster between the two.
    double _ground_speed_ned_rate_hz{0.0};
    double _position_rate_hz{-1.0};

    void* _rc_channels_timeout_cookie{nullptr};
    void* _gps_raw_timeout_cookie{nullptr};
    void* _unix_epoch_timeout_cookie{nullptr};
};
} // namespace mavsdk
