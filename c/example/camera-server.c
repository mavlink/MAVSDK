#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "cmavsdk/mavsdk.h"
#include "cmavsdk/plugins/camera_server/camera_server.h"

static void subscribe_camera_operation(mavsdk_camera_server_t camera_server);

// Sample for camera current status
static time_t start_video_time;
static bool is_recording_video = false;
static bool is_capture_in_progress = false;
static int32_t image_count = 0;

int main(int argc, char** argv)
{
    // Create MAVSDK instance as Camera
    mavsdk_configuration_t configuration = 
        mavsdk_configuration_create_with_component_type(MAVSDK_COMPONENT_TYPE_CAMERA);
    mavsdk_t mavsdk = mavsdk_create(configuration);

    // 14030 is the default camera port for PX4 SITL
    mavsdk_connection_result_t result = mavsdk_add_any_connection(mavsdk, "udpin://0.0.0.0:14030");
    if (result != MAVSDK_CONNECTION_RESULT_SUCCESS) {
        printf("Could not establish connection: %d\n", result);
        return 1;
    }
    printf("Created camera server connection\n");

    mavsdk_server_component_t server_component = mavsdk_server_component(mavsdk, 0);
    mavsdk_camera_server_t camera_server = mavsdk_camera_server_create(server_component);

    if (!camera_server) {
        printf("Failed to create camera server\n");
        mavsdk_destroy(mavsdk);
        return 1;
    }

    // First add all subscriptions. This defines the camera capabilities.
    subscribe_camera_operation(camera_server);

    // Set initial state
    mavsdk_camera_server_set_in_progress(camera_server, false);

    // Set camera information to "activate" the camera plugin
    mavsdk_camera_server_information_t information = {0};
    information.vendor_name = "MAVSDK";
    information.model_name = "Example Camera Server";
    information.firmware_version = "1.0.0";
    information.focal_length_mm = 3.0f;
    information.horizontal_sensor_size_mm = 3.68f;
    information.vertical_sensor_size_mm = 2.76f;
    information.horizontal_resolution_px = 3280;
    information.vertical_resolution_px = 2464;
    information.lens_id = 0;
    information.definition_file_version = 0;
    information.definition_file_uri = "";
    information.image_in_video_mode_supported = false;
    information.video_in_image_mode_supported = false;

    mavsdk_camera_server_result_t set_info_result = 
        mavsdk_camera_server_set_information(camera_server, information);

    if (set_info_result != MAVSDK_CAMERA_SERVER_RESULT_SUCCESS) {
        printf("Failed to set camera info, exiting\n");
        mavsdk_camera_server_destroy(camera_server);
        mavsdk_destroy(mavsdk);
        return 2;
    }

    printf("Ready for clients\n");
    // Works as a server and never quits
    while (true) {
        sleep(1);
    }

    return 0;
}

