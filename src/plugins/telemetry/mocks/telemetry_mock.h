#include <gmock/gmock.h>

#include "plugins/telemetry/telemetry.h"

namespace mavsdk {
namespace testing {

class MockTelemetry {
public:
    MOCK_CONST_METHOD1(subscribe_position, void(Telemetry::PositionCallback)){};
    MOCK_CONST_METHOD1(subscribe_health, void(Telemetry::HealthCallback)){};
    MOCK_CONST_METHOD1(subscribe_home, void(Telemetry::PositionCallback)){};
    MOCK_CONST_METHOD1(subscribe_in_air, void(Telemetry::InAirCallback)){};
    MOCK_CONST_METHOD1(subscribe_status_text, void(Telemetry::StatusTextCallback)){};
    MOCK_CONST_METHOD1(subscribe_armed, void(Telemetry::ArmedCallback)){};
    MOCK_CONST_METHOD1(subscribe_gps_info, void(Telemetry::GpsInfoCallback)){};
    MOCK_CONST_METHOD1(subscribe_battery, void(Telemetry::BatteryCallback)){};
    MOCK_CONST_METHOD1(subscribe_flight_mode, void(Telemetry::FlightModeCallback)){};
    MOCK_CONST_METHOD1(subscribe_landed_state, void(Telemetry::LandedStateCallback)){};
    MOCK_CONST_METHOD1(
        subscribe_attitude_quaternion, void(Telemetry::AttitudeQuaternionCallback)){};
    MOCK_CONST_METHOD1(
        subscribe_attitude_angular_velocity_body,
        void(Telemetry::AttitudeAngularVelocityBodyCallback)){};
    MOCK_CONST_METHOD1(subscribe_attitude_euler, void(Telemetry::AttitudeEulerCallback)){};
    MOCK_CONST_METHOD1(
        subscribe_camera_attitude_quaternion, void(Telemetry::AttitudeQuaternionCallback)){};
    MOCK_CONST_METHOD1(subscribe_camera_attitude_euler, void(Telemetry::AttitudeEulerCallback)){};
    MOCK_CONST_METHOD1(subscribe_velocity_ned, void(Telemetry::VelocityNedCallback)){};
    MOCK_CONST_METHOD1(subscribe_rc_status, void(Telemetry::RcStatusCallback)){};
    MOCK_CONST_METHOD1(
        subscribe_actuator_control_target, void(Telemetry::ActuatorControlTargetCallback)){};
    MOCK_CONST_METHOD1(
        subscribe_actuator_output_status, void(Telemetry::ActuatorOutputStatusCallback)){};
    MOCK_CONST_METHOD1(subscribe_odometry, void(Telemetry::OdometryCallback)){};
    MOCK_CONST_METHOD1(subscribe_distance_sensor, void(Telemetry::DistanceSensorCallback)){};
    MOCK_CONST_METHOD1(
        subscribe_position_velocity_ned, void(Telemetry::PositionVelocityNedCallback)){};
    MOCK_CONST_METHOD1(subscribe_ground_truth, void(Telemetry::GroundTruthCallback)){};
    MOCK_CONST_METHOD1(subscribe_fixedwing_metrics, void(Telemetry::FixedwingMetricsCallback)){};
    MOCK_CONST_METHOD1(subscribe_imu, void(Telemetry::ImuCallback)){};
    MOCK_CONST_METHOD1(subscribe_health_all_ok, void(Telemetry::HealthAllOkCallback)){};
    MOCK_CONST_METHOD1(subscribe_unix_epoch_time, void(Telemetry::UnixEpochTimeCallback)){};

    MOCK_METHOD1(set_rate_position, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_home, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_in_air, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_landed_state, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_attitude, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_camera_attitude, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_velocity_ned, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_gps_info, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_battery, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_rc_status, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_actuator_control_target, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_actuator_output_status, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_odometry, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_distance_sensor, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_position_velocity_ned, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_ground_truth, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_fixedwing_metrics, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_imu, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_unix_epoch_time, Telemetry::Result(double)){};

    MOCK_CONST_METHOD2(set_rate_position_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_home_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_in_air_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_landed_state_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_attitude_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_camera_attitude_async, void(double, Telemetry::ResultCallback)){};
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
        set_rate_position_velocity_ned_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_ground_truth_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_fixedwing_metrics_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_imu_async, void(double, Telemetry::ResultCallback)){};
    MOCK_CONST_METHOD2(set_rate_unix_epoch_time_async, void(double, Telemetry::ResultCallback)){};

    MOCK_METHOD0(
        get_gps_global_origin, std::pair<Telemetry::Result, Telemetry::GpsGlobalOrigin>()){};
    MOCK_METHOD1(get_gps_global_origin_async, void(Telemetry::GetGpsGlobalOriginCallback)){};
};

} // namespace testing
} // namespace mavsdk
