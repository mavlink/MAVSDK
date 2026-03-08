import asyncio
from aiomavsdk import Mavsdk, Configuration, ComponentType
from aiomavsdk.plugins.camera_server import (
    CameraServerAsync, CameraFeedback, CaptureInfo, Position, Quaternion,
    StorageInformation, CaptureStatus, Information
)


class CameraState:
    def __init__(self):
        self.start_video_time = 0.0
        self.is_recording_video = False
        self.is_capture_in_progress = False
        self.image_count = 0


async def handle_take_photo(camera_server: CameraServerAsync, index: int, state: CameraState):
    await camera_server.set_in_progress(True)
    state.is_capture_in_progress = True
    print(f"Taking a picture ({index})...")

    await asyncio.sleep(0.5)

    capture_info = CaptureInfo()
    capture_info.position = Position(
        latitude_deg=0.0,
        longitude_deg=0.0,
        absolute_altitude_m=0.0,
        relative_altitude_m=0.0
    )
    capture_info.attitude_quaternion = Quaternion(w=1.0, x=0.0, y=0.0, z=0.0)
    capture_info.time_utc_us = int(asyncio.get_event_loop().time() * 1_000_000)
    capture_info.is_success = True
    capture_info.index = index
    capture_info.file_url = ""

    state.image_count += 1
    await camera_server.set_in_progress(False)
    await camera_server.respond_take_photo(CameraFeedback.OK, capture_info)
    state.is_capture_in_progress = False


async def handle_start_video(camera_server: CameraServerAsync, stream_id: int, state: CameraState):
    import time
    print("Start video record")
    state.is_recording_video = True
    state.start_video_time = time.time()
    await camera_server.respond_start_video(CameraFeedback.OK)


async def handle_stop_video(camera_server: CameraServerAsync, stream_id: int, state: CameraState):
    print("Stop video record")
    state.is_recording_video = False
    await camera_server.respond_stop_video(CameraFeedback.OK)


async def handle_start_video_streaming(camera_server: CameraServerAsync, stream_id: int):
    print(f"Start video streaming {stream_id}")
    await camera_server.respond_start_video_streaming(CameraFeedback.OK)


async def handle_stop_video_streaming(camera_server: CameraServerAsync, stream_id: int):
    print(f"Stop video streaming {stream_id}")
    await camera_server.respond_stop_video_streaming(CameraFeedback.OK)


async def handle_set_mode(camera_server: CameraServerAsync, mode):
    print(f"Set camera mode {mode}")
    await camera_server.respond_set_mode(CameraFeedback.OK)


async def handle_storage_information(camera_server: CameraServerAsync, storage_id: int):
    print(f"Storage information requested for storage_id: {storage_id}")

    total_storage = 4 * 1024 * 1024  # 4GB
    storage_information = StorageInformation()
    storage_information.total_storage_mib = total_storage
    storage_information.used_storage_mib = 100.0
    storage_information.available_storage_mib = total_storage - storage_information.used_storage_mib
    storage_information.storage_status = StorageInformation.StorageStatus.FORMATTED
    storage_information.storage_type = StorageInformation.StorageType.MICROSD
    storage_information.storage_id = storage_id
    storage_information.read_speed_mib_s = 0.0
    storage_information.write_speed_mib_s = 0.0
    await camera_server.respond_storage_information(CameraFeedback.OK, storage_information)


async def handle_capture_status(camera_server: CameraServerAsync, state: CameraState):
    import time
    capture_status = CaptureStatus()
    capture_status.image_count = state.image_count
    capture_status.image_status = (
        CaptureStatus.ImageStatus.CAPTURE_IN_PROGRESS if state.is_capture_in_progress
        else CaptureStatus.ImageStatus.IDLE
    )
    capture_status.video_status = (
        CaptureStatus.VideoStatus.CAPTURE_IN_PROGRESS if state.is_recording_video
        else CaptureStatus.VideoStatus.IDLE
    )
    if state.is_recording_video:
        capture_status.recording_time_s = float(time.time() - state.start_video_time)
    await camera_server.respond_capture_status(CameraFeedback.OK, capture_status)


