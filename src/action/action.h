#ifndef CMAVSDK_ACTION_H
#define CMAVSDK_ACTION_H

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
typedef void* mavsdk_action_t;

// ===== Enums =====
typedef enum {
    MAVSDK_ACTION_RESULT_UNKNOWN = 0,
    MAVSDK_ACTION_RESULT_SUCCESS = 1,
    MAVSDK_ACTION_RESULT_NO_SYSTEM = 2,
    MAVSDK_ACTION_RESULT_CONNECTION_ERROR = 3,
    MAVSDK_ACTION_RESULT_BUSY = 4,
    MAVSDK_ACTION_RESULT_COMMAND_DENIED = 5,
    MAVSDK_ACTION_RESULT_COMMAND_DENIED_LANDED_STATE_UNKNOWN = 6,
    MAVSDK_ACTION_RESULT_COMMAND_DENIED_NOT_LANDED = 7,
    MAVSDK_ACTION_RESULT_TIMEOUT = 8,
    MAVSDK_ACTION_RESULT_VTOL_TRANSITION_SUPPORT_UNKNOWN = 9,
    MAVSDK_ACTION_RESULT_NO_VTOL_TRANSITION_SUPPORT = 10,
    MAVSDK_ACTION_RESULT_PARAMETER_ERROR = 11,
    MAVSDK_ACTION_RESULT_UNSUPPORTED = 12,
    MAVSDK_ACTION_RESULT_FAILED = 13,
    MAVSDK_ACTION_RESULT_INVALID_ARGUMENT = 14
} mavsdk_action_result_t;

typedef enum {
    MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_HOLD_FRONT_TO_CIRCLE_CENTER = 0,
    MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_HOLD_INITIAL_HEADING = 1,
    MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_UNCONTROLLED = 2,
    MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_HOLD_FRONT_TANGENT_TO_CIRCLE = 3,
    MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_RC_CONTROLLED = 4
} mavsdk_action_orbit_yaw_behavior_t;

// ===== Callback Typedefs =====
typedef void (*mavsdk_action_result_callback_t)(mavsdk_action_result_t result, void* user_data);
typedef void (*mavsdk_action_get_altitude_callback_t)(mavsdk_action_result_t result, float altitude, void* user_data);

// ===== Action Creation/Destruction =====
CMAVSDK_EXPORT mavsdk_action_t mavsdk_action_create(mavsdk_system_t system);
CMAVSDK_EXPORT void mavsdk_action_destroy(mavsdk_action_t action);

// ===== Basic Actions (Async) =====
CMAVSDK_EXPORT void mavsdk_action_arm_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_arm_force_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_disarm_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_takeoff_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_land_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_reboot_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_shutdown_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_terminate_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_kill_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_return_to_launch_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_hold_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_transition_to_fixedwing_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);
CMAVSDK_EXPORT void mavsdk_action_transition_to_multicopter_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data);

// ===== Basic Actions (Sync) =====
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_arm(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_arm_force(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_disarm(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_takeoff(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_land(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_reboot(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_shutdown(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_terminate(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_kill(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_return_to_launch(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_hold(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_transition_to_fixedwing(mavsdk_action_t action);
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_transition_to_multicopter(mavsdk_action_t action);

// ===== Advanced Actions (Async) =====
CMAVSDK_EXPORT void mavsdk_action_goto_location_async(
    mavsdk_action_t action,
    double latitude_deg,
    double longitude_deg,
    float absolute_altitude_m,
    float yaw_deg,
    mavsdk_action_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_action_do_orbit_async(
    mavsdk_action_t action,
    float radius_m,
    float velocity_ms,
    mavsdk_action_orbit_yaw_behavior_t yaw_behavior,
    double latitude_deg,
    double longitude_deg,
    double absolute_altitude_m,
    mavsdk_action_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_action_set_actuator_async(
    mavsdk_action_t action,
    int32_t index,
    float value,
    mavsdk_action_result_callback_t callback,
    void* user_data);

// ===== Advanced Actions (Sync) =====
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_goto_location(
    mavsdk_action_t action,
    double latitude_deg,
    double longitude_deg,
    float absolute_altitude_m,
    float yaw_deg);

CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_do_orbit(
    mavsdk_action_t action,
    float radius_m,
    float velocity_ms,
    mavsdk_action_orbit_yaw_behavior_t yaw_behavior,
    double latitude_deg,
    double longitude_deg,
    double absolute_altitude_m);

CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_set_actuator(
    mavsdk_action_t action,
    int32_t index,
    float value);

// ===== Configuration Getters (Async) =====
CMAVSDK_EXPORT void mavsdk_action_get_takeoff_altitude_async(
    mavsdk_action_t action,
    mavsdk_action_get_altitude_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_action_get_return_to_launch_altitude_async(
    mavsdk_action_t action,
    mavsdk_action_get_altitude_callback_t callback,
    void* user_data);

// ===== Configuration Getters (Sync) =====
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_get_takeoff_altitude(
    mavsdk_action_t action,
    float* altitude_out);

CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_get_return_to_launch_altitude(
    mavsdk_action_t action,
    float* altitude_out);

// ===== Configuration Setters (Async) =====
CMAVSDK_EXPORT void mavsdk_action_set_takeoff_altitude_async(
    mavsdk_action_t action,
    float altitude,
    mavsdk_action_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_action_set_return_to_launch_altitude_async(
    mavsdk_action_t action,
    float relative_altitude_m,
    mavsdk_action_result_callback_t callback,
    void* user_data);

CMAVSDK_EXPORT void mavsdk_action_set_current_speed_async(
    mavsdk_action_t action,
    float speed_m_s,
    mavsdk_action_result_callback_t callback,
    void* user_data);

// ===== Configuration Setters (Sync) =====
CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_set_takeoff_altitude(
    mavsdk_action_t action,
    float altitude);

CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_set_return_to_launch_altitude(
    mavsdk_action_t action,
    float relative_altitude_m);

CMAVSDK_EXPORT mavsdk_action_result_t mavsdk_action_set_current_speed(
    mavsdk_action_t action,
    float speed_m_s);

#ifdef __cplusplus
}
#endif

#endif // CMAVSDK_ACTION_H
