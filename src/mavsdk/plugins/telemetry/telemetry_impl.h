#pragma once

#include <atomic>
#include <mutex>
#include <optional>

#include "plugins/telemetry/telemetry.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "callback_list.h"

namespace mavsdk {

class System;

class TelemetryImpl : public PluginImplBase {
public:
    explicit TelemetryImpl(System& system);
    explicit TelemetryImpl(std::shared_ptr<System> system);
    ~TelemetryImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Telemetry::Result set_rate_position_velocity_ned(double rate_hz);
    Telemetry::Result set_rate_position(double rate_hz);
    Telemetry::Result set_rate_home(double rate_hz);
    Telemetry::Result set_rate_in_air(double rate_hz);
    Telemetry::Result set_rate_landed_state(double rate_hz);
    Telemetry::Result set_rate_vtol_state(double rate_hz);
    Telemetry::Result set_rate_attitude_quaternion(double rate_hz);
    Telemetry::Result set_rate_attitude_euler(double rate_hz);
    Telemetry::Result set_rate_velocity_ned(double rate_hz);
    Telemetry::Result set_rate_imu(double rate_hz);
    Telemetry::Result set_rate_scaled_imu(double rate_hz);
    Telemetry::Result set_rate_raw_imu(double rate_hz);
    Telemetry::Result set_rate_fixedwing_metrics(double rate_hz);
    Telemetry::Result set_rate_ground_truth(double rate_hz);
    Telemetry::Result set_rate_gps_info(double rate_hz);
    Telemetry::Result set_rate_battery(double rate_hz);
    Telemetry::Result set_rate_rc_status(double rate_hz);
    Telemetry::Result set_rate_actuator_control_target(double rate_hz);
    Telemetry::Result set_rate_actuator_output_status(double rate_hz);
    Telemetry::Result set_rate_odometry(double rate_hz);
    Telemetry::Result set_rate_distance_sensor(double rate_hz);
    Telemetry::Result set_rate_scaled_pressure(double rate_hz);
    Telemetry::Result set_rate_unix_epoch_time(double rate_hz);
    Telemetry::Result set_rate_altitude(double rate_hz);

    void set_rate_position_velocity_ned_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_position_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_home_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_in_air_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_landed_state_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_vtol_state_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_attitude_quaternion_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_attitude_euler_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_velocity_ned_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_imu_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_scaled_imu_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_raw_imu_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_fixedwing_metrics_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_ground_truth_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_gps_info_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_battery_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_rc_status_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_actuator_control_target_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_actuator_output_status_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_odometry_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_distance_sensor_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_scaled_pressure_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_unix_epoch_time_async(double rate_hz, Telemetry::ResultCallback callback);
    void set_rate_altitude_async(double rate_hz, Telemetry::ResultCallback callback);

    void get_gps_global_origin_async(const Telemetry::GetGpsGlobalOriginCallback callback);
    std::pair<Telemetry::Result, Telemetry::GpsGlobalOrigin> get_gps_global_origin();

    Telemetry::PositionVelocityNed position_velocity_ned() const;
    Telemetry::Position position() const;
    Telemetry::Position home() const;
    bool in_air() const;
    bool armed() const;
    Telemetry::VtolState vtol_state() const;
    Telemetry::LandedState landed_state() const;
    Telemetry::StatusText status_text() const;
    Telemetry::EulerAngle attitude_euler() const;
    Telemetry::Quaternion attitude_quaternion() const;
    Telemetry::AngularVelocityBody attitude_angular_velocity_body() const;
    Telemetry::GroundTruth ground_truth() const;
    Telemetry::FixedwingMetrics fixedwing_metrics() const;
    Telemetry::VelocityNed velocity_ned() const;
    Telemetry::Imu imu() const;
    Telemetry::Imu scaled_imu() const;
    Telemetry::Imu raw_imu() const;
    Telemetry::GpsInfo gps_info() const;
    Telemetry::RawGps raw_gps() const;
    Telemetry::Battery battery() const;
    Telemetry::FlightMode flight_mode() const;
    Telemetry::Health health() const;
    bool health_all_ok() const;
    Telemetry::RcStatus rc_status() const;
    Telemetry::ActuatorControlTarget actuator_control_target() const;
    Telemetry::ActuatorOutputStatus actuator_output_status() const;
    Telemetry::Odometry odometry() const;
    Telemetry::DistanceSensor distance_sensor() const;
    Telemetry::ScaledPressure scaled_pressure() const;
    uint64_t unix_epoch_time() const;
    Telemetry::Heading heading() const;
    Telemetry::Altitude altitude() const;

