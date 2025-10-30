#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cmavsdk/mavsdk.h"
#include "cmavsdk/plugins/action/action.h"
#include "cmavsdk/plugins/mission/mission.h"
#include "cmavsdk/plugins/telemetry/telemetry.h"

mavsdk_mission_mission_item_t make_mission_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    mavsdk_mission_mission_item_camera_action_t camera_action)
{
    mavsdk_mission_mission_item_t item = {0};
    item.latitude_deg = latitude_deg;
    item.longitude_deg = longitude_deg;
    item.relative_altitude_m = relative_altitude_m;
    item.speed_m_s = speed_m_s;
    item.is_fly_through = is_fly_through;
    item.gimbal_pitch_deg = gimbal_pitch_deg;
    item.gimbal_yaw_deg = gimbal_yaw_deg;
    item.camera_action = camera_action;
    item.loiter_time_s = 0.0f;
    item.camera_photo_interval_s = 1.0;
    item.acceptance_radius_m = 1.0f;
    item.yaw_deg = 0.0f;
    item.camera_photo_distance_m = 0.0f;
    item.vehicle_action = MAVSDK_MISSION_MISSION_ITEM_VEHICLE_ACTION_NONE;
    return item;
}

void usage(const char* bin_name)
{
    fprintf(stderr, "Usage : %s <connection_url>\n", bin_name);
    fprintf(stderr, "Connection URL format should be :\n");
    fprintf(stderr, " For TCP server: tcpin://<our_ip>:<port>\n");
    fprintf(stderr, " For TCP client: tcpout://<remote_ip>:<port>\n");
    fprintf(stderr, " For UDP server: udpin://<our_ip>:<port>\n");
    fprintf(stderr, " For UDP client: udpout://<remote_ip>:<port>\n");
    fprintf(stderr, " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n");
    fprintf(stderr, "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n");
}

static volatile bool want_to_pause = false;

