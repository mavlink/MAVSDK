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
typedef void* mavsdk_telemetry_t;

// ===== Opaque Subscription Handles =====
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
typedef void* mavsdk_telemetry_health_all_ok_handle_t;

// ===== Enums =====
typedef enum {
    MAVSDK_TELEMETRY_RESULT_UNKNOWN = 0,
    MAVSDK_TELEMETRY_RESULT_SUCCESS = 1,
    MAVSDK_TELEMETRY_RESULT_NO_SYSTEM = 2,
    MAVSDK_TELEMETRY_RESULT_CONNECTION_ERROR = 3,
    MAVSDK_TELEMETRY_RESULT_BUSY = 4,
    MAVSDK_TELEMETRY_RESULT_COMMAND_DENIED = 5,
    MAVSDK_TELEMETRY_RESULT_TIMEOUT = 6,
    MAVSDK_TELEMETRY_RESULT_UNSUPPORTED = 7
} mavsdk_telemetry_result_t;

typedef enum {
    MAVSDK_TELEMETRY_FIX_TYPE_NO_GPS = 0,
    MAVSDK_TELEMETRY_FIX_TYPE_NO_FIX = 1,
    MAVSDK_TELEMETRY_FIX_TYPE_FIX_2D = 2,
    MAVSDK_TELEMETRY_FIX_TYPE_FIX_3D = 3,
    MAVSDK_TELEMETRY_FIX_TYPE_FIX_DGPS = 4,
    MAVSDK_TELEMETRY_FIX_TYPE_RTK_FLOAT = 5,
    MAVSDK_TELEMETRY_FIX_TYPE_RTK_FIXED = 6
} mavsdk_telemetry_fix_type_t;

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
    MAVSDK_TELEMETRY_FLIGHT_MODE_RATTITUDE = 14
} mavsdk_telemetry_flight_mode_t;

typedef enum {
    MAVSDK_TELEMETRY_LANDED_STATE_UNKNOWN = 0,
    MAVSDK_TELEMETRY_LANDED_STATE_ON_GROUND = 1,
    MAVSDK_TELEMETRY_LANDED_STATE_IN_AIR = 2,
    MAVSDK_TELEMETRY_LANDED_STATE_TAKING_OFF = 3,
    MAVSDK_TELEMETRY_LANDED_STATE_LANDING = 4
} mavsdk_telemetry_landed_state_t;

typedef enum {
    MAVSDK_TELEMETRY_VTOL_STATE_UNDEFINED = 0,
    MAVSDK_TELEMETRY_VTOL_STATE_TRANSITION_TO_FW = 1,
    MAVSDK_TELEMETRY_VTOL_STATE_TRANSITION_TO_MC = 2,
    MAVSDK_TELEMETRY_VTOL_STATE_MC = 3,
    MAVSDK_TELEMETRY_VTOL_STATE_FW = 4
} mavsdk_telemetry_vtol_state_t;

// ===== Data Structures =====
typedef struct {
    double latitude_deg;
    double longitude_deg;
    float absolute_altitude_m;
    float relative_altitude_m;
} mavsdk_telemetry_position_t;

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
    float north_m_s;
    float east_m_s;
    float down_m_s;
} mavsdk_telemetry_velocity_ned_t;

typedef struct {
    int32_t num_satellites;
    mavsdk_telemetry_fix_type_t fix_type;
} mavsdk_telemetry_gps_info_t;

typedef struct {
    uint32_t id;
    float temperature_degc;
    float voltage_v;
    float current_battery_a;
    float capacity_consumed_ah;
    float remaining_percent;
    float time_remaining_s;
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
    const char* text;
    int type; // For simplicity, using int for status text type
} mavsdk_telemetry_status_text_t;

// ===== Callback Typedefs =====
typedef void (*mavsdk_telemetry_result_callback_t)(mavsdk_telemetry_result_t result, void* user_data);
typedef void (*mavsdk_telemetry_position_callback_t)(const mavsdk_telemetry_position_t* position, void* user_data);
typedef void (*mavsdk_telemetry_home_callback_t)(const mavsdk_telemetry_position_t* home, void* user_data);
typedef void (*mavsdk_telemetry_in_air_callback_t)(bool in_air, void* user_data);
typedef void (*mavsdk_telemetry_landed_state_callback_t)(mavsdk_telemetry_landed_state_t landed_state, void* user_data);
typedef void (*mavsdk_telemetry_armed_callback_t)(bool armed, void* user_data);
typedef void (*mavsdk_telemetry_vtol_state_callback_t)(mavsdk_telemetry_vtol_state_t vtol_state, void* user_data);
typedef void (*mavsdk_telemetry_attitude_quaternion_callback_t)(const mavsdk_telemetry_quaternion_t* quaternion, void* user_data);
typedef void (*mavsdk_telemetry_attitude_euler_callback_t)(const mavsdk_telemetry_euler_angle_t* euler_angle, void* user_data);
typedef void (*mavsdk_telemetry_attitude_angular_velocity_body_callback_t)(const mavsdk_telemetry_angular_velocity_body_t* angular_velocity, void* user_data);
typedef void (*mavsdk_telemetry_velocity_ned_callback_t)(const mavsdk_telemetry_velocity_ned_t* velocity, void* user_data);
typedef void (*mavsdk_telemetry_gps_info_callback_t)(const mavsdk_telemetry_gps_info_t* gps_info, void* user_data);
typedef void (*mavsdk_telemetry_battery_callback_t)(const mavsdk_telemetry_battery_t* battery, void* user_data);
typedef void (*mavsdk_telemetry_flight_mode_callback_t)(mavsdk_telemetry_flight_mode_t flight_mode, void* user_data);
typedef void (*mavsdk_telemetry_health_callback_t)(const mavsdk_telemetry_health_t* health, void* user_data);
typedef void (*mavsdk_telemetry_rc_status_callback_t)(const mavsdk_telemetry_rc_status_t* rc_status, void* user_data);
typedef void (*mavsdk_telemetry_status_text_callback_t)(const mavsdk_telemetry_status_text_t* status_text, void* user_data);
typedef void (*mavsdk_telemetry_health_all_ok_callback_t)(bool health_all_ok, void* user_data);

