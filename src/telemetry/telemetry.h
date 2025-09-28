#ifndef CMAVSDK_TELEMETRY_H
#define CMAVSDK_TELEMETRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "mavsdk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

// ===== Forward Declarations =====
typedef void* mavsdk_system_t;

// ===== Opaque Handles =====
typedef void* mavsdk_telemetry_t;
typedef void* mavsdk_telemetry_position_handle_t;
typedef void* mavsdk_telemetry_home_handle_t;
typedef void* mavsdk_telemetry_in_air_handle_t;
typedef void* mavsdk_telemetry_landed_state_handle_t;
typedef void* mavsdk_telemetry_armed_handle_t;
typedef void* mavsdk_telemetry_vtol_state_handle_t;
typedef void* mavsdk_telemetry_attitude_quaternion_handle_t;
typedef void* mavsdk_telemetry_attitude_euler_handle_t;
typedef void* mavsdk_telemetry_attitude_angular_velocity_body_handle_t;
typedef void* mavsdk_telemetry_velocity_ned_handle_t;
typedef void* mavsdk_telemetry_gps_info_handle_t;
typedef void* mavsdk_telemetry_raw_gps_handle_t;
typedef void* mavsdk_telemetry_battery_handle_t;
typedef void* mavsdk_telemetry_flight_mode_handle_t;
typedef void* mavsdk_telemetry_health_handle_t;
typedef void* mavsdk_telemetry_rc_status_handle_t;
typedef void* mavsdk_telemetry_status_text_handle_t;
typedef void* mavsdk_telemetry_actuator_control_target_handle_t;
typedef void* mavsdk_telemetry_actuator_output_status_handle_t;
typedef void* mavsdk_telemetry_odometry_handle_t;
typedef void* mavsdk_telemetry_position_velocity_ned_handle_t;
typedef void* mavsdk_telemetry_ground_truth_handle_t;
typedef void* mavsdk_telemetry_fixedwing_metrics_handle_t;
typedef void* mavsdk_telemetry_imu_handle_t;
typedef void* mavsdk_telemetry_scaled_imu_handle_t;
typedef void* mavsdk_telemetry_raw_imu_handle_t;
typedef void* mavsdk_telemetry_health_all_ok_handle_t;
typedef void* mavsdk_telemetry_unix_epoch_time_handle_t;
typedef void* mavsdk_telemetry_distance_sensor_handle_t;
typedef void* mavsdk_telemetry_scaled_pressure_handle_t;
typedef void* mavsdk_telemetry_heading_handle_t;
typedef void* mavsdk_telemetry_altitude_handle_t;
typedef void* mavsdk_telemetry_wind_handle_t;

// ===== Enums =====
typedef enum {
    MAVSDK_TELEMETRY_FIX_TYPE_NO_GPS = 0,
    MAVSDK_TELEMETRY_FIX_TYPE_NO_FIX = 1,
    MAVSDK_TELEMETRY_FIX_TYPE_FIX_2D = 2,
    MAVSDK_TELEMETRY_FIX_TYPE_FIX_3D = 3,
    MAVSDK_TELEMETRY_FIX_TYPE_FIX_DGPS = 4,
    MAVSDK_TELEMETRY_FIX_TYPE_RTK_FLOAT = 5,
    MAVSDK_TELEMETRY_FIX_TYPE_RTK_FIXED = 6,
} mavsdk_telemetry_fix_type_t;

typedef enum {
    MAVSDK_TELEMETRY_BATTERY_FUNCTION_UNKNOWN = 0,
    MAVSDK_TELEMETRY_BATTERY_FUNCTION_ALL = 1,
    MAVSDK_TELEMETRY_BATTERY_FUNCTION_PROPULSION = 2,
    MAVSDK_TELEMETRY_BATTERY_FUNCTION_AVIONICS = 3,
    MAVSDK_TELEMETRY_BATTERY_FUNCTION_PAYLOAD = 4,
} mavsdk_telemetry_battery_function_t;