    Telemetry::PositionVelocityNedHandle
    subscribe_position_velocity_ned(const Telemetry::PositionVelocityNedCallback& callback);
    void unsubscribe_position_velocity_ned(Telemetry::PositionVelocityNedHandle handle);
    Telemetry::PositionHandle subscribe_position(const Telemetry::PositionCallback& callback);
    void unsubscribe_position(Telemetry::PositionHandle handle);
    Telemetry::HomeHandle subscribe_home(const Telemetry::PositionCallback& callback);
    void unsubscribe_home(Telemetry::HomeHandle handle);
    Telemetry::InAirHandle subscribe_in_air(const Telemetry::InAirCallback& callback);
    void unsubscribe_in_air(Telemetry::InAirHandle handle);
    Telemetry::StatusTextHandle
    subscribe_status_text(const Telemetry::StatusTextCallback& callback);
    void unsubscribe_status_text(Telemetry::StatusTextHandle handle);
    Telemetry::ArmedHandle subscribe_armed(const Telemetry::ArmedCallback& callback);
    void unsubscribe_armed(Telemetry::ArmedHandle handle);
    Telemetry::AttitudeQuaternionHandle
    subscribe_attitude_quaternion(const Telemetry::AttitudeQuaternionCallback& callback);
    void unsubscribe_attitude_quaternion(Telemetry::AttitudeQuaternionHandle handle);
    Telemetry::AttitudeEulerHandle
    subscribe_attitude_euler(const Telemetry::AttitudeEulerCallback& callback);
    void unsubscribe_attitude_euler(Telemetry::AttitudeEulerHandle handle);
    Telemetry::AttitudeAngularVelocityBodyHandle subscribe_attitude_angular_velocity_body(
        const Telemetry::AttitudeAngularVelocityBodyCallback& callback);
    void
    unsubscribe_attitude_angular_velocity_body(Telemetry::AttitudeAngularVelocityBodyHandle handle);
    Telemetry::FixedwingMetricsHandle
    subscribe_fixedwing_metrics(const Telemetry::FixedwingMetricsCallback& callback);
    void unsubscribe_fixedwing_metrics(Telemetry::FixedwingMetricsHandle handle);
    Telemetry::GroundTruthHandle
    subscribe_ground_truth(const Telemetry::GroundTruthCallback& callback);
    void unsubscribe_ground_truth(Telemetry::GroundTruthHandle handle);
    Telemetry::VelocityNedHandle
    subscribe_velocity_ned(const Telemetry::VelocityNedCallback& callback);
    void unsubscribe_velocity_ned(Telemetry::VelocityNedHandle handle);
    Telemetry::ImuHandle subscribe_imu(const Telemetry::ImuCallback& callback);
    void unsubscribe_imu(Telemetry::ImuHandle handle);
    Telemetry::ScaledImuHandle subscribe_scaled_imu(const Telemetry::ScaledImuCallback& callback);
    void unsubscribe_scaled_imu(Telemetry::ScaledImuHandle handle);
    Telemetry::RawImuHandle subscribe_raw_imu(const Telemetry::RawImuCallback& callback);
    void unsubscribe_raw_imu(Telemetry::RawImuHandle handle);
    Telemetry::GpsInfoHandle subscribe_gps_info(const Telemetry::GpsInfoCallback& callback);
    void unsubscribe_gps_info(Telemetry::GpsInfoHandle handle);
    Telemetry::RawGpsHandle subscribe_raw_gps(const Telemetry::RawGpsCallback& callback);
    void unsubscribe_raw_gps(Telemetry::RawGpsHandle handle);
    Telemetry::BatteryHandle subscribe_battery(const Telemetry::BatteryCallback& callback);
    void unsubscribe_battery(Telemetry::BatteryHandle handle);
    Telemetry::FlightModeHandle
    subscribe_flight_mode(const Telemetry::FlightModeCallback& callback);
    void unsubscribe_flight_mode(Telemetry::FlightModeHandle handle);
    Telemetry::HealthHandle subscribe_health(const Telemetry::HealthCallback& callback);
    void unsubscribe_health(Telemetry::HealthHandle handle);
    Telemetry::HealthAllOkHandle
    subscribe_health_all_ok(const Telemetry::HealthAllOkCallback& callback);
    void unsubscribe_health_all_ok(Telemetry::HealthAllOkHandle handle);
    Telemetry::VtolStateHandle subscribe_vtol_state(const Telemetry::VtolStateCallback& callback);
    void unsubscribe_vtol_state(Telemetry::VtolStateHandle handle);
    Telemetry::LandedStateHandle
    subscribe_landed_state(const Telemetry::LandedStateCallback& callback);
    void unsubscribe_landed_state(Telemetry::LandedStateHandle handle);
    Telemetry::RcStatusHandle subscribe_rc_status(const Telemetry::RcStatusCallback& callback);
    void unsubscribe_rc_status(Telemetry::RcStatusHandle handle);
    Telemetry::UnixEpochTimeHandle
    subscribe_unix_epoch_time(const Telemetry::UnixEpochTimeCallback& callback);
    void unsubscribe_unix_epoch_time(Telemetry::UnixEpochTimeHandle handle);
    Telemetry::ActuatorControlTargetHandle
    subscribe_actuator_control_target(const Telemetry::ActuatorControlTargetCallback& callback);
    void unsubscribe_actuator_control_target(Telemetry::ActuatorControlTargetHandle handle);
    Telemetry::ActuatorOutputStatusHandle
    subscribe_actuator_output_status(const Telemetry::ActuatorOutputStatusCallback& callback);
    void unsubscribe_actuator_output_status(Telemetry::ActuatorOutputStatusHandle handle);
    Telemetry::OdometryHandle subscribe_odometry(const Telemetry::OdometryCallback& callback);
    void unsubscribe_odometry(Telemetry::OdometryHandle handle);
    Telemetry::DistanceSensorHandle
    subscribe_distance_sensor(const Telemetry::DistanceSensorCallback& callback);
    void unsubscribe_distance_sensor(Telemetry::DistanceSensorHandle handle);
    Telemetry::ScaledPressureHandle
    subscribe_scaled_pressure(const Telemetry::ScaledPressureCallback& callback);
    void unsubscribe_scaled_pressure(Telemetry::ScaledPressureHandle handle);
    Telemetry::HeadingHandle subscribe_heading(const Telemetry::HeadingCallback& callback);
    void unsubscribe_heading(Telemetry::HeadingHandle handle);
    Telemetry::AltitudeHandle subscribe_altitude(const Telemetry::AltitudeCallback& callback);
    void unsubscribe_altitude(Telemetry::AltitudeHandle handle);

