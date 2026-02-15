#ifndef CMAVSDK_SYSTEM_H
#define CMAVSDK_SYSTEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "mavsdk_export.h"
#include "autopilot.h"
#include "vehicle.h"
#include "component_type.h"

#ifdef __cplusplus
extern "C" {
#endif

// ===== Opaque Handles =====
typedef void* mavsdk_system_t;
typedef void* mavsdk_is_connected_handle_t;
typedef void* mavsdk_component_discovered_handle_t;
typedef void* mavsdk_component_discovered_id_handle_t;

// ===== Basic System Queries =====
CMAVSDK_EXPORT bool mavsdk_system_has_autopilot(mavsdk_system_t system);
CMAVSDK_EXPORT bool mavsdk_system_is_standalone(mavsdk_system_t system);
CMAVSDK_EXPORT bool mavsdk_system_has_camera(mavsdk_system_t system, int camera_id);
CMAVSDK_EXPORT bool mavsdk_system_has_gimbal(mavsdk_system_t system);
CMAVSDK_EXPORT bool mavsdk_system_is_connected(mavsdk_system_t system);
CMAVSDK_EXPORT uint8_t mavsdk_system_get_system_id(mavsdk_system_t system);

/**
 * @brief Get all component IDs.
 * @param system The system handle.
 * @param count Output: number of component IDs.
 * @return Pointer to array of component IDs (uint8_t). Must be freed with mavsdk_system_free_component_ids().
 */
CMAVSDK_EXPORT uint8_t* mavsdk_system_component_ids(mavsdk_system_t system, size_t* count);
CMAVSDK_EXPORT void mavsdk_system_free_component_ids(uint8_t* ids);

// ===== Subscriptions =====
typedef void (*mavsdk_is_connected_callback_t)(bool is_connected, void* user_data);
CMAVSDK_EXPORT mavsdk_is_connected_handle_t mavsdk_system_subscribe_is_connected(
    mavsdk_system_t system,
    mavsdk_is_connected_callback_t callback,
    void* user_data
);
CMAVSDK_EXPORT void mavsdk_system_unsubscribe_is_connected(
    mavsdk_system_t system,
    mavsdk_is_connected_handle_t handle
);

typedef void (*mavsdk_component_discovered_callback_t)(mavsdk_component_type_t component_type, void* user_data);
CMAVSDK_EXPORT mavsdk_component_discovered_handle_t mavsdk_system_subscribe_component_discovered(
    mavsdk_system_t system,
    mavsdk_component_discovered_callback_t callback,
    void* user_data
);
CMAVSDK_EXPORT void mavsdk_system_unsubscribe_component_discovered(
    mavsdk_system_t system,
    mavsdk_component_discovered_handle_t handle
);

typedef void (*mavsdk_component_discovered_id_callback_t)(mavsdk_component_type_t component_type, uint8_t component_id, void* user_data);
CMAVSDK_EXPORT mavsdk_component_discovered_id_handle_t mavsdk_system_subscribe_component_discovered_id(
    mavsdk_system_t system,
    mavsdk_component_discovered_id_callback_t callback,
    void* user_data
);
CMAVSDK_EXPORT void mavsdk_system_unsubscribe_component_discovered_id(
    mavsdk_system_t system,
    mavsdk_component_discovered_id_handle_t handle
);

// ===== Other System Functions =====
CMAVSDK_EXPORT void mavsdk_system_enable_timesync(mavsdk_system_t system);
CMAVSDK_EXPORT mavsdk_autopilot_t mavsdk_system_autopilot_type(mavsdk_system_t system);
CMAVSDK_EXPORT mavsdk_vehicle_t mavsdk_system_vehicle_type(mavsdk_system_t system);

#ifdef __cplusplus
}
#endif

#endif // CMAVSDK_SYSTEM_H