typedef enum {
    MAVSDK_TELEMETRY_FLIGHT_MODE_UNKNOWN = 0,
    MAVSDK_TELEMETRY_FLIGHT_MODE_READY = 1,
    MAVSDK_TELEMETRY_FLIGHT_MODE_TAKEOFF = 2,
    MAVSDK_TELEMETRY_FLIGHT_MODE_HOLD = 3,
    MAVSDK_TELEMETRY_FLIGHT_MODE_MISSION = 4,
    MAVSDK_TELEMETRY_FLIGHT_MODE_RETURN_TO_LAUNCH = 5,
    MAVSDK_TELEMETRY_FLIGHT_MODE_LAND = 6,
    MAVSDK_TELEMETRY_FLIGHT_MODE_OFFBOARD = 7,
    MAVSDK_TELEMETRY_FLIGHT_MODE_FOLLOW_ME = 8,
    MAVSDK_TELEMETRY_FLIGHT_MODE_MANUAL = 9,
    MAVSDK_TELEMETRY_FLIGHT_MODE_ALTCTL = 10,
    MAVSDK_TELEMETRY_FLIGHT_MODE_POSCTL = 11,
    MAVSDK_TELEMETRY_FLIGHT_MODE_ACRO = 12,
    MAVSDK_TELEMETRY_FLIGHT_MODE_STABILIZED = 13,
    MAVSDK_TELEMETRY_FLIGHT_MODE_RATTITUDE = 14,
} mavsdk_telemetry_flight_mode_t;

typedef enum {
    MAVSDK_TELEMETRY_STATUS_TEXT_TYPE_DEBUG = 0,
    MAVSDK_TELEMETRY_STATUS_TEXT_TYPE_INFO = 1,
    MAVSDK_TELEMETRY_STATUS_TEXT_TYPE_NOTICE = 2,
    MAVSDK_TELEMETRY_STATUS_TEXT_TYPE_WARNING = 3,
    MAVSDK_TELEMETRY_STATUS_TEXT_TYPE_ERROR = 4,
    MAVSDK_TELEMETRY_STATUS_TEXT_TYPE_CRITICAL = 5,
    MAVSDK_TELEMETRY_STATUS_TEXT_TYPE_ALERT = 6,
    MAVSDK_TELEMETRY_STATUS_TEXT_TYPE_EMERGENCY = 7,
} mavsdk_telemetry_status_text_type_t;

typedef enum {
    MAVSDK_TELEMETRY_LANDED_STATE_UNKNOWN = 0,
    MAVSDK_TELEMETRY_LANDED_STATE_ON_GROUND = 1,
    MAVSDK_TELEMETRY_LANDED_STATE_IN_AIR = 2,
    MAVSDK_TELEMETRY_LANDED_STATE_TAKING_OFF = 3,
    MAVSDK_TELEMETRY_LANDED_STATE_LANDING = 4,
} mavsdk_telemetry_landed_state_t;

typedef enum {
    MAVSDK_TELEMETRY_VTOL_STATE_UNDEFINED = 0,
    MAVSDK_TELEMETRY_VTOL_STATE_TRANSITION_TO_FW = 1,
    MAVSDK_TELEMETRY_VTOL_STATE_TRANSITION_TO_MC = 2,
    MAVSDK_TELEMETRY_VTOL_STATE_MC = 3,
    MAVSDK_TELEMETRY_VTOL_STATE_FW = 4,
} mavsdk_telemetry_vtol_state_t;


// ===== Structs =====
typedef struct {
    double latitude_deg;
    double longitude_deg;
    float absolute_altitude_m;
    float relative_altitude_m;
} mavsdk_telemetry_position_t;


typedef struct {
    double heading_deg;
} mavsdk_telemetry_heading_t;


typedef struct {
    float w;
    float x;
    float y;
    float z;
    uint64_t timestamp_us;
} mavsdk_telemetry_quaternion_t;


typedef struct {
    float roll_deg;
    float pitch_deg;
    float yaw_deg;
    uint64_t timestamp_us;
} mavsdk_telemetry_euler_angle_t;


