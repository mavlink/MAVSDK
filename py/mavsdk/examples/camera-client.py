import time
from mavsdk import *
from mavsdk.plugins.camera import *


def storage_callback(update, user_data=None):
    print("Camera storage update:")
    print(f"  Component ID: {update.storage.component_id}")
    print(f"  Storage ID: {update.storage.storage_id}")
    print(f"  Total storage: {update.storage.total_storage_mib:.2f} MiB")
    print(f"  Used storage: {update.storage.used_storage_mib:.2f} MiB")
    print(f"  Available storage: {update.storage.available_storage_mib:.2f} MiB")
    print(f"  Storage status: {update.storage.storage_status}")
    print(f"  Storage type: {update.storage.storage_type}")


def do_camera_operation(component_id, camera_plugin):
    camera_plugin.set_mode(component_id, Mode.PHOTO)
    print("Set camera to photo mode")

    camera_plugin.take_photo(component_id)
    print("Took photo")

    camera_plugin.set_mode(component_id, Mode.VIDEO)
    print("Set camera to video mode")

    camera_plugin.start_video(component_id)
    print("Started video")

    camera_plugin.stop_video(component_id)
    print("Stopped video")

    camera_plugin.start_video_streaming(component_id, 0)
    print("Started video streaming")

    camera_plugin.stop_video_streaming(component_id, 0)
    print("Stopped video streaming")

    camera_plugin.format_storage(component_id, 0)
    print("Formatted storage")

    camera_plugin.reset_settings(component_id)
    print("Reset camera settings")


def main():
    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)
    mavsdk.add_any_connection("udpout://127.0.0.1:14030")
    print("Connected!")

    print("Waiting for camera system...")
    camera_server = None
    while camera_server is None:
        systems = mavsdk.get_systems()
        if systems:
            camera_server = systems[0]
        else:
            time.sleep(1)

    camera_plugin = Camera(camera_server)

    print("Waiting for cameras...")
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

    camera_plugin.subscribe_storage(storage_callback)

    do_camera_operation(component_id, camera_plugin)

    print("Camera operations completed. Keeping running to receive updates...")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Shutting down...")


if __name__ == "__main__":
    main()