    TelemetryImpl(const TelemetryImpl&) = delete;
    TelemetryImpl& operator=(const TelemetryImpl&) = delete;

private:
    void set_position_velocity_ned(Telemetry::PositionVelocityNed position_velocity_ned);
    void set_position(Telemetry::Position position);
    void set_home_position(Telemetry::Position home_position);
    void set_in_air(bool in_air);
    void set_vtol_state(Telemetry::VtolState vtol_state);
    void set_landed_state(Telemetry::LandedState landed_state);
    void set_status_text(Telemetry::StatusText status_text);
    void set_armed(bool armed);
    void set_attitude_quaternion(Telemetry::Quaternion quaternion);
    void set_attitude_euler(Telemetry::EulerAngle euler);
    void set_attitude_angular_velocity_body(Telemetry::AngularVelocityBody angular_velocity_body);
    void set_fixedwing_metrics(Telemetry::FixedwingMetrics fixedwing_metrics);
    void set_ground_truth(Telemetry::GroundTruth ground_truth);
    void set_velocity_ned(Telemetry::VelocityNed velocity_ned);
    void set_imu_reading_ned(Telemetry::Imu imu);
    void set_scaled_imu(Telemetry::Imu imu);
    void set_raw_imu(Telemetry::Imu imu);
    void set_gps_info(Telemetry::GpsInfo gps_info);
    void set_raw_gps(Telemetry::RawGps raw_gps);
    void set_battery(Telemetry::Battery battery);
    void set_health_local_position(bool ok);
    void set_health_global_position(bool ok);
    void set_health_home_position(bool ok);
    void set_health_gyrometer_calibration(bool ok);
    void set_health_accelerometer_calibration(bool ok);
    void set_health_magnetometer_calibration(bool ok);
    void set_health_armable(bool ok);
    void set_rc_status(std::optional<bool> available, std::optional<float> signal_strength_percent);
    void set_unix_epoch_time_us(uint64_t time_us);
    void set_actuator_control_target(uint8_t group, const std::vector<float>& controls);
    void set_actuator_output_status(uint32_t active, const std::vector<float>& actuators);
    void set_odometry(Telemetry::Odometry& odometry);
    void set_distance_sensor(Telemetry::DistanceSensor& distance_sensor);
    void set_scaled_pressure(Telemetry::ScaledPressure& scaled_pressure);
    void set_heading(Telemetry::Heading heading);
    void set_altitude(Telemetry::Altitude altitude);

