#include "mavsdk.h"
#include <stdio.h>

// C log callback function
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

int main() {
    // Set custom log callback
    mavsdk_log_subscribe(my_log_callback, NULL);
    
    // Now when using MAVSDK, the custom logger will be called
    mavsdk_configuration_t configuration = mavsdk_configuration_create_with_component_type(MAVSDK_COMPONENT_TYPE_GROUND_STATION);
    mavsdk_t mavsdk = mavsdk_create(configuration);
    mavsdk_add_any_connection(mavsdk, "udpin://0.0.0.0:14540");

    printf("Waiting for autopilot\n");
    mavsdk_system_t drone = mavsdk_first_autopilot(mavsdk, 30);
    // Do more stuff here
    
    // Cleanup
    mavsdk_log_unsubscribe();
    mavsdk_destroy(mavsdk);
    
    return 0;
}
