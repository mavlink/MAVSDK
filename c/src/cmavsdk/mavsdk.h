#ifndef CMAVSDK_MAVSDK_H
#define CMAVSDK_MAVSDK_H

#include <stddef.h>
#include <stdint.h>
#include "log.h"
#include "mavsdk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mavsdk_s *mavsdk_t;
typedef struct mavsdk_configuration_s *mavsdk_configuration_t;
typedef struct mavsdk_system_s *mavsdk_system_t;
typedef struct mavsdk_server_component_s *mavsdk_server_component_t;
typedef struct mavsdk_connection_handle_s *mavsdk_connection_handle_t;
typedef struct mavsdk_connection_error_handle_s *mavsdk_connection_error_handle_t;
typedef struct mavsdk_new_system_handle_s *mavsdk_new_system_handle_t;
typedef struct mavsdk_intercept_json_handle_s *mavsdk_intercept_json_handle_t;

typedef enum {
    MAVSDK_COMPONENT_TYPE_AUTOPILOT = 0,
    MAVSDK_COMPONENT_TYPE_GROUND_STATION = 1,
    MAVSDK_COMPONENT_TYPE_COMPANION_COMPUTER = 2,
    MAVSDK_COMPONENT_TYPE_CAMERA = 3,
    MAVSDK_COMPONENT_TYPE_GIMBAL = 4,
    MAVSDK_COMPONENT_TYPE_REMOTE_ID = 5
} mavsdk_component_type_t;

typedef enum {
    MAVSDK_FORWARDING_OPTION_OFF = 0,
    MAVSDK_FORWARDING_OPTION_ON = 1
} mavsdk_forwarding_option_t;

typedef enum {
    MAVSDK_CONNECTION_RESULT_SUCCESS = 0,
    MAVSDK_CONNECTION_RESULT_TIMEOUT = 1,
    MAVSDK_CONNECTION_RESULT_SOCKET_ERROR = 2,
    MAVSDK_CONNECTION_RESULT_BIND_ERROR = 3,
    MAVSDK_CONNECTION_RESULT_SOCKET_CONNECTION_ERROR = 4,
    MAVSDK_CONNECTION_RESULT_CONNECTION_ERROR = 5,
    MAVSDK_CONNECTION_RESULT_NOT_IMPLEMENTED = 6,
    MAVSDK_CONNECTION_RESULT_SYSTEM_NOT_CONNECTED = 7,
    MAVSDK_CONNECTION_RESULT_SYSTEM_BUSY = 8,
    MAVSDK_CONNECTION_RESULT_COMMAND_DENIED = 9,
    MAVSDK_CONNECTION_RESULT_DESTINATION_IP_UNKNOWN = 10,
    MAVSDK_CONNECTION_RESULT_CONNECTIONS_EXHAUSTED = 11,
    MAVSDK_CONNECTION_RESULT_CONNECTION_URL_INVALID = 12,
    MAVSDK_CONNECTION_RESULT_BAUDRATE_UNKNOWN = 13
} mavsdk_connection_result_t;

typedef struct {
    char *error_description;
    mavsdk_connection_handle_t connection_handle;
} mavsdk_connection_error_t;

typedef struct {
    char *message_name;
    uint32_t system_id;
    uint32_t component_id;
    uint32_t target_system_id;
    uint32_t target_component_id;
    char *fields_json;
} mavsdk_message_t;

typedef struct {
    mavsdk_connection_result_t result;
    mavsdk_connection_handle_t handle;
} mavsdk_connection_result_with_handle_t;

// ===== Callback Typedefs =====
typedef void (*mavsdk_connection_error_callback_t)(const mavsdk_connection_error_t error, void *user_data);
typedef void (*mavsdk_new_system_callback_t)(void *user_data);
typedef int  (*mavsdk_intercept_json_callback_t)(const mavsdk_message_t message, void *user_data);

// ===== Configuration =====
CMAVSDK_EXPORT mavsdk_configuration_t mavsdk_configuration_create_with_component_type(mavsdk_component_type_t type);
CMAVSDK_EXPORT mavsdk_configuration_t mavsdk_configuration_create_manual(uint8_t system_id, uint8_t component_id, int always_send_heartbeats);
CMAVSDK_EXPORT void mavsdk_configuration_destroy(mavsdk_configuration_t config);
CMAVSDK_EXPORT uint8_t mavsdk_configuration_get_system_id(mavsdk_configuration_t config);
CMAVSDK_EXPORT void mavsdk_configuration_set_system_id(mavsdk_configuration_t config, uint8_t system_id);
CMAVSDK_EXPORT uint8_t mavsdk_configuration_get_component_id(mavsdk_configuration_t config);
CMAVSDK_EXPORT void mavsdk_configuration_set_component_id(mavsdk_configuration_t config, uint8_t component_id);
CMAVSDK_EXPORT int mavsdk_configuration_get_always_send_heartbeats(mavsdk_configuration_t config);
CMAVSDK_EXPORT void mavsdk_configuration_set_always_send_heartbeats(mavsdk_configuration_t config, int always_send_heartbeats);
CMAVSDK_EXPORT mavsdk_component_type_t mavsdk_configuration_get_component_type(mavsdk_configuration_t config);
CMAVSDK_EXPORT void mavsdk_configuration_set_component_type(mavsdk_configuration_t config, mavsdk_component_type_t component_type);
CMAVSDK_EXPORT uint8_t mavsdk_configuration_get_mav_type(mavsdk_configuration_t config);
CMAVSDK_EXPORT void mavsdk_configuration_set_mav_type(mavsdk_configuration_t config, uint8_t mav_type);