typedef struct {
    float roll_rad_s;
    float pitch_rad_s;
    float yaw_rad_s;
} mavsdk_telemetry_angular_velocity_body_t;


typedef struct {
    int32_t num_satellites;
    mavsdk_telemetry_fix_type_t fix_type;
} mavsdk_telemetry_gps_info_t;


typedef struct {
    uint64_t timestamp_us;
    double latitude_deg;
    double longitude_deg;
    float absolute_altitude_m;
    float hdop;
    float vdop;
    float velocity_m_s;
    float cog_deg;
    float altitude_ellipsoid_m;
    float horizontal_uncertainty_m;
    float vertical_uncertainty_m;
    float velocity_uncertainty_m_s;
    float heading_uncertainty_deg;
    float yaw_deg;
} mavsdk_telemetry_raw_gps_t;


typedef struct {
    uint32_t id;
    float temperature_degc;
    float voltage_v;
    float current_battery_a;
    float capacity_consumed_ah;
    float remaining_percent;
    float time_remaining_s;
    mavsdk_telemetry_battery_function_t battery_function;
} mavsdk_telemetry_battery_t;


typedef struct {
    bool is_gyrometer_calibration_ok;
    bool is_accelerometer_calibration_ok;
    bool is_magnetometer_calibration_ok;
    bool is_local_position_ok;
    bool is_global_position_ok;
    bool is_home_position_ok;
    bool is_armable;
} mavsdk_telemetry_health_t;


typedef struct {
    bool was_available_once;
    bool is_available;
    float signal_strength_percent;
} mavsdk_telemetry_rc_status_t;


typedef struct {
    mavsdk_telemetry_status_text_type_t type;
    char* text;
} mavsdk_telemetry_status_text_t;


typedef struct {
    int32_t group;
    float* controls;
    int controls_size;
} mavsdk_telemetry_actuator_control_target_t;

void mavsdk_telemetry_actuator_control_target_destroy(
    mavsdk_telemetry_actuator_control_target_t* target);

typedef struct {
    uint32_t active;
    float* actuator;
    int actuator_size;
} mavsdk_telemetry_actuator_output_status_t;

void mavsdk_telemetry_actuator_output_status_destroy(
    mavsdk_telemetry_actuator_output_status_t* target);

typedef struct {
    float* covariance_matrix;
    int covariance_matrix_size;
} mavsdk_telemetry_covariance_t;

void mavsdk_telemetry_covariance_destroy(
    mavsdk_telemetry_covariance_t* target);

typedef struct {
    float x_m_s;
    float y_m_s;
    float z_m_s;
} mavsdk_telemetry_velocity_body_t;


typedef struct {
    float x_m;
    float y_m;
    float z_m;
} mavsdk_telemetry_position_body_t;


typedef enum {
    MAVSDK_TELEMETRY_ODOMETRY_MAV_FRAME_UNDEF = 0,
    MAVSDK_TELEMETRY_ODOMETRY_MAV_FRAME_BODY_NED = 1,
    MAVSDK_TELEMETRY_ODOMETRY_MAV_FRAME_VISION_NED = 2,
    MAVSDK_TELEMETRY_ODOMETRY_MAV_FRAME_ESTIM_NED = 3,
} mavsdk_telemetry_odometry_mav_frame_t;

typedef struct {
    uint64_t time_usec;
    mavsdk_telemetry_odometry_mav_frame_t frame_id;
    mavsdk_telemetry_odometry_mav_frame_t child_frame_id;
    mavsdk_telemetry_position_body_t position_body;
    mavsdk_telemetry_quaternion_t q;
    mavsdk_telemetry_velocity_body_t velocity_body;
    mavsdk_telemetry_angular_velocity_body_t angular_velocity_body;
    mavsdk_telemetry_covariance_t pose_covariance;
    mavsdk_telemetry_covariance_t velocity_covariance;
} mavsdk_telemetry_odometry_t;


