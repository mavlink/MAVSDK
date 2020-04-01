#include <gmock/gmock.h>

#include "plugins/telemetry/telemetry.h"

namespace mavsdk {
namespace testing {

class MockTelemetry {
public:
    MOCK_CONST_METHOD1(position_async, void(Telemetry::position_callback_t)){};
    MOCK_CONST_METHOD1(health_async, void(Telemetry::health_callback_t)){};
    MOCK_CONST_METHOD1(home_async, void(Telemetry::position_callback_t)){};
    MOCK_CONST_METHOD1(in_air_async, void(Telemetry::in_air_callback_t)){};
    MOCK_CONST_METHOD1(status_text_async, void(Telemetry::status_text_callback_t)){};
    MOCK_CONST_METHOD1(armed_async, void(Telemetry::armed_callback_t)){};
    MOCK_CONST_METHOD1(gps_info_async, void(Telemetry::gps_info_callback_t)){};
    MOCK_CONST_METHOD1(battery_async, void(Telemetry::battery_callback_t)){};
    MOCK_CONST_METHOD1(flight_mode_async, void(Telemetry::flight_mode_callback_t)){};
    MOCK_CONST_METHOD1(landed_state_async, void(Telemetry::landed_state_callback_t)){};
    MOCK_CONST_METHOD1(
        attitude_quaternion_async, void(Telemetry::attitude_quaternion_callback_t)){};
    MOCK_CONST_METHOD1(
        attitude_angular_velocity_body_async,
        void(Telemetry::attitude_angular_velocity_body_callback_t)){};
    MOCK_CONST_METHOD1(attitude_euler_async, void(Telemetry::attitude_euler_callback_t)){};
    MOCK_CONST_METHOD1(
        camera_attitude_quaternion_async, void(Telemetry::attitude_quaternion_callback_t)){};
    MOCK_CONST_METHOD1(camera_attitude_euler_async, void(Telemetry::attitude_euler_callback_t)){};
    MOCK_CONST_METHOD1(ground_speed_ned_async, void(Telemetry::ground_speed_ned_callback_t)){};
    MOCK_CONST_METHOD1(rc_status_async, void(Telemetry::rc_status_callback_t)){};
    MOCK_CONST_METHOD1(
        actuator_control_target_async, void(Telemetry::actuator_control_target_callback_t)){};
    MOCK_CONST_METHOD1(
        actuator_output_status_async, void(Telemetry::actuator_output_status_callback_t)){};
    MOCK_CONST_METHOD1(odometry_async, void(Telemetry::odometry_callback_t)){};
    MOCK_CONST_METHOD1(
        position_velocity_ned_async, void(Telemetry::position_velocity_ned_callback_t)){};
    MOCK_CONST_METHOD1(ground_truth_async, void(Telemetry::ground_truth_callback_t)){};
    MOCK_CONST_METHOD1(fixedwing_metrics_async, void(Telemetry::fixedwing_metrics_callback_t)){};
    MOCK_CONST_METHOD1(imu_async, void(Telemetry::imu_callback_t)){};
    MOCK_CONST_METHOD1(health_all_ok_async, void(Telemetry::health_all_ok_callback_t)){};
    MOCK_CONST_METHOD1(unix_epoch_time_async, void(Telemetry::unix_epoch_time_callback_t)){};

    MOCK_METHOD1(set_rate_position, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_home, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_in_air, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_landed_state, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_attitude, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_camera_attitude, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_ground_speed_ned, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_gps_info, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_battery, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_rc_status, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_actuator_control_target, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_actuator_output_status, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_odometry, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_position_velocity_ned, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_ground_truth, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_fixedwing_metrics, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_imu, Telemetry::Result(double)){};
    MOCK_METHOD1(set_rate_unix_epoch_time, Telemetry::Result(double)){};

    MOCK_CONST_METHOD2(set_rate_position_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_home_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_in_air_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_landed_state_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_attitude_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(
        set_rate_camera_attitude_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(
        set_rate_ground_speed_ned_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_gps_info_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_battery_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_rc_status_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(
        set_rate_actuator_control_target_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(
        set_rate_actuator_output_status_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_odometry_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(
        set_rate_position_velocity_ned_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_ground_truth_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(
        set_rate_fixedwing_metrics_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(set_rate_imu_async, void(double, Telemetry::result_callback_t)){};
    MOCK_CONST_METHOD2(
        set_rate_unix_epoch_time_async, void(double, Telemetry::result_callback_t)){};
};

} // namespace testing
} // namespace mavsdk
