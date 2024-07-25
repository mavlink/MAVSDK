#include <gmock/gmock.h>

#include "plugins/telemetry/telemetry.h"

namespace mavsdk {
namespace testing {

class MockTelemetry {
public:
    MOCK_CONST_METHOD1(
        subscribe_position, Telemetry::PositionHandle(Telemetry::PositionCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_position, void(Telemetry::PositionHandle)){};
    MOCK_CONST_METHOD1(subscribe_health, Telemetry::HealthHandle(Telemetry::HealthCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_health, void(Telemetry::HealthHandle)){};
    MOCK_CONST_METHOD1(subscribe_heading, Telemetry::HeadingHandle(Telemetry::HeadingCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_heading, void(Telemetry::HeadingHandle)){};
    MOCK_CONST_METHOD1(subscribe_home, Telemetry::PositionHandle(Telemetry::PositionCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_home, void(Telemetry::PositionHandle)){};
    MOCK_CONST_METHOD1(subscribe_in_air, Telemetry::InAirHandle(Telemetry::InAirCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_in_air, void(Telemetry::InAirHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_status_text, Telemetry::StatusTextHandle(Telemetry::StatusTextCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_status_text, void(Telemetry::StatusTextHandle)){};
    MOCK_CONST_METHOD1(subscribe_armed, Telemetry::ArmedHandle(Telemetry::ArmedCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_armed, void(Telemetry::ArmedHandle)){};
    MOCK_CONST_METHOD1(subscribe_gps_info, Telemetry::GpsInfoHandle(Telemetry::GpsInfoCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_gps_info, void(Telemetry::GpsInfoHandle)){};
    MOCK_CONST_METHOD1(subscribe_raw_gps, Telemetry::RawGpsHandle(Telemetry::RawGpsCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_raw_gps, void(Telemetry::RawGpsHandle)){};
    MOCK_CONST_METHOD1(subscribe_battery, Telemetry::BatteryHandle(Telemetry::BatteryCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_battery, void(Telemetry::BatteryHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_flight_mode, Telemetry::FlightModeHandle(Telemetry::FlightModeCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_flight_mode, void(Telemetry::FlightModeHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_landed_state, Telemetry::LandedStateHandle(Telemetry::LandedStateCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_landed_state, void(Telemetry::LandedStateHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_attitude_quaternion,
        Telemetry::AttitudeQuaternionHandle(Telemetry::AttitudeQuaternionCallback)){};
    MOCK_CONST_METHOD1(
        unsubscribe_attitude_quaternion, void(Telemetry::AttitudeQuaternionHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_attitude_angular_velocity_body,
        Telemetry::AttitudeAngularVelocityBodyHandle(
            Telemetry::AttitudeAngularVelocityBodyCallback)){};
    MOCK_CONST_METHOD1(
        unsubscribe_attitude_angular_velocity_body,
        void(Telemetry::AttitudeAngularVelocityBodyHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_attitude_euler,
        Telemetry::AttitudeEulerHandle(Telemetry::AttitudeEulerCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_attitude_euler, void(Telemetry::AttitudeEulerHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_velocity_ned, Telemetry::VelocityNedHandle(Telemetry::VelocityNedCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_velocity_ned, void(Telemetry::VelocityNedHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_rc_status, Telemetry::RcStatusHandle(Telemetry::RcStatusCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_rc_status, void(Telemetry::RcStatusHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_actuator_control_target,
        Telemetry::ActuatorControlTargetHandle(Telemetry::ActuatorControlTargetCallback)){};
    MOCK_CONST_METHOD1(
        unsubscribe_actuator_control_target, void(Telemetry::ActuatorControlTargetHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_actuator_output_status,
        Telemetry::ActuatorOutputStatusHandle(Telemetry::ActuatorOutputStatusCallback)){};
    MOCK_CONST_METHOD1(
        unsubscribe_actuator_output_status, void(Telemetry::ActuatorOutputStatusHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_odometry, Telemetry::OdometryHandle(Telemetry::OdometryCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_odometry, void(Telemetry::OdometryHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_distance_sensor,
        Telemetry::DistanceSensorHandle(Telemetry::DistanceSensorCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_distance_sensor, void(Telemetry::DistanceSensorHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_scaled_pressure,
        Telemetry::ScaledPressureHandle(Telemetry::ScaledPressureCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_scaled_pressure, void(Telemetry::ScaledPressureHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_position_velocity_ned,
        Telemetry::PositionVelocityNedHandle(Telemetry::PositionVelocityNedCallback)){};
    MOCK_CONST_METHOD1(
        unsubscribe_position_velocity_ned, void(Telemetry::PositionVelocityNedHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_ground_truth, Telemetry::GroundTruthHandle(Telemetry::GroundTruthCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_ground_truth, void(Telemetry::GroundTruthHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_fixedwing_metrics,
        Telemetry::FixedwingMetricsHandle(Telemetry::FixedwingMetricsCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_fixedwing_metrics, void(Telemetry::FixedwingMetricsHandle)){};
    MOCK_CONST_METHOD1(subscribe_imu, Telemetry::ImuHandle(Telemetry::ImuCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_imu, void(Telemetry::ImuHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_scaled_imu, Telemetry::ScaledImuHandle(Telemetry::ScaledImuCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_scaled_imu, void(Telemetry::ScaledImuHandle)){};
    MOCK_CONST_METHOD1(subscribe_raw_imu, Telemetry::RawImuHandle(Telemetry::RawImuCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_raw_imu, void(Telemetry::RawImuHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_health_all_ok, Telemetry::HealthAllOkHandle(Telemetry::HealthAllOkCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_health_all_ok, void(Telemetry::HealthAllOkHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_unix_epoch_time,
        Telemetry::UnixEpochTimeHandle(Telemetry::UnixEpochTimeCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_unix_epoch_time, void(Telemetry::UnixEpochTimeHandle)){};
    MOCK_CONST_METHOD1(
        subscribe_vtol_state, Telemetry::VtolStateHandle(Telemetry::VtolStateCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_vtol_state, void(Telemetry::VtolStateHandle)){};
    MOCK_CONST_METHOD1(subscribe_altitude, Telemetry::AltitudeHandle(Telemetry::AltitudeCallback));
    MOCK_CONST_METHOD1(unsubscribe_altitude, void(Telemetry::AltitudeHandle));

    MOCK_METHOD1(set_rate_position, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_home, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_in_air, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_landed_state, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_attitude_quaternion, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_attitude_euler, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_velocity_ned, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_gps_info, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_raw_gps, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_battery, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_rc_status, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_actuator_control_target, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_actuator_output_status, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_odometry, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_distance_sensor, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_scaled_pressure, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_position_velocity_ned, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_ground_truth, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_fixedwing_metrics, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_imu, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_scaled_imu, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_raw_imu, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_unix_epoch_time, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_vtol_state, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_altitude, Telemetry::Result(double)){};

    MOCK_CONST_METHOD2(set_rate_position_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_home_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_in_air_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_landed_state_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(
        set_rate_attitude_quaternion_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_attitude_euler_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_velocity_ned_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_gps_info_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_battery_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_rc_status_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(
        set_rate_actuator_control_target_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(
        set_rate_actuator_output_status_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_odometry_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_distance_sensor_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(
        set_rate_scaled_pressure_async, void(double, Telemetry::ScaledPressureCallback)){};
    MOCK_CONST_METHOD2(
        set_rate_position_velocity_ned_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_ground_truth_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_fixedwing_metrics_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_imu_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_scaled_imu_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_raw_imu_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_unix_epoch_time_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_altitude_async, void(double, Telemetry::ResultCallback)){};
    MOCK_METHOD0(
        get_gps_global_origin, std::pair<Telemetry::Result, Telemetry::GpsGlobalOrigin>()){};
    MOCK_METHOD1(get_gps_global_origin_async, void(Telemetry::GetGpsGlobalOriginCallback)){};
};

} // namespace testing
} // namespace mavsdk