    void process_position_velocity_ned(const mavlink_message_t& message);
    void process_global_position_int(const mavlink_message_t& message);
    void process_home_position(const mavlink_message_t& message);
    void process_attitude(const mavlink_message_t& message);
    void process_attitude_quaternion(const mavlink_message_t& message);
    void process_imu_reading_ned(const mavlink_message_t& message);
    void process_scaled_imu(const mavlink_message_t& message);
    void process_raw_imu(const mavlink_message_t& message);
    void process_gps_raw_int(const mavlink_message_t& message);
    void process_ground_truth(const mavlink_message_t& message);
    void process_extended_sys_state(const mavlink_message_t& message);
    void process_fixedwing_metrics(const mavlink_message_t& message);
    void process_sys_status(const mavlink_message_t& message);
    void process_battery_status(const mavlink_message_t& message);
    void process_heartbeat(const mavlink_message_t& message);
    void process_rc_channels(const mavlink_message_t& message);
    void process_unix_epoch_time(const mavlink_message_t& message);
    void process_actuator_control_target(const mavlink_message_t& message);
    void process_actuator_output_status(const mavlink_message_t& message);
    void process_odometry(const mavlink_message_t& message);
    void process_distance_sensor(const mavlink_message_t& message);
    void process_scaled_pressure(const mavlink_message_t& message);
    void process_altitude(const mavlink_message_t& message);
    void receive_param_cal_gyro(MavlinkParameterClient::Result result, int value);
    void receive_param_cal_accel(MavlinkParameterClient::Result result, int value);
    void receive_param_cal_mag(MavlinkParameterClient::Result result, int value);

    // Ardupilot sensor offset callbacks.
    void receive_param_cal_gyro_offset_x(MavlinkParameterClient::Result result, float value);
    void receive_param_cal_gyro_offset_y(MavlinkParameterClient::Result result, float value);
    void receive_param_cal_gyro_offset_z(MavlinkParameterClient::Result result, float value);
    void receive_param_cal_accel_offset_x(MavlinkParameterClient::Result result, float value);
    void receive_param_cal_accel_offset_y(MavlinkParameterClient::Result result, float value);
    void receive_param_cal_accel_offset_z(MavlinkParameterClient::Result result, float value);
    void receive_param_cal_mag_offset_x(MavlinkParameterClient::Result result, float value);
    void receive_param_cal_mag_offset_y(MavlinkParameterClient::Result result, float value);
    void receive_param_cal_mag_offset_z(MavlinkParameterClient::Result result, float value);