// ===== Core =====
CMAVSDK_EXPORT mavsdk_t mavsdk_create(mavsdk_configuration_t config);
CMAVSDK_EXPORT void mavsdk_destroy(mavsdk_t mavsdk);
CMAVSDK_EXPORT const char* mavsdk_version(mavsdk_t mavsdk);

// ===== Connection Management =====
CMAVSDK_EXPORT mavsdk_connection_result_t mavsdk_add_any_connection(
    mavsdk_t mavsdk,
    const char *connection_url
);
CMAVSDK_EXPORT mavsdk_connection_result_t mavsdk_add_any_connection_with_forwarding(
    mavsdk_t mavsdk,
    const char *connection_url,
    mavsdk_forwarding_option_t forwarding_option
);
CMAVSDK_EXPORT mavsdk_connection_result_with_handle_t mavsdk_add_any_connection_with_handle(
    mavsdk_t mavsdk,
    const char *connection_url
);
CMAVSDK_EXPORT mavsdk_connection_result_with_handle_t mavsdk_add_any_connection_with_handle_and_forwarding(
    mavsdk_t mavsdk,
    const char *connection_url,
    mavsdk_forwarding_option_t forwarding_option
);
CMAVSDK_EXPORT void mavsdk_remove_connection(
    mavsdk_t mavsdk,
    mavsdk_connection_handle_t handle
);

// ===== Connection Error Handling =====
CMAVSDK_EXPORT mavsdk_connection_error_handle_t mavsdk_subscribe_connection_errors(
    mavsdk_t mavsdk,
    mavsdk_connection_error_callback_t callback,
    void *user_data
);
CMAVSDK_EXPORT void mavsdk_unsubscribe_connection_errors(
    mavsdk_t mavsdk,
    mavsdk_connection_error_handle_t handle
);

// ===== System Discovery and Management =====
CMAVSDK_EXPORT size_t mavsdk_system_count(mavsdk_t mavsdk);
CMAVSDK_EXPORT mavsdk_system_t* mavsdk_get_systems(
    mavsdk_t mavsdk,
    size_t *count
);
CMAVSDK_EXPORT void mavsdk_free_systems_array(mavsdk_system_t *systems);
CMAVSDK_EXPORT mavsdk_system_t mavsdk_first_autopilot(
    mavsdk_t mavsdk,
    double timeout_s
);

// ===== System Event Subscriptions =====
CMAVSDK_EXPORT mavsdk_new_system_handle_t mavsdk_subscribe_on_new_system(
    mavsdk_t mavsdk,
    mavsdk_new_system_callback_t callback,
    void *user_data
);
CMAVSDK_EXPORT void mavsdk_unsubscribe_on_new_system(
    mavsdk_t mavsdk,
    mavsdk_new_system_handle_t handle
);

// ===== Configuration and Timeout =====
CMAVSDK_EXPORT void mavsdk_set_configuration(
    mavsdk_t mavsdk,
    mavsdk_configuration_t configuration
);
CMAVSDK_EXPORT void mavsdk_set_timeout_s(
    mavsdk_t mavsdk,
    double timeout_s
);

// ===== Server Components =====
CMAVSDK_EXPORT mavsdk_server_component_t mavsdk_server_component(
    mavsdk_t mavsdk,
    unsigned int instance
);
CMAVSDK_EXPORT mavsdk_server_component_t mavsdk_server_component_by_type(
    mavsdk_t mavsdk,
    mavsdk_component_type_t component_type,
    unsigned int instance
);
CMAVSDK_EXPORT mavsdk_server_component_t mavsdk_server_component_by_id(
    mavsdk_t mavsdk,
    uint8_t component_id
);

// ===== Message Interception (JSON) =====
CMAVSDK_EXPORT mavsdk_intercept_json_handle_t mavsdk_subscribe_incoming_messages_json(
    mavsdk_t mavsdk,
    mavsdk_intercept_json_callback_t callback,
    void *user_data
);
CMAVSDK_EXPORT void mavsdk_unsubscribe_incoming_messages_json(
    mavsdk_t mavsdk,
    mavsdk_intercept_json_handle_t handle
);
CMAVSDK_EXPORT mavsdk_intercept_json_handle_t mavsdk_subscribe_outgoing_messages_json(
    mavsdk_t mavsdk,
    mavsdk_intercept_json_callback_t callback,
    void *user_data
);
CMAVSDK_EXPORT void mavsdk_unsubscribe_outgoing_messages_json(
    mavsdk_t mavsdk,
    mavsdk_intercept_json_handle_t handle
);

// ===== Memory Management Helpers =====
CMAVSDK_EXPORT void mavsdk_free_connection_error(
    mavsdk_connection_error_t *error
);
CMAVSDK_EXPORT void mavsdk_free_mavsdk_message(
    mavsdk_message_t *message
);

#ifdef __cplusplus
}
#endif
#endif // CMAVSDK_MAVSDK_H
