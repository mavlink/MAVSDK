import time
from pymavsdk import *
from pymavsdk.plugins.camera_server import *

start_video_time = 0
is_recording_video = False
is_capture_in_progress = False
image_count = 0


def take_photo_callback(index, user_data=None):
    global is_capture_in_progress, image_count
    camera_server = user_data

    camera_server.set_in_progress(True)
    is_capture_in_progress = True
    print(f"Taking a picture ({index})...")
    time.sleep(0.5)

    capture_info = CaptureInfo()
    capture_info.position = Position(
        latitude_deg=0.0,
        longitude_deg=0.0,
        absolute_altitude_m=0.0,
        relative_altitude_m=0.0
    )
    capture_info.attitude_quaternion = Quaternion(w=1.0, x=0.0, y=0.0, z=0.0)
    capture_info.time_utc_us = int(time.time() * 1_000_000)
    capture_info.is_success = True
    capture_info.index = index
    capture_info.file_url = ""

    image_count += 1
    camera_server.set_in_progress(False)
    camera_server.respond_take_photo(CameraFeedback.OK, capture_info)
    is_capture_in_progress = False


def start_video_callback(stream_id, user_data=None):
    global is_recording_video, start_video_time
    camera_server = user_data

    print("Start video record")
    is_recording_video = True
    start_video_time = time.time()
    camera_server.respond_start_video(CameraFeedback.OK)


def stop_video_callback(stream_id, user_data=None):
    global is_recording_video
    camera_server = user_data

    print("Stop video record")
    is_recording_video = False
    camera_server.respond_stop_video(CameraFeedback.OK)


def start_video_streaming_callback(stream_id, user_data=None):
    camera_server = user_data
    print(f"Start video streaming {stream_id}")
    camera_server.respond_start_video_streaming(CameraFeedback.OK)


def stop_video_streaming_callback(stream_id, user_data=None):
    camera_server = user_data
    print(f"Stop video streaming {stream_id}")
    camera_server.respond_stop_video_streaming(CameraFeedback.OK)


def set_mode_callback(mode, user_data=None):
    camera_server = user_data
    print(f"Set camera mode {mode}")
    camera_server.respond_set_mode(CameraFeedback.OK)


def storage_information_callback(storage_id, user_data=None):
    camera_server = user_data
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
    camera_server.respond_storage_information(CameraFeedback.OK, storage_information)


def capture_status_callback(reserved, user_data=None):
    camera_server = user_data

    capture_status = CaptureStatus()
    capture_status.image_count = image_count
    capture_status.image_status = (
        CaptureStatusImageStatus.CAPTURE_IN_PROGRESS if is_capture_in_progress
        else CaptureStatusImageStatus.IDLE
    )
    capture_status.video_status = (
        CaptureStatusVideoStatus.CAPTURE_IN_PROGRESS if is_recording_video
        else CaptureStatusVideoStatus.IDLE
    )
    if is_recording_video:
        capture_status.recording_time_s = float(time.time() - start_video_time)

    camera_server.respond_capture_status(CameraFeedback.OK, capture_status)


def format_storage_callback(storage_id, user_data=None):
    camera_server = user_data
    print(f"Format storage with id: {storage_id}")
    camera_server.respond_format_storage(CameraFeedback.OK)


def reset_settings_callback(camera_id, user_data=None):
    camera_server = user_data
    print("Reset camera settings")
    camera_server.respond_reset_settings(CameraFeedback.OK)


def subscribe_camera_operation(camera_server):
    camera_server.subscribe_take_photo(take_photo_callback, camera_server)
    camera_server.subscribe_start_video(start_video_callback, camera_server)
    camera_server.subscribe_stop_video(stop_video_callback, camera_server)
    camera_server.subscribe_start_video_streaming(start_video_streaming_callback, camera_server)
    camera_server.subscribe_stop_video_streaming(stop_video_streaming_callback, camera_server)
    camera_server.subscribe_set_mode(set_mode_callback, camera_server)
    camera_server.subscribe_storage_information(storage_information_callback, camera_server)
    camera_server.subscribe_capture_status(capture_status_callback, camera_server)
    camera_server.subscribe_format_storage(format_storage_callback, camera_server)
    camera_server.subscribe_reset_settings(reset_settings_callback, camera_server)


def main():
    configuration = Configuration.create_with_component_type(ComponentType.CAMERA)
    mavsdk = Mavsdk(configuration)
    mavsdk.add_any_connection("udpin://0.0.0.0:14030")
    print("Created camera server connection")

    server_component = mavsdk.server_component(0)
    camera_server = CameraServer(server_component)

    subscribe_camera_operation(camera_server)
    camera_server.set_in_progress(False)

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

    camera_server.set_information(information)

    print("Ready for clients")
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Shutting down camera server")


if __name__ == "__main__":
    main()