static void mission_progress_callback(
    const mavsdk_mission_mission_progress_t mission_progress,
    void* user_data)
{
    (void)user_data; // Unused
    
    printf("Mission status update: %d / %d\n",
           mission_progress.current,
           mission_progress.total);
    
    if (mission_progress.current >= 2) {
        want_to_pause = true;
    }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }
    
    // Create MAVSDK instance
    mavsdk_configuration_t config = 
        mavsdk_configuration_create_with_component_type(MAVSDK_COMPONENT_TYPE_GROUND_STATION);
    mavsdk_t mavsdk = mavsdk_create(config);
    
    // Add connection
    mavsdk_connection_result_t connection_result = 
        mavsdk_add_any_connection(mavsdk, argv[1]);
    
    if (connection_result != MAVSDK_CONNECTION_RESULT_SUCCESS) {
        fprintf(stderr, "Connection failed: %d\n", connection_result);
        mavsdk_destroy(mavsdk);
        return 1;
    }
    
    printf("Waiting for system...\n");
    mavsdk_system_t system = mavsdk_first_autopilot(mavsdk, 3.0);
    if (!system) {
        fprintf(stderr, "Timed out waiting for system\n");
        mavsdk_destroy(mavsdk);
        return 1;
    }
    
    // Create plugins
    mavsdk_action_t action = mavsdk_action_create(system);
    mavsdk_mission_t mission = mavsdk_mission_create(system);
    mavsdk_telemetry_t telemetry = mavsdk_telemetry_create(system);
    
    if (!action || !mission || !telemetry) {
        fprintf(stderr, "Failed to create plugins\n");
        if (action) mavsdk_action_destroy(action);
        if (mission) mavsdk_mission_destroy(mission);
        if (telemetry) mavsdk_telemetry_destroy(telemetry);
        mavsdk_destroy(mavsdk);
        return 1;
    }
    
    // Wait for system to be ready
    bool health_ok = false;
    while (!health_ok) {
        printf("Waiting for system to be ready\n");
        sleep(1);
        mavsdk_telemetry_health_t health;
        mavsdk_telemetry_health(telemetry, &health);
        health_ok = health.is_gyrometer_calibration_ok &&
                    health.is_accelerometer_calibration_ok &&
                    health.is_magnetometer_calibration_ok &&
                    health.is_local_position_ok &&
                    health.is_global_position_ok &&
                    health.is_home_position_ok;
    }
    
    printf("System ready\n");
    printf("Creating and uploading mission\n");
    
    // Create mission items array
    mavsdk_mission_mission_item_t* mission_items = 
        malloc(6 * sizeof(mavsdk_mission_mission_item_t));
    
    mission_items[0] = make_mission_item(
        47.398170327054473, 8.5456490218639658, 10.0f, 5.0f, false,
        20.0f, 60.0f, MAVSDK_MISSION_MISSION_ITEM_CAMERA_ACTION_NONE);
    
    mission_items[1] = make_mission_item(
        47.398241338125118, 8.5455360114574432, 10.0f, 2.0f, true,
        0.0f, -60.0f, MAVSDK_MISSION_MISSION_ITEM_CAMERA_ACTION_TAKE_PHOTO);
    
    mission_items[2] = make_mission_item(
        47.398139363821485, 8.5453846156597137, 10.0f, 5.0f, true,
        -45.0f, 0.0f, MAVSDK_MISSION_MISSION_ITEM_CAMERA_ACTION_START_VIDEO);
    
    mission_items[3] = make_mission_item(
        47.398058617228855, 8.5454618036746979, 10.0f, 2.0f, false,
        -90.0f, 30.0f, MAVSDK_MISSION_MISSION_ITEM_CAMERA_ACTION_STOP_VIDEO);
    
    mission_items[4] = make_mission_item(
        47.398100366082858, 8.5456969141960144, 10.0f, 5.0f, false,
        -45.0f, -30.0f, MAVSDK_MISSION_MISSION_ITEM_CAMERA_ACTION_START_PHOTO_INTERVAL);
    
    mission_items[5] = make_mission_item(
        47.398001890458097, 8.5455576181411743, 10.0f, 5.0f, false,
        0.0f, 0.0f, MAVSDK_MISSION_MISSION_ITEM_CAMERA_ACTION_STOP_PHOTO_INTERVAL);
    
    // Create mission plan
    mavsdk_mission_mission_plan_t mission_plan = {0};
    mission_plan.mission_items = mission_items;
    mission_plan.mission_items_size = 6;
    
    // Upload mission
    printf("Uploading mission...\n");
    mavsdk_mission_result_t upload_result = 
        mavsdk_mission_upload_mission(mission, mission_plan);
    
    if (upload_result != MAVSDK_MISSION_RESULT_SUCCESS) {
        fprintf(stderr, "Mission upload failed: %d, exiting.\n", upload_result);
        free(mission_items);
        mavsdk_mission_destroy(mission);
        mavsdk_action_destroy(action);
        mavsdk_telemetry_destroy(telemetry);
        mavsdk_destroy(mavsdk);
        return 1;
    }
    
    // Free the mission items array
    free(mission_items);
    
    // Arm
    printf("Arming...\n");
    mavsdk_action_result_t arm_result = mavsdk_action_arm(action);
    if (arm_result != MAVSDK_ACTION_RESULT_SUCCESS) {
        fprintf(stderr, "Arming failed: %d\n", arm_result);
        mavsdk_mission_destroy(mission);
        mavsdk_action_destroy(action);
        mavsdk_telemetry_destroy(telemetry);
        mavsdk_destroy(mavsdk);
        return 1;
    }
    printf("Armed.\n");
    
    // Subscribe to mission progress before starting
    mavsdk_mission_mission_progress_handle_t progress_handle = 
        mavsdk_mission_subscribe_mission_progress(
            mission, mission_progress_callback, NULL);
    
    // Start mission
    printf("Starting mission...\n");
    mavsdk_mission_result_t start_result = mavsdk_mission_start_mission(mission);
    if (start_result != MAVSDK_MISSION_RESULT_SUCCESS) {
        fprintf(stderr, "Starting mission failed: %d\n", start_result);
        mavsdk_mission_unsubscribe_mission_progress(mission, progress_handle);
        mavsdk_mission_destroy(mission);
        mavsdk_action_destroy(action);
        mavsdk_telemetry_destroy(telemetry);
        mavsdk_destroy(mavsdk);
        return 1;
    }
    
    // Wait for pause condition
    while (!want_to_pause) {
        sleep(1);
    }
    
    // Pause mission
    printf("Pausing mission...\n");
    mavsdk_mission_result_t pause_result = mavsdk_mission_pause_mission(mission);
    if (pause_result != MAVSDK_MISSION_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to pause mission: %d\n", pause_result);
    }
    printf("Mission paused.\n");
    
    // Pause for 5 seconds
    sleep(5);
    
    // Continue mission
    printf("Continuing mission...\n");
    mavsdk_mission_result_t continue_result = mavsdk_mission_start_mission(mission);
    if (continue_result != MAVSDK_MISSION_RESULT_SUCCESS) {
        fprintf(stderr, "Starting mission again failed: %d\n", continue_result);
        mavsdk_mission_unsubscribe_mission_progress(mission, progress_handle);
        mavsdk_mission_destroy(mission);
        mavsdk_action_destroy(action);
        mavsdk_telemetry_destroy(telemetry);
        mavsdk_destroy(mavsdk);
        return 1;
    }
    
    // Wait for mission to finish
    bool is_finished = false;
    while (!is_finished) {
        sleep(1);
        mavsdk_mission_is_mission_finished(mission, &is_finished);
    }
    
    // Return to launch
    printf("Commanding RTL...\n");
    mavsdk_action_result_t rtl_result = mavsdk_action_return_to_launch(action);
    if (rtl_result != MAVSDK_ACTION_RESULT_SUCCESS) {
        fprintf(stderr, "Failed to command RTL: %d\n", rtl_result);
        mavsdk_mission_unsubscribe_mission_progress(mission, progress_handle);
        mavsdk_mission_destroy(mission);
        mavsdk_action_destroy(action);
        mavsdk_telemetry_destroy(telemetry);
        mavsdk_destroy(mavsdk);
        return 1;
    }
    printf("Commanded RTL.\n");
    
    // Wait a bit for armed state to be correct
    sleep(2);
    
    // Wait until disarmed
    bool armed = true;
    while (armed) {
        sleep(1);
        mavsdk_telemetry_armed(telemetry, &armed);
    }
    
    printf("Disarmed, exiting.\n");
    
    // Cleanup
    mavsdk_mission_unsubscribe_mission_progress(mission, progress_handle);
    mavsdk_mission_destroy(mission);
    mavsdk_action_destroy(action);
    mavsdk_telemetry_destroy(telemetry);
    mavsdk_destroy(mavsdk);
    
    return 0;
}
