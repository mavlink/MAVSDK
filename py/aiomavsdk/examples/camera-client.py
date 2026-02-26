import asyncio
from aiomavsdk import Mavsdk, Configuration, ComponentType
from aiomavsdk.plugins.camera import CameraAsync, Mode


async def print_storage_updates(camera_plugin: CameraAsync):
    async for update in camera_plugin.subscribe_storage():
        print("Camera storage update:")
        print(f"  Component ID: {update.storage.component_id}")
        print(f"  Storage ID: {update.storage.storage_id}")
        print(f"  Total storage: {update.storage.total_storage_mib:.2f} MiB")
        print(f"  Used storage: {update.storage.used_storage_mib:.2f} MiB")
        print(f"  Available storage: {update.storage.available_storage_mib:.2f} MiB")
        print(f"  Storage status: {update.storage.storage_status}")
        print(f"  Storage type: {update.storage.storage_type}")


async def do_camera_operation(component_id: int, camera_plugin: CameraAsync):
    await camera_plugin.set_mode(component_id, Mode.PHOTO)
    print("Set camera to photo mode")

    await camera_plugin.take_photo(component_id)
    print("Took photo")

    await camera_plugin.set_mode(component_id, Mode.VIDEO)
    print("Set camera to video mode")

    await camera_plugin.start_video(component_id)
    print("Started video")

    await camera_plugin.stop_video(component_id)
    print("Stopped video")

    await camera_plugin.start_video_streaming(component_id, 0)
    print("Started video streaming")

    await camera_plugin.stop_video_streaming(component_id, 0)
    print("Stopped video streaming")

    await camera_plugin.format_storage(component_id, 0)
    print("Formatted storage")

    await camera_plugin.reset_settings(component_id)
    print("Reset camera settings")


async def main():
    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)
    await mavsdk.add_any_connection("udpout://127.0.0.1:14030")
    print("Connected!")

    print("Waiting for camera system...")
    camera_server = None
    async for _ in mavsdk.on_new_system():
        systems = await mavsdk.get_systems()
        if systems:
            camera_server = systems[0]
            break

    camera_plugin = CameraAsync(camera_server)

    print("Waiting for cameras...")
    while True:
        camera_list = await camera_plugin.camera_list()
        if camera_list and len(camera_list.cameras) > 0:
            break
        await asyncio.sleep(1)

    print("Cameras found:")
    for camera in camera_list.cameras:
        print(f"Camera component ID: {camera.component_id}, model: {camera.model_name}, vendor: {camera.vendor_name}")

    if len(camera_list.cameras) > 1:
        print("More than one camera found, using first one discovered.")

    component_id = camera_list.cameras[0].component_id

    storage_task = asyncio.create_task(print_storage_updates(camera_plugin))

    await do_camera_operation(component_id, camera_plugin)

    print("Camera operations completed. Keeping running to receive updates...")
    try:
        await asyncio.sleep(float('inf'))
    except asyncio.CancelledError:
        pass
    finally:
        storage_task.cancel()
        try:
            await storage_task
        except asyncio.CancelledError:
            pass


if __name__ == "__main__":
    asyncio.run(main())