    void process_parameter_update(const std::string& name);
    void receive_param_hitl(MavlinkParameterClient::Result result, int value);

    void receive_rc_channels_timeout();
    void receive_gps_raw_timeout();
    void receive_unix_epoch_timeout();

    void receive_statustext(const MavlinkStatustextHandler::Statustext&);

    void request_home_position_async();
    void request_home_position_again();
    void check_calibration();

    static bool sys_status_present_enabled_health(
        const mavlink_sys_status_t& sys_status, MAV_SYS_STATUS_SENSOR flag);

    static Telemetry::Result
    telemetry_result_from_command_result(MavlinkCommandSender::Result command_result);

    static void command_result_callback(
        MavlinkCommandSender::Result command_result, const Telemetry::ResultCallback& callback);

    static Telemetry::LandedState to_landed_state(mavlink_extended_sys_state_t extended_sys_state);
    static Telemetry::VtolState to_vtol_state(mavlink_extended_sys_state_t extended_sys_state);

    static Telemetry::FlightMode telemetry_flight_mode_from_flight_mode(FlightMode flight_mode);

    // Make all fields thread-safe using mutexs
    // The mutexs are mutable so that the lock can get aqcuired in
    // methods marked const.
    mutable std::mutex _position_mutex{};
    Telemetry::Position _position{};

    mutable std::mutex _heading_mutex{};
    Telemetry::Heading _heading{};

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

    mutable std::mutex _attitude_euler_mutex{};
    Telemetry::EulerAngle _attitude_euler{};

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

    mutable std::mutex _scaled_imu_mutex{};
    Telemetry::Imu _scaled_imu{};

    mutable std::mutex _raw_imu_mutex{};
    Telemetry::Imu _raw_imu{};

    mutable std::mutex _gps_info_mutex{};
    Telemetry::GpsInfo _gps_info{};

    mutable std::mutex _raw_gps_mutex{};
    Telemetry::RawGps _raw_gps{};

    mutable std::mutex _battery_mutex{};
    Telemetry::Battery _battery{};

    mutable std::mutex _health_mutex{};
    Telemetry::Health _health{};

    mutable std::mutex _vtol_state_mutex{};
    Telemetry::VtolState _vtol_state{Telemetry::VtolState::Undefined};

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

    mutable std::mutex _scaled_pressure_mutex{};
    Telemetry::ScaledPressure _scaled_pressure{};

    mutable std::mutex _altitude_mutex{};
    Telemetry::Altitude _altitude{};

    mutable std::mutex _request_home_position_mutex{};

    std::atomic<bool> _hitl_enabled{false};

    std::mutex _subscription_mutex{};
    CallbackList<Telemetry::PositionVelocityNed> _position_velocity_ned_subscriptions{};
    CallbackList<Telemetry::Position> _position_subscriptions{};
    CallbackList<Telemetry::Position> _home_position_subscriptions{};
    CallbackList<bool> _in_air_subscriptions{};
    CallbackList<Telemetry::StatusText> _status_text_subscriptions{};
    CallbackList<bool> _armed_subscriptions{};
    CallbackList<Telemetry::Quaternion> _attitude_quaternion_angle_subscriptions{};
    CallbackList<Telemetry::AngularVelocityBody> _attitude_angular_velocity_body_subscriptions{};
    CallbackList<Telemetry::GroundTruth> _ground_truth_subscriptions{};
    CallbackList<Telemetry::FixedwingMetrics> _fixedwing_metrics_subscriptions{};
    CallbackList<Telemetry::EulerAngle> _attitude_euler_angle_subscriptions{};
    CallbackList<Telemetry::VelocityNed> _velocity_ned_subscriptions{};
    CallbackList<Telemetry::Imu> _imu_reading_ned_subscriptions{};
    CallbackList<Telemetry::Imu> _scaled_imu_subscriptions{};
    CallbackList<Telemetry::Imu> _raw_imu_subscriptions{};
    CallbackList<Telemetry::GpsInfo> _gps_info_subscriptions{};
    CallbackList<Telemetry::RawGps> _raw_gps_subscriptions{};
    CallbackList<Telemetry::Battery> _battery_subscriptions{};
    CallbackList<Telemetry::FlightMode> _flight_mode_subscriptions{};
    CallbackList<Telemetry::Health> _health_subscriptions{};
    CallbackList<bool> _health_all_ok_subscriptions{};
    CallbackList<Telemetry::VtolState> _vtol_state_subscriptions{};
    CallbackList<Telemetry::LandedState> _landed_state_subscriptions{};
    CallbackList<Telemetry::RcStatus> _rc_status_subscriptions{};
    CallbackList<uint64_t> _unix_epoch_time_subscriptions{};
    CallbackList<Telemetry::ActuatorControlTarget> _actuator_control_target_subscriptions{};
    CallbackList<Telemetry::ActuatorOutputStatus> _actuator_output_status_subscriptions{};
    CallbackList<Telemetry::Odometry> _odometry_subscriptions{};
    CallbackList<Telemetry::DistanceSensor> _distance_sensor_subscriptions{};
    CallbackList<Telemetry::ScaledPressure> _scaled_pressure_subscriptions{};
    CallbackList<Telemetry::Heading> _heading_subscriptions{};
    CallbackList<Telemetry::Altitude> _altitude_subscriptions{};