// ===== Telemetry Creation/Destruction =====
CMAVSDK_EXPORT mavsdk_telemetry_t mavsdk_telemetry_create(mavsdk_system_t system);
CMAVSDK_EXPORT void mavsdk_telemetry_destroy(mavsdk_telemetry_t telemetry);

// ===== Subscription Methods =====
CMAVSDK_EXPORT mavsdk_telemetry_position_handle_t mavsdk_telemetry_subscribe_position(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_position(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_home_handle_t mavsdk_telemetry_subscribe_home(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_home_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_home(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_home_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_in_air_handle_t mavsdk_telemetry_subscribe_in_air(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_in_air_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_in_air(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_in_air_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_landed_state_handle_t mavsdk_telemetry_subscribe_landed_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_landed_state_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_landed_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_landed_state_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_armed_handle_t mavsdk_telemetry_subscribe_armed(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_armed_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_armed(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_armed_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_vtol_state_handle_t mavsdk_telemetry_subscribe_vtol_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_vtol_state_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_vtol_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_vtol_state_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_attitude_quaternion_handle_t mavsdk_telemetry_subscribe_attitude_quaternion(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_quaternion_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_attitude_quaternion(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_quaternion_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_attitude_euler_handle_t mavsdk_telemetry_subscribe_attitude_euler(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_euler_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_attitude_euler(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_euler_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_attitude_angular_velocity_body_handle_t mavsdk_telemetry_subscribe_attitude_angular_velocity_body(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_angular_velocity_body_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_attitude_angular_velocity_body(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_angular_velocity_body_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_velocity_ned_handle_t mavsdk_telemetry_subscribe_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_velocity_ned_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_velocity_ned_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_gps_info_handle_t mavsdk_telemetry_subscribe_gps_info(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_gps_info_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_gps_info(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_gps_info_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_battery_handle_t mavsdk_telemetry_subscribe_battery(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_battery_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_battery(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_battery_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_flight_mode_handle_t mavsdk_telemetry_subscribe_flight_mode(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_flight_mode_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_flight_mode(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_flight_mode_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_health_handle_t mavsdk_telemetry_subscribe_health(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_health(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_rc_status_handle_t mavsdk_telemetry_subscribe_rc_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_rc_status_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_rc_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_rc_status_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_status_text_handle_t mavsdk_telemetry_subscribe_status_text(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_status_text_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_status_text(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_status_text_handle_t handle);

CMAVSDK_EXPORT mavsdk_telemetry_health_all_ok_handle_t mavsdk_telemetry_subscribe_health_all_ok(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_all_ok_callback_t callback,
    void* user_data);
CMAVSDK_EXPORT void mavsdk_telemetry_unsubscribe_health_all_ok(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_all_ok_handle_t handle);

// ===== Rate Setting Methods (Async) =====
CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_position_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_home_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_in_air_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_attitude_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_velocity_ned_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_gps_info_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_telemetry_set_rate_battery_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data);

// ===== Rate Setting Methods (Sync) =====
CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_position(
    mavsdk_telemetry_t telemetry,
    double rate_hz);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_home(
    mavsdk_telemetry_t telemetry,
    double rate_hz);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_in_air(
    mavsdk_telemetry_t telemetry,
    double rate_hz);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_attitude(
    mavsdk_telemetry_t telemetry,
    double rate_hz);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_velocity_ned(
    mavsdk_telemetry_t telemetry,
    double rate_hz);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_gps_info(
    mavsdk_telemetry_t telemetry,
    double rate_hz);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_battery(
    mavsdk_telemetry_t telemetry,
    double rate_hz);

// ===== Polling Methods =====
CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_position(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_t* position_out);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_home(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_t* home_out);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_in_air(
    mavsdk_telemetry_t telemetry,
    bool* in_air_out);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_landed_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_landed_state_t* landed_state_out);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_armed(
    mavsdk_telemetry_t telemetry,
    bool* armed_out);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_flight_mode(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_flight_mode_t* flight_mode_out);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_health(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_t* health_out);

CMAVSDK_EXPORT mavsdk_telemetry_result_t mavsdk_telemetry_battery(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_battery_t* battery_out);

#ifdef __cplusplus
}
#endif

#endif // CMAVSDK_TELEMETRY_H