typedef struct {
    float minimum_distance_m;
    float maximum_distance_m;
    float current_distance_m;
    mavsdk_telemetry_euler_angle_t orientation;
} mavsdk_telemetry_distance_sensor_t;


typedef struct {
    uint64_t timestamp_us;
    float absolute_pressure_hpa;
    float differential_pressure_hpa;
    float temperature_deg;
    float differential_pressure_temperature_deg;
} mavsdk_telemetry_scaled_pressure_t;


typedef struct {
    float north_m;
    float east_m;
    float down_m;
} mavsdk_telemetry_position_ned_t;


typedef struct {
    float north_m_s;
    float east_m_s;
    float down_m_s;
} mavsdk_telemetry_velocity_ned_t;


typedef struct {
    mavsdk_telemetry_position_ned_t position;
    mavsdk_telemetry_velocity_ned_t velocity;
} mavsdk_telemetry_position_velocity_ned_t;


typedef struct {
    double latitude_deg;
    double longitude_deg;
    float absolute_altitude_m;
} mavsdk_telemetry_ground_truth_t;


typedef struct {
    float airspeed_m_s;
    float throttle_percentage;
    float climb_rate_m_s;
    float groundspeed_m_s;
    float heading_deg;
    float absolute_altitude_m;
} mavsdk_telemetry_fixedwing_metrics_t;


typedef struct {
    float forward_m_s2;
    float right_m_s2;
    float down_m_s2;
} mavsdk_telemetry_acceleration_frd_t;


typedef struct {
    float forward_rad_s;
    float right_rad_s;
    float down_rad_s;
} mavsdk_telemetry_angular_velocity_frd_t;


typedef struct {
    float forward_gauss;
    float right_gauss;
    float down_gauss;
} mavsdk_telemetry_magnetic_field_frd_t;


typedef struct {
    mavsdk_telemetry_acceleration_frd_t acceleration_frd;
    mavsdk_telemetry_angular_velocity_frd_t angular_velocity_frd;
    mavsdk_telemetry_magnetic_field_frd_t magnetic_field_frd;
    float temperature_degc;
    uint64_t timestamp_us;
} mavsdk_telemetry_imu_t;


typedef struct {
    double latitude_deg;
    double longitude_deg;
    float altitude_m;
} mavsdk_telemetry_gps_global_origin_t;


typedef struct {
    float altitude_monotonic_m;
    float altitude_amsl_m;
    float altitude_local_m;
    float altitude_relative_m;
    float altitude_terrain_m;
    float bottom_clearance_m;
} mavsdk_telemetry_altitude_t;


typedef struct {
    float wind_x_ned_m_s;
    float wind_y_ned_m_s;
    float wind_z_ned_m_s;
    float horizontal_variability_stddev_m_s;
    float vertical_variability_stddev_m_s;
    float wind_altitude_msl_m;
    float horizontal_wind_speed_accuracy_m_s;
    float vertical_wind_speed_accuracy_m_s;
} mavsdk_telemetry_wind_t;


typedef enum {
    MAVSDK_TELEMETRY_RESULT_UNKNOWN = 0,
    MAVSDK_TELEMETRY_RESULT_SUCCESS = 1,
    MAVSDK_TELEMETRY_RESULT_NO_SYSTEM = 2,
    MAVSDK_TELEMETRY_RESULT_CONNECTION_ERROR = 3,
    MAVSDK_TELEMETRY_RESULT_BUSY = 4,
    MAVSDK_TELEMETRY_RESULT_COMMAND_DENIED = 5,
    MAVSDK_TELEMETRY_RESULT_TIMEOUT = 6,
    MAVSDK_TELEMETRY_RESULT_UNSUPPORTED = 7,
} mavsdk_telemetry_result_t;




