#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cmavsdk/mavsdk.h"
#include "cmavsdk/plugins/calibration/calibration.h"

typedef struct {
    bool completed;
} CalibrationState;

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

static void calibration_callback(
    const mavsdk_calibration_result_t result,
    const mavsdk_calibration_progress_data_t progress_data,
    void* user_data)
{
    CalibrationState* state = (CalibrationState*)user_data;
    
    switch (result) {
        case MAVSDK_CALIBRATION_RESULT_SUCCESS:
            printf("--- Calibration succeeded!\n");
            state->completed = true;
            break;
            
        case MAVSDK_CALIBRATION_RESULT_NEXT:
            if (progress_data.has_progress) {
                printf("    Progress: %.1f%%\n", progress_data.progress);
            }
            if (progress_data.has_status_text) {
                printf("    Instruction: %s\n", progress_data.status_text);
            }
            break;
            
        case MAVSDK_CALIBRATION_RESULT_FAILED:
            printf("--- Calibration failed!\n");
            state->completed = true;
            break;
            
        case MAVSDK_CALIBRATION_RESULT_NO_SYSTEM:
            printf("--- Error: No system connected\n");
            state->completed = true;
            break;
            
        case MAVSDK_CALIBRATION_RESULT_CONNECTION_ERROR:
            printf("--- Error: Connection error\n");
            state->completed = true;
            break;
            
        case MAVSDK_CALIBRATION_RESULT_BUSY:
            printf("--- Error: Vehicle is busy\n");
            state->completed = true;
            break;
            
        case MAVSDK_CALIBRATION_RESULT_COMMAND_DENIED:
            printf("--- Error: Command denied\n");
            state->completed = true;
            break;
            
        case MAVSDK_CALIBRATION_RESULT_TIMEOUT:
            printf("--- Error: Command timed out\n");
            state->completed = true;
            break;
            
        case MAVSDK_CALIBRATION_RESULT_CANCELLED:
            printf("--- Calibration cancelled\n");
            state->completed = true;
            break;
            
        case MAVSDK_CALIBRATION_RESULT_FAILED_ARMED:
            printf("--- Error: Cannot calibrate while armed\n");
            state->completed = true;
            break;
            
        case MAVSDK_CALIBRATION_RESULT_UNSUPPORTED:
            printf("--- Error: Calibration not supported\n");
            state->completed = true;
            break;
            
        default:
            printf("--- Unknown calibration result\n");
            state->completed = true;
            break;
    }
}

static void calibrate_accelerometer(mavsdk_calibration_t calibration)
{
    printf("Calibrating accelerometer...\n");
    
    CalibrationState state = {.completed = false};
    
    mavsdk_calibration_calibrate_accelerometer_async(
        calibration,
        calibration_callback,
        &state);
    
    // Wait for calibration to complete
    while (!state.completed) {
        sleep(1);
    }
}

static void calibrate_gyro(mavsdk_calibration_t calibration)
{
    printf("Calibrating gyro...\n");
    
    CalibrationState state = {.completed = false};
    
    mavsdk_calibration_calibrate_gyro_async(
        calibration,
        calibration_callback,
        &state);
    
    // Wait for calibration to complete
    while (!state.completed) {
        sleep(1);
    }
}

static void calibrate_magnetometer(mavsdk_calibration_t calibration)
{
    printf("Calibrating magnetometer...\n");
    
    CalibrationState state = {.completed = false};
    
    mavsdk_calibration_calibrate_magnetometer_async(
        calibration,
        calibration_callback,
        &state);
    
    // Wait for calibration to complete
    while (!state.completed) {
        sleep(1);
    }
}

int main()
{
    mavsdk_log_subscribe(my_log_callback, NULL);
    
    // Create MAVSDK instance
    mavsdk_configuration_t configuration =
        mavsdk_configuration_create_with_component_type(MAVSDK_COMPONENT_TYPE_GROUND_STATION);
    mavsdk_t mavsdk = mavsdk_create(configuration);

    // Add connection
    mavsdk_connection_result_t connection_result = 
    mavsdk_add_any_connection(mavsdk, "udpin://0.0.0.0:14540");
    
    if (connection_result != MAVSDK_CONNECTION_RESULT_SUCCESS) {
        fprintf(stderr, "Connection failed: %d\n", connection_result);
        mavsdk_destroy(mavsdk);
        return 1;
    }
    
    // Wait for autopilot
    printf("Waiting for autopilot...\n");
    mavsdk_system_t system = mavsdk_first_autopilot(mavsdk, 3);
    
    if (!system) {
        fprintf(stderr, "Timed out waiting for system\n");
        mavsdk_destroy(mavsdk);
        return 1;
    }
    
    // Create calibration plugin
    mavsdk_calibration_t calibration = mavsdk_calibration_create(system);
    
    if (!calibration) {
        fprintf(stderr, "Failed to create calibration plugin\n");
        mavsdk_destroy(mavsdk);
        return 1;
    }
    
    // Run calibrations
    calibrate_accelerometer(calibration);
    calibrate_gyro(calibration);
    calibrate_magnetometer(calibration);
    
    // Cleanup
    mavsdk_calibration_destroy(calibration);
    mavsdk_destroy(mavsdk);
    
    return 0;
}
