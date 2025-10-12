#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "cmavsdk/mavsdk.h"
#include "cmavsdk/plugins/camera/camera.h"

static void do_camera_operation(int32_t component_id, mavsdk_camera_t camera);

static void storage_callback(const mavsdk_camera_storage_update_t update, void* user_data) {
    printf("Camera storage update:\n");
    printf("  Component ID: %d\n", update.storage.component_id);
    printf("  Storage ID: %d\n", update.storage.storage_id);
    printf("  Total storage: %.2f MiB\n", update.storage.total_storage_mib);
    printf("  Used storage: %.2f MiB\n", update.storage.used_storage_mib);
    printf("  Available storage: %.2f MiB\n", update.storage.available_storage_mib);
}

int main(int argc, const char* argv[])
{
    // Create MAVSDK instance as Ground Station
    mavsdk_configuration_t configuration = 
        mavsdk_configuration_create_with_component_type(MAVSDK_COMPONENT_TYPE_GROUND_STATION);
    mavsdk_t mavsdk = mavsdk_create(configuration);

    mavsdk_connection_result_t result = mavsdk_add_any_connection(mavsdk, "udpout://127.0.0.1:14030");
    if (result == MAVSDK_CONNECTION_RESULT_SUCCESS) {
        printf("Connected!\n");
    }

    printf("Waiting for camera system...\n");
    mavsdk_system_t* camera_servers;
    size_t camera_servers_size = 0;

    do {
        camera_servers = mavsdk_get_systems(mavsdk, &camera_servers_size);
    } while (camera_servers == NULL);

    mavsdk_system_t camera_server = camera_servers[0];

    mavsdk_camera_t camera_plugin = mavsdk_camera_create(camera_server);
    if (!camera_plugin) {
        printf("Failed to create camera plugin\n");
        mavsdk_destroy(mavsdk);
        return -1;
    }

    printf("Waiting for cameras...\n");
    mavsdk_camera_camera_list_t camera_list;
    do {
        mavsdk_camera_camera_list(camera_plugin, &camera_list);
        sleep(1);
    } while (camera_list.cameras_size == 0);

    printf("Cameras found:\n");
    for (size_t i = 0; i < camera_list.cameras_size; i++) {
        printf("Camera component ID: %d, model: %s, vendor: %s\n",
               camera_list.cameras[i].component_id,
               camera_list.cameras[i].model_name,
               camera_list.cameras[i].vendor_name);
    }

    if (camera_list.cameras_size > 1) {
        printf("More than one camera found, using first one discovered.\n");
    }

    int32_t component_id = camera_list.cameras[0].component_id;

    // Clean up camera list
    mavsdk_camera_camera_list_destroy(&camera_list);

    // Subscribe to storage updates
    mavsdk_camera_storage_handle_t storage_handle = 
        mavsdk_camera_subscribe_storage(camera_plugin, storage_callback, NULL);

    // Perform camera operations
    do_camera_operation(component_id, camera_plugin);

    // Keep running to receive updates
    while (true) {
        sleep(1);
    }

    // Cleanup (unreachable in this example)
    mavsdk_camera_unsubscribe_storage(camera_plugin, storage_handle);
    mavsdk_camera_destroy(camera_plugin);
    mavsdk_destroy(mavsdk);

    return 0;
}

void do_camera_operation(int32_t component_id, mavsdk_camera_t camera_plugin)
{
    mavsdk_camera_result_t result;

    // Switch to photo mode to take photo
    result = mavsdk_camera_set_mode(camera_plugin, component_id, MAVSDK_CAMERA_MODE_PHOTO);
    printf("Set camera to photo mode result: %d\n", result);

    result = mavsdk_camera_take_photo(camera_plugin, component_id);
    printf("Take photo result: %d\n", result);

    // Switch to video mode to record video
    result = mavsdk_camera_set_mode(camera_plugin, component_id, MAVSDK_CAMERA_MODE_VIDEO);
    printf("Set camera to video mode result: %d\n", result);

    result = mavsdk_camera_start_video(camera_plugin, component_id);
    printf("Start video result: %d\n", result);

    result = mavsdk_camera_stop_video(camera_plugin, component_id);
    printf("Stop video result: %d\n", result);

    // The camera can have multi video stream so may need the stream id
    result = mavsdk_camera_start_video_streaming(camera_plugin, component_id, 0);
    printf("Start video streaming result: %d\n", result);

    result = mavsdk_camera_stop_video_streaming(camera_plugin, component_id, 0);
    printf("Stop video streaming result: %d\n", result);

    // Format the storage with special storage id test
    result = mavsdk_camera_format_storage(camera_plugin, component_id, 0);
    printf("Format storage result: %d\n", result);

    result = mavsdk_camera_reset_settings(camera_plugin, component_id);
    printf("Reset camera settings result: %d\n", result);
}