// ===== Callback Typedefs =====
typedef void (*mavsdk_telemetry_position_callback_t)(const mavsdk_telemetry_position_t position, void* user_data);
typedef void (*mavsdk_telemetry_home_callback_t)(const mavsdk_telemetry_position_t home, void* user_data);
typedef void (*mavsdk_telemetry_in_air_callback_t)(const bool in_air, void* user_data);
typedef void (*mavsdk_telemetry_landed_state_callback_t)(const mavsdk_telemetry_landed_state_t landed_state, void* user_data);
typedef void (*mavsdk_telemetry_armed_callback_t)(const bool armed, void* user_data);
typedef void (*mavsdk_telemetry_vtol_state_callback_t)(const mavsdk_telemetry_vtol_state_t vtol_state, void* user_data);
typedef void (*mavsdk_telemetry_attitude_quaternion_callback_t)(const mavsdk_telemetry_quaternion_t attitude_quaternion, void* user_data);
typedef void (*mavsdk_telemetry_attitude_euler_callback_t)(const mavsdk_telemetry_euler_angle_t attitude_euler, void* user_data);
typedef void (*mavsdk_telemetry_attitude_angular_velocity_body_callback_t)(const mavsdk_telemetry_angular_velocity_body_t attitude_angular_velocity_body, void* user_data);
typedef void (*mavsdk_telemetry_velocity_ned_callback_t)(const mavsdk_telemetry_velocity_ned_t velocity_ned, void* user_data);
typedef void (*mavsdk_telemetry_gps_info_callback_t)(const mavsdk_telemetry_gps_info_t gps_info, void* user_data);
typedef void (*mavsdk_telemetry_raw_gps_callback_t)(const mavsdk_telemetry_raw_gps_t raw_gps, void* user_data);
typedef void (*mavsdk_telemetry_battery_callback_t)(const mavsdk_telemetry_battery_t battery, void* user_data);
typedef void (*mavsdk_telemetry_flight_mode_callback_t)(const mavsdk_telemetry_flight_mode_t flight_mode, void* user_data);
typedef void (*mavsdk_telemetry_health_callback_t)(const mavsdk_telemetry_health_t health, void* user_data);
typedef void (*mavsdk_telemetry_rc_status_callback_t)(const mavsdk_telemetry_rc_status_t rc_status, void* user_data);
typedef void (*mavsdk_telemetry_status_text_callback_t)(const mavsdk_telemetry_status_text_t status_text, void* user_data);
typedef void (*mavsdk_telemetry_actuator_control_target_callback_t)(const mavsdk_telemetry_actuator_control_target_t actuator_control_target, void* user_data);
typedef void (*mavsdk_telemetry_actuator_output_status_callback_t)(const mavsdk_telemetry_actuator_output_status_t actuator_output_status, void* user_data);
typedef void (*mavsdk_telemetry_odometry_callback_t)(const mavsdk_telemetry_odometry_t odometry, void* user_data);
typedef void (*mavsdk_telemetry_position_velocity_ned_callback_t)(const mavsdk_telemetry_position_velocity_ned_t position_velocity_ned, void* user_data);
typedef void (*mavsdk_telemetry_ground_truth_callback_t)(const mavsdk_telemetry_ground_truth_t ground_truth, void* user_data);
typedef void (*mavsdk_telemetry_fixedwing_metrics_callback_t)(const mavsdk_telemetry_fixedwing_metrics_t fixedwing_metrics, void* user_data);
typedef void (*mavsdk_telemetry_imu_callback_t)(const mavsdk_telemetry_imu_t imu, void* user_data);
typedef void (*mavsdk_telemetry_scaled_imu_callback_t)(const mavsdk_telemetry_imu_t scaled_imu, void* user_data);
typedef void (*mavsdk_telemetry_raw_imu_callback_t)(const mavsdk_telemetry_imu_t raw_imu, void* user_data);
typedef void (*mavsdk_telemetry_health_all_ok_callback_t)(const bool health_all_ok, void* user_data);
typedef void (*mavsdk_telemetry_unix_epoch_time_callback_t)(const uint64_t unix_epoch_time, void* user_data);
typedef void (*mavsdk_telemetry_distance_sensor_callback_t)(const mavsdk_telemetry_distance_sensor_t distance_sensor, void* user_data);
typedef void (*mavsdk_telemetry_scaled_pressure_callback_t)(const mavsdk_telemetry_scaled_pressure_t scaled_pressure, void* user_data);
typedef void (*mavsdk_telemetry_heading_callback_t)(const mavsdk_telemetry_heading_t heading, void* user_data);
typedef void (*mavsdk_telemetry_altitude_callback_t)(const mavsdk_telemetry_altitude_t altitude, void* user_data);
typedef void (*mavsdk_telemetry_wind_callback_t)(const mavsdk_telemetry_wind_t wind, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_position_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_home_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_in_air_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_landed_state_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_vtol_state_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_attitude_quaternion_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_attitude_euler_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_velocity_ned_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_gps_info_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_battery_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_rc_status_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_actuator_control_target_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_actuator_output_status_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_odometry_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_position_velocity_ned_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_ground_truth_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_fixedwing_metrics_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_imu_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_scaled_imu_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_raw_imu_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_unix_epoch_time_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_distance_sensor_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_altitude_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_set_rate_health_callback_t)(const mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_get_gps_global_origin_callback_t)(const mavsdk_telemetry_result_t result, const mavsdk_telemetry_gps_global_origin_t get_gps_global_origin, void* user_data);