    // The velocity (former ground speed) and position are coupled to the same message, therefore,
    // we just use the faster between the two.
    double _velocity_ned_rate_hz{0.0};
    double _position_rate_hz{-1.0};

    TimeoutHandler::Cookie _rc_channels_timeout_cookie{};
    TimeoutHandler::Cookie _gps_raw_timeout_cookie{};
    TimeoutHandler::Cookie _unix_epoch_timeout_cookie{};

    // Battery info can be extracted from SYS_STATUS or from BATTERY_STATUS.
    // If no BATTERY_STATUS messages are received, use info from SYS_STATUS.
    bool _has_bat_status{false};

    CallEveryHandler::Cookie _calibration_cookie{};
    CallEveryHandler::Cookie _homepos_cookie{};

    std::atomic<bool> _has_received_hitl_param{false};

    std::atomic<bool> _has_received_gyro_calibration{false};
    std::atomic<bool> _has_received_accel_calibration{false};
    std::atomic<bool> _has_received_mag_calibration{false};

    std::mutex _ap_calibration_mutex{};
    struct ArdupilotCalibration {
        struct OffsetStatus {
            std::optional<float> x{};
            std::optional<float> y{};
            std::optional<float> z{};

            [[nodiscard]] bool received_all() const
            {
                return x.has_value() && y.has_value() && z.has_value();
            }
            [[nodiscard]] bool calibrated() const
            {
                return received_all() && ((x.value() != 0) && (y.value() != 0) && (z.value() != 0));
            }
        };

        OffsetStatus mag_offset;
        OffsetStatus accel_offset;
        OffsetStatus gyro_offset;

    } _ap_calibration{};

    enum class SysStatusUsed {
        Unknown,
        Yes,
        No,
    };
    std::atomic<SysStatusUsed> _sys_status_used_for_position{SysStatusUsed::Unknown};

    Telemetry::EulerAngle extractOrientation(mavlink_distance_sensor_t distance_sensor_msg);