async def handle_format_storage(camera_server: CameraServerAsync, storage_id: int):
    print(f"Format storage with id: {storage_id}")
    await camera_server.respond_format_storage(CameraFeedback.OK)


async def handle_reset_settings(camera_server: CameraServerAsync, camera_id: int):
    print("Reset camera settings")
    await camera_server.respond_reset_settings(CameraFeedback.OK)


async def run_camera_operations(camera_server: CameraServerAsync, state: CameraState):
    """Fan out all camera subscription streams concurrently."""
    async def take_photo_loop():
        async for index in camera_server.subscribe_take_photo():
            await handle_take_photo(camera_server, index, state)

    async def start_video_loop():
        async for stream_id in camera_server.subscribe_start_video():
            await handle_start_video(camera_server, stream_id, state)

    async def stop_video_loop():
        async for stream_id in camera_server.subscribe_stop_video():
            await handle_stop_video(camera_server, stream_id, state)

    async def start_video_streaming_loop():
        async for stream_id in camera_server.subscribe_start_video_streaming():
            await handle_start_video_streaming(camera_server, stream_id)

    async def stop_video_streaming_loop():
        async for stream_id in camera_server.subscribe_stop_video_streaming():
            await handle_stop_video_streaming(camera_server, stream_id)

    async def set_mode_loop():
        async for mode in camera_server.subscribe_set_mode():
            await handle_set_mode(camera_server, mode)

    async def storage_information_loop():
        async for storage_id in camera_server.subscribe_storage_information():
            await handle_storage_information(camera_server, storage_id)

    async def capture_status_loop():
        async for _ in camera_server.subscribe_capture_status():
            await handle_capture_status(camera_server, state)

    async def format_storage_loop():
        async for storage_id in camera_server.subscribe_format_storage():
            await handle_format_storage(camera_server, storage_id)

    async def reset_settings_loop():
        async for camera_id in camera_server.subscribe_reset_settings():
            await handle_reset_settings(camera_server, camera_id)

    await asyncio.gather(
        take_photo_loop(),
        start_video_loop(),
        stop_video_loop(),
        start_video_streaming_loop(),
        stop_video_streaming_loop(),
        set_mode_loop(),
        storage_information_loop(),
        capture_status_loop(),
        format_storage_loop(),
        reset_settings_loop(),
    )


async def main():
    configuration = Configuration.create_with_component_type(ComponentType.CAMERA)
    mavsdk = Mavsdk(configuration)
    await mavsdk.add_any_connection("udpin://0.0.0.0:14030")
    print("Created camera server connection")

    server_component = await mavsdk.server_component(0)
    camera_server = CameraServerAsync(server_component)

    state = CameraState()
    await camera_server.set_in_progress(False)

    information = Information()
    information.vendor_name = "MAVSDK"
    information.model_name = "Example Camera Server"
    information.firmware_version = "1.0.0"
    information.focal_length_mm = 3.0
    information.horizontal_sensor_size_mm = 3.68
    information.vertical_sensor_size_mm = 2.76
    information.horizontal_resolution_px = 3280
    information.vertical_resolution_px = 2464
    information.lens_id = 0
    information.definition_file_version = 0
    information.definition_file_uri = ""
    information.image_in_video_mode_supported = False
    information.video_in_image_mode_supported = False
    await camera_server.set_information(information)

    print("Ready for clients")

    ops_task = asyncio.create_task(run_camera_operations(camera_server, state))

    try:
        await asyncio.sleep(float('inf'))
    except asyncio.CancelledError:
        pass
    finally:
        ops_task.cancel()
        try:
            await ops_task
        except asyncio.CancelledError:
            pass


if __name__ == "__main__":
    asyncio.run(main())