static void take_photo_callback(int32_t index, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    mavsdk_camera_server_set_in_progress(camera_server, true);
    is_capture_in_progress = true;

    printf("Taking a picture (%d)...\n", index);

    // Simulate capture with delay
    usleep(500000); // 500ms

    // Populate capture info
    mavsdk_camera_server_capture_info_t capture_info = {0};
    capture_info.position.latitude_deg = 0.0;
    capture_info.position.longitude_deg = 0.0;
    capture_info.position.absolute_altitude_m = 0.0f;
    capture_info.position.relative_altitude_m = 0.0f;

    capture_info.attitude_quaternion.w = 1.0f;
    capture_info.attitude_quaternion.x = 0.0f;
    capture_info.attitude_quaternion.y = 0.0f;
    capture_info.attitude_quaternion.z = 0.0f;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    capture_info.time_utc_us = (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
    capture_info.is_success = true;
    capture_info.index = index;
    capture_info.file_url = "";

    image_count++;
    mavsdk_camera_server_set_in_progress(camera_server, false);

    mavsdk_camera_server_respond_take_photo(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK, 
        capture_info);

    is_capture_in_progress = false;
}

static void start_video_callback(int32_t stream_id, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    printf("Start video record\n");
    is_recording_video = true;
    start_video_time = time(NULL);

    mavsdk_camera_server_respond_start_video(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK);
}

static void stop_video_callback(int32_t stream_id, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    printf("Stop video record\n");
    is_recording_video = false;

    mavsdk_camera_server_respond_stop_video(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK);
}

static void start_video_streaming_callback(int32_t stream_id, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    printf("Start video streaming %d\n", stream_id);

    mavsdk_camera_server_respond_start_video_streaming(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK);
}

static void stop_video_streaming_callback(int32_t stream_id, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    printf("Stop video streaming %d\n", stream_id);

    mavsdk_camera_server_respond_stop_video_streaming(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK);
}

static void set_mode_callback(mavsdk_camera_server_mode_t mode, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    printf("Set camera mode %d\n", mode);

    mavsdk_camera_server_respond_set_mode(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK);
}

static void storage_information_callback(int32_t storage_id, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    mavsdk_camera_server_storage_information_t storage_information = {0};
    const int kTotalStorage = 4 * 1024 * 1024;
    storage_information.total_storage_mib = kTotalStorage;
    storage_information.used_storage_mib = 100.0f;
    storage_information.available_storage_mib = 
        kTotalStorage - storage_information.used_storage_mib;
    storage_information.storage_status = 
        MAVSDK_CAMERA_SERVER_STORAGE_INFORMATION_STORAGE_STATUS_FORMATTED;
    storage_information.storage_type = 
        MAVSDK_CAMERA_SERVER_STORAGE_INFORMATION_STORAGE_TYPE_MICROSD;

    mavsdk_camera_server_respond_storage_information(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK, 
        storage_information);
}

static void capture_status_callback(int32_t reserved, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    mavsdk_camera_server_capture_status_t capture_status = {0};
    capture_status.image_count = image_count;
    capture_status.image_status = is_capture_in_progress ?
        MAVSDK_CAMERA_SERVER_CAPTURE_STATUS_IMAGE_STATUS_CAPTURE_IN_PROGRESS :
        MAVSDK_CAMERA_SERVER_CAPTURE_STATUS_IMAGE_STATUS_IDLE;
    capture_status.video_status = is_recording_video ?
        MAVSDK_CAMERA_SERVER_CAPTURE_STATUS_VIDEO_STATUS_CAPTURE_IN_PROGRESS :
        MAVSDK_CAMERA_SERVER_CAPTURE_STATUS_VIDEO_STATUS_IDLE;

    if (is_recording_video) {
        time_t current_time = time(NULL);
        capture_status.recording_time_s = (float)difftime(current_time, start_video_time);
    }

    mavsdk_camera_server_respond_capture_status(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK, 
        capture_status);
}

static void format_storage_callback(int32_t storage_id, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    printf("Format storage with id: %d\n", storage_id);

    mavsdk_camera_server_respond_format_storage(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK);
}

static void reset_settings_callback(int32_t camera_id, void* user_data) {
    mavsdk_camera_server_t camera_server = (mavsdk_camera_server_t)user_data;

    printf("Reset camera settings\n");

    mavsdk_camera_server_respond_reset_settings(
        camera_server, 
        MAVSDK_CAMERA_SERVER_CAMERA_FEEDBACK_OK);
}

static void subscribe_camera_operation(mavsdk_camera_server_t camera_server)
{
    mavsdk_camera_server_subscribe_take_photo(
        camera_server, take_photo_callback, camera_server);

    mavsdk_camera_server_subscribe_start_video(
        camera_server, start_video_callback, camera_server);

    mavsdk_camera_server_subscribe_stop_video(
        camera_server, stop_video_callback, camera_server);

    mavsdk_camera_server_subscribe_start_video_streaming(
        camera_server, start_video_streaming_callback, camera_server);

    mavsdk_camera_server_subscribe_stop_video_streaming(
        camera_server, stop_video_streaming_callback, camera_server);

    mavsdk_camera_server_subscribe_set_mode(
        camera_server, set_mode_callback, camera_server);

    mavsdk_camera_server_subscribe_storage_information(
        camera_server, storage_information_callback, camera_server);

    mavsdk_camera_server_subscribe_capture_status(
        camera_server, capture_status_callback, camera_server);

    mavsdk_camera_server_subscribe_format_storage(
        camera_server, format_storage_callback, camera_server);

    mavsdk_camera_server_subscribe_reset_settings(
        camera_server, reset_settings_callback, camera_server);
}