    enum class MavSensorOrientation {
        MAV_SENSOR_ROTATION_NONE = 0, // Roll: 0, Pitch: 0, Yaw: 0
        MAV_SENSOR_ROTATION_YAW_45, // Roll: 0, Pitch: 0, Yaw: 45
        MAV_SENSOR_ROTATION_YAW_90, // Roll: 0, Pitch: 0, Yaw: 90
        MAV_SENSOR_ROTATION_YAW_135, // Roll: 0, Pitch: 0, Yaw: 135
        MAV_SENSOR_ROTATION_YAW_180, // Roll: 0, Pitch: 0, Yaw: 180
        MAV_SENSOR_ROTATION_YAW_225, // Roll: 0, Pitch: 0, Yaw: 225
        MAV_SENSOR_ROTATION_YAW_270, // Roll: 0, Pitch: 0, Yaw: 270
        MAV_SENSOR_ROTATION_YAW_315, // Roll: 0, Pitch: 0, Yaw: 315
        MAV_SENSOR_ROTATION_ROLL_180, // Roll: 180, Pitch: 0, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_180_YAW_45, // Roll: 180, Pitch: 0, Yaw: 45
        MAV_SENSOR_ROTATION_ROLL_180_YAW_90, // Roll: 180, Pitch: 0, Yaw: 90
        MAV_SENSOR_ROTATION_ROLL_180_YAW_135, // Roll: 180, Pitch: 0, Yaw: 135
        MAV_SENSOR_ROTATION_PITCH_180, // Roll: 0, Pitch: 180, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_180_YAW_225, // Roll: 180, Pitch: 0, Yaw: 225
        MAV_SENSOR_ROTATION_ROLL_180_YAW_270, // Roll: 180, Pitch: 0, Yaw: 270
        MAV_SENSOR_ROTATION_ROLL_180_YAW_315, // Roll: 180, Pitch: 0, Yaw: 315
        MAV_SENSOR_ROTATION_ROLL_90, // Roll: 90, Pitch: 0, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_90_YAW_45, // Roll: 90, Pitch: 0, Yaw: 45
        MAV_SENSOR_ROTATION_ROLL_90_YAW_90, // Roll: 90, Pitch: 0, Yaw: 90
        MAV_SENSOR_ROTATION_ROLL_90_YAW_135, // Roll: 90, Pitch: 0, Yaw: 135
        MAV_SENSOR_ROTATION_ROLL_270, // Roll: 270, Pitch: 0, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_270_YAW_45, // Roll: 270, Pitch: 0, Yaw: 45
        MAV_SENSOR_ROTATION_ROLL_270_YAW_90, // Roll: 270, Pitch: 0, Yaw: 90
        MAV_SENSOR_ROTATION_ROLL_270_YAW_135, // Roll: 270, Pitch: 0, Yaw: 135
        MAV_SENSOR_ROTATION_PITCH_90, // Roll: 0, Pitch: 90, Yaw: 0
        MAV_SENSOR_ROTATION_PITCH_270, // Roll: 0, Pitch: 270, Yaw: 0
        MAV_SENSOR_ROTATION_PITCH_180_YAW_90, // Roll: 0, Pitch: 180, Yaw: 90
        MAV_SENSOR_ROTATION_PITCH_180_YAW_270, // Roll: 0, Pitch: 180, Yaw: 270
        MAV_SENSOR_ROTATION_ROLL_90_PITCH_90, // Roll: 90, Pitch: 90, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_180_PITCH_90, // Roll: 180, Pitch: 90, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_270_PITCH_90, // Roll: 270, Pitch: 90, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_90_PITCH_180, // Roll: 90, Pitch: 180, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_270_PITCH_180, // Roll: 270, Pitch: 180, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_90_PITCH_270, // Roll: 90, Pitch: 270, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_180_PITCH_270, // Roll: 180, Pitch: 270, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_270_PITCH_270, // Roll: 270, Pitch: 270, Yaw: 0
        MAV_SENSOR_ROTATION_ROLL_90_PITCH_180_YAW_90, // Roll: 90, Pitch: 180, Yaw: 90
        MAV_SENSOR_ROTATION_ROLL_90_YAW_270, // Roll: 90, Pitch: 0, Yaw: 270
        MAV_SENSOR_ROTATION_ROLL_90_PITCH_68_YAW_293, // Roll: 90, Pitch: 68, Yaw: 293
        MAV_SENSOR_ROTATION_PITCH_315, // Pitch: 315
        MAV_SENSOR_ROTATION_ROLL_90_PITCH_315, // Roll: 90, Pitch: 315
        MAV_SENSOR_ROTATION_CUSTOM =
            100, // Custom orientation will be set as Roll: -1, Pitch: -1, Yaw: -1
    };
};
} // namespace mavsdk