// ===== Telemetry Creation/Destruction =====
CMAVSDK_EXPORT mavsdk_telemetry_t mavsdk_telemetry_create(mavsdk_system_t system);
CMAVSDK_EXPORT void mavsdk_telemetry_destroy(mavsdk_telemetry_t telemetry);

// ===== Methods =====

CMAVSDK_EXPORT mavsdk_telemetry_position_handle_t mavsdk_telemetry_subscribe_position(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_position(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_position(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_t* position_out);


CMAVSDK_EXPORT mavsdk_telemetry_home_handle_t mavsdk_telemetry_subscribe_home(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_home_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_home(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_home_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_home(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_t* home_out);


CMAVSDK_EXPORT mavsdk_telemetry_in_air_handle_t mavsdk_telemetry_subscribe_in_air(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_in_air_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_in_air(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_in_air_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_in_air(
    mavsdk_telemetry_t telemetry,
    bool* is_in_air_out
);


CMAVSDK_EXPORT mavsdk_telemetry_landed_state_handle_t mavsdk_telemetry_subscribe_landed_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_landed_state_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_landed_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_landed_state_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_landed_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_landed_state_t* landed_state_out);


CMAVSDK_EXPORT mavsdk_telemetry_armed_handle_t mavsdk_telemetry_subscribe_armed(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_armed_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_armed(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_armed_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_armed(
    mavsdk_telemetry_t telemetry,
    bool* is_armed_out
);


CMAVSDK_EXPORT mavsdk_telemetry_vtol_state_handle_t mavsdk_telemetry_subscribe_vtol_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_vtol_state_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_vtol_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_vtol_state_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_vtol_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_vtol_state_t* vtol_state_out);


CMAVSDK_EXPORT mavsdk_telemetry_attitude_quaternion_handle_t mavsdk_telemetry_subscribe_attitude_quaternion(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_quaternion_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_attitude_quaternion(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_quaternion_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_attitude_quaternion(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_quaternion_t* attitude_quaternion_out);


CMAVSDK_EXPORT mavsdk_telemetry_attitude_euler_handle_t mavsdk_telemetry_subscribe_attitude_euler(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_euler_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_attitude_euler(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_euler_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_attitude_euler(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_euler_angle_t* attitude_euler_out);


CMAVSDK_EXPORT mavsdk_telemetry_attitude_angular_velocity_body_handle_t mavsdk_telemetry_subscribe_attitude_angular_velocity_body(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_angular_velocity_body_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_attitude_angular_velocity_body(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_angular_velocity_body_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_attitude_angular_velocity_body(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_angular_velocity_body_t* attitude_angular_velocity_body_out);


CMAVSDK_EXPORT mavsdk_telemetry_velocity_ned_handle_t mavsdk_telemetry_subscribe_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_velocity_ned_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_velocity_ned_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_velocity_ned_t* velocity_ned_out);


CMAVSDK_EXPORT mavsdk_telemetry_gps_info_handle_t mavsdk_telemetry_subscribe_gps_info(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_gps_info_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_gps_info(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_gps_info_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_gps_info(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_gps_info_t* gps_info_out);


CMAVSDK_EXPORT mavsdk_telemetry_raw_gps_handle_t mavsdk_telemetry_subscribe_raw_gps(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_raw_gps_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_raw_gps(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_raw_gps_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_raw_gps(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_raw_gps_t* raw_gps_out);


CMAVSDK_EXPORT mavsdk_telemetry_battery_handle_t mavsdk_telemetry_subscribe_battery(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_battery_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_battery(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_battery_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_battery(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_battery_t* battery_out);


CMAVSDK_EXPORT mavsdk_telemetry_flight_mode_handle_t mavsdk_telemetry_subscribe_flight_mode(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_flight_mode_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_flight_mode(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_flight_mode_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_flight_mode(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_flight_mode_t* flight_mode_out);


CMAVSDK_EXPORT mavsdk_telemetry_health_handle_t mavsdk_telemetry_subscribe_health(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_health(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_health(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_t* health_out);


CMAVSDK_EXPORT mavsdk_telemetry_rc_status_handle_t mavsdk_telemetry_subscribe_rc_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_rc_status_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_rc_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_rc_status_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_rc_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_rc_status_t* rc_status_out);


CMAVSDK_EXPORT mavsdk_telemetry_status_text_handle_t mavsdk_telemetry_subscribe_status_text(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_status_text_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_status_text(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_status_text_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_status_text(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_status_text_t* status_text_out);


CMAVSDK_EXPORT mavsdk_telemetry_actuator_control_target_handle_t mavsdk_telemetry_subscribe_actuator_control_target(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_actuator_control_target_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_actuator_control_target(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_actuator_control_target_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_actuator_control_target(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_actuator_control_target_t* actuator_control_target_out);


CMAVSDK_EXPORT mavsdk_telemetry_actuator_output_status_handle_t mavsdk_telemetry_subscribe_actuator_output_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_actuator_output_status_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_actuator_output_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_actuator_output_status_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_actuator_output_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_actuator_output_status_t* actuator_output_status_out);


CMAVSDK_EXPORT mavsdk_telemetry_odometry_handle_t mavsdk_telemetry_subscribe_odometry(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_odometry_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_odometry(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_odometry_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_odometry(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_odometry_t* odometry_out);


CMAVSDK_EXPORT mavsdk_telemetry_position_velocity_ned_handle_t mavsdk_telemetry_subscribe_position_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_velocity_ned_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_position_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_velocity_ned_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_position_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_velocity_ned_t* position_velocity_ned_out);


CMAVSDK_EXPORT mavsdk_telemetry_ground_truth_handle_t mavsdk_telemetry_subscribe_ground_truth(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_ground_truth_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_ground_truth(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_ground_truth_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_ground_truth(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_ground_truth_t* ground_truth_out);


CMAVSDK_EXPORT mavsdk_telemetry_fixedwing_metrics_handle_t mavsdk_telemetry_subscribe_fixedwing_metrics(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_fixedwing_metrics_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_fixedwing_metrics(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_fixedwing_metrics_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_fixedwing_metrics(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_fixedwing_metrics_t* fixedwing_metrics_out);


CMAVSDK_EXPORT mavsdk_telemetry_imu_handle_t mavsdk_telemetry_subscribe_imu(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_imu_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_imu(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_imu_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_imu(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_imu_t* imu_out);


CMAVSDK_EXPORT mavsdk_telemetry_scaled_imu_handle_t mavsdk_telemetry_subscribe_scaled_imu(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_scaled_imu_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_scaled_imu(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_scaled_imu_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_scaled_imu(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_imu_t* imu_out);


CMAVSDK_EXPORT mavsdk_telemetry_raw_imu_handle_t mavsdk_telemetry_subscribe_raw_imu(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_raw_imu_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_raw_imu(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_raw_imu_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_raw_imu(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_imu_t* imu_out);


CMAVSDK_EXPORT mavsdk_telemetry_health_all_ok_handle_t mavsdk_telemetry_subscribe_health_all_ok(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_all_ok_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_health_all_ok(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_all_ok_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_health_all_ok(
    mavsdk_telemetry_t telemetry,
    bool* is_health_all_ok_out
);


CMAVSDK_EXPORT mavsdk_telemetry_unix_epoch_time_handle_t mavsdk_telemetry_subscribe_unix_epoch_time(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_unix_epoch_time_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_unix_epoch_time(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_unix_epoch_time_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_unix_epoch_time(
    mavsdk_telemetry_t telemetry,
    uint64_t* time_us_out
);


CMAVSDK_EXPORT mavsdk_telemetry_distance_sensor_handle_t mavsdk_telemetry_subscribe_distance_sensor(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_distance_sensor_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_distance_sensor(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_distance_sensor_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_distance_sensor(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_distance_sensor_t* distance_sensor_out);


CMAVSDK_EXPORT mavsdk_telemetry_scaled_pressure_handle_t mavsdk_telemetry_subscribe_scaled_pressure(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_scaled_pressure_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_scaled_pressure(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_scaled_pressure_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_scaled_pressure(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_scaled_pressure_t* scaled_pressure_out);


CMAVSDK_EXPORT mavsdk_telemetry_heading_handle_t mavsdk_telemetry_subscribe_heading(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_heading_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_heading(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_heading_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_heading(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_heading_t* heading_deg_out);


CMAVSDK_EXPORT mavsdk_telemetry_altitude_handle_t mavsdk_telemetry_subscribe_altitude(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_altitude_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_altitude(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_altitude_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_altitude(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_altitude_t* altitude_out);


CMAVSDK_EXPORT mavsdk_telemetry_wind_handle_t mavsdk_telemetry_subscribe_wind(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_wind_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_wind(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_wind_handle_t);

CMAVSDK_EXPORT
void
mavsdk_telemetry_wind(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_wind_t* wind_out);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_position_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_position_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_position(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_home_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_home_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_home(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_in_air_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_in_air_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_in_air(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_landed_state_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_landed_state_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_landed_state(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_vtol_state_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_vtol_state_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_vtol_state(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_attitude_quaternion_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_attitude_quaternion_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_attitude_quaternion(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_attitude_euler_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_attitude_euler_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_attitude_euler(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_velocity_ned_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_velocity_ned_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_velocity_ned(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_gps_info_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_gps_info_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_gps_info(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_battery_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_battery_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_battery(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_rc_status_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_rc_status_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_rc_status(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_actuator_control_target_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_actuator_control_target_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_actuator_control_target(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_actuator_output_status_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_actuator_output_status_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_actuator_output_status(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_odometry_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_odometry_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_odometry(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_position_velocity_ned_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_position_velocity_ned_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_position_velocity_ned(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_ground_truth_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_ground_truth_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_ground_truth(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_fixedwing_metrics_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_fixedwing_metrics_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_fixedwing_metrics(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_imu_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_imu_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_imu(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_scaled_imu_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_scaled_imu_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_scaled_imu(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_raw_imu_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_raw_imu_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_raw_imu(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_unix_epoch_time_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_unix_epoch_time_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_unix_epoch_time(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_distance_sensor_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_distance_sensor_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_distance_sensor(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_altitude_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_altitude_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_altitude(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_health_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_set_rate_health_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_set_rate_health(
    mavsdk_telemetry_t telemetry,
    double rate_hz);


CMAVSDK_EXPORT void mavsdk_telemetry_get_gps_global_origin_async(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_get_gps_global_origin_callback_t callback,
    void* user_data);


CMAVSDK_EXPORT
mavsdk_telemetry_result_t
mavsdk_telemetry_get_gps_global_origin(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_gps_global_origin_t* gps_global_origin_out);


#ifdef __cplusplus
}
#endif

#endif // CMAVSDK_TELEMETRY_H