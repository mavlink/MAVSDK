import time
from pymavsdk import *
from pymavsdk.plugins.camera import *

def storage_callback(update, user_data=None):
    """Callback for camera storage updates"""
    print("Camera storage update:")
    print(f"  Component ID: {update.storage.component_id}")
    print(f"  Storage ID: {update.storage.storage_id}")
    print(f"  Total storage: {update.storage.total_storage_mib:.2f} MiB")
    print(f"  Used storage: {update.storage.used_storage_mib:.2f} MiB")
    print(f"  Available storage: {update.storage.available_storage_mib:.2f} MiB")
    print(f"  Storage status: {update.storage.storage_status}")
    print(f"  Storage type: {update.storage.storage_type}")

def do_camera_operation(component_id, camera_plugin):
    """Perform various camera operations"""
    # Switch to photo mode to take photo
    result = camera_plugin.set_mode(component_id, Mode.PHOTO)
    print(f"Set camera to photo mode result: {result}")

    result = camera_plugin.take_photo(component_id)
    print(f"Take photo result: {result}")

    # Switch to video mode to record video
    result = camera_plugin.set_mode(component_id, Mode.VIDEO)
    print(f"Set camera to video mode result: {result}")

    result = camera_plugin.start_video(component_id)
    print(f"Start video result: {result}")

    result = camera_plugin.stop_video(component_id)
    print(f"Stop video result: {result}")

    # The camera can have multi video stream so may need the stream id
    result = camera_plugin.start_video_streaming(component_id, 0)
    print(f"Start video streaming result: {result}")

    result = camera_plugin.stop_video_streaming(component_id, 0)
    print(f"Stop video streaming result: {result}")

    # Format the storage with special storage id test
    result = camera_plugin.format_storage(component_id, 0)
    print(f"Format storage result: {result}")

    result = camera_plugin.reset_settings(component_id)
    print(f"Reset camera settings result: {result}")

def main():
    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)

    # Add connection
    result = mavsdk.add_any_connection("udpout://127.0.0.1:14030")
    if result == ConnectionResult.SUCCESS:
        print("Connected!")

    print("Waiting for camera system...")
    
    # Wait for camera system
    camera_server = None
    while camera_server is None:
        systems = mavsdk.get_systems()
        if len(systems) > 0:
            camera_server = systems[0]
        else:
            time.sleep(1)

    # Create camera plugin
    camera_plugin = Camera(camera_server)
    if not camera_plugin:
        print("Failed to create camera plugin")
        return -1

    print("Waiting for cameras...")
    
    # Wait for cameras to be discovered
    camera_list = None
    while True:
        camera_list = camera_plugin.camera_list()
        if camera_list and len(camera_list.cameras) > 0:
            break
        time.sleep(1)

    print("Cameras found:")
    for camera in camera_list.cameras:
        print(f"Camera component ID: {camera.component_id}, model: {camera.model_name}, vendor: {camera.vendor_name}")

    if len(camera_list.cameras) > 1:
        print("More than one camera found, using first one discovered.")

    component_id = camera_list.cameras[0].component_id

    # Subscribe to storage updates
    storage_handle = camera_plugin.subscribe_storage(storage_callback)

    # Perform camera operations
    do_camera_operation(component_id, camera_plugin)

    print("Camera operations completed. Keeping running to receive updates...")
    
    # Keep running to receive updates
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Shutting down...")

    # Cleanup
    camera_plugin.unsubscribe_storage(storage_handle)

if __name__ == "__main__":
    main()
