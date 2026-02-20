#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "cmavsdk/mavsdk.h"
#include "cmavsdk/plugins/action/action.h"
#include "cmavsdk/plugins/telemetry/telemetry.h"

static bool my_log_callback(mavsdk_log_level_t level, const char* message, const char* file, int line, void* user_data) {
    const char* level_str = "UNKNOWN";
    switch (level) {
        case MAVSDK_LOG_LEVEL_DEBUG: level_str = "DEBUG"; break;
        case MAVSDK_LOG_LEVEL_INFO: level_str = "INFO"; break;
        case MAVSDK_LOG_LEVEL_WARN: level_str = "WARN"; break;
        case MAVSDK_LOG_LEVEL_ERROR: level_str = "ERROR"; break;
    }
    
    if (file) {
        printf("[%s] %s:%d - %s\n", level_str, file, line, message);
    } else {
        printf("[%s] %s\n", level_str, message);
    }
    
    // Return true to prevent default logging, false to keep both
    return true;
}

static void position_callback(const mavsdk_telemetry_position_t position, void* user_data) {
    (void)user_data; // Unused parameter
    
    printf("Altitude: %.1f m\n", position.absolute_altitude_m);
}

int main() {
    mavsdk_log_subscribe(my_log_callback, NULL);
    
    mavsdk_configuration_t configuration = mavsdk_configuration_create_with_component_type(MAVSDK_COMPONENT_TYPE_GROUND_STATION);
    mavsdk_t mavsdk = mavsdk_create(configuration);
    mavsdk_add_any_connection(mavsdk, "udpin://0.0.0.0:14540");

    printf("Waiting for autopilot\n");
    mavsdk_system_t drone = mavsdk_first_autopilot(mavsdk, 30);

    printf("Subscribe to some telemetry\n");
    mavsdk_telemetry_t telemetry = mavsdk_telemetry_create(drone);
    if (!telemetry) {
        printf("Failed to create telemetry plugin\n");
        mavsdk_destroy(mavsdk);
        return 1;
    }

    mavsdk_telemetry_position_handle_t position_handle = 
        mavsdk_telemetry_subscribe_position(telemetry, position_callback, NULL);
    
    if (!position_handle) {
        printf("Failed to subscribe to position updates\n");
        mavsdk_telemetry_destroy(telemetry);
        mavsdk_destroy(mavsdk);
        return 1;
    }

    mavsdk_telemetry_set_rate_position(telemetry, 1.0);

    printf("Perform some actions\n");
    mavsdk_action_t action = mavsdk_action_create(drone);
    if (!action) {
        printf("Failed to create action plugin\n");
        mavsdk_destroy(mavsdk);
        return 1;
    }
    printf("Arming...\n");
    mavsdk_action_arm(action);
    printf("Taking off...\n");
    mavsdk_action_takeoff(action);

    sleep(5);

    printf("Unsubscribing from position updates...\n");
    mavsdk_telemetry_unsubscribe_position(telemetry, position_handle);

    sleep(5);

    printf("Subscribing again...\n");
    mavsdk_telemetry_subscribe_position(telemetry, position_callback, NULL);

    sleep(5);

    mavsdk_action_land(action);

    sleep(15);
   
    // Cleanup
    mavsdk_log_unsubscribe();

    mavsdk_action_destroy(action);
    mavsdk_telemetry_destroy(telemetry);
    mavsdk_destroy(mavsdk);
    
    return 0;
}
