import time
import sys
from pymavsdk import *
from pymavsdk.plugins.action import *
from pymavsdk.plugins.mission import *
from pymavsdk.plugins.telemetry import *

def usage(bin_name):
    print(f"Usage : {bin_name} <connection_url>")
    print("Connection URL format should be :")
    print(" For TCP server: tcpin://<our_ip>:<port>")
    print(" For TCP client: tcpout://<remote_ip>:<port>")
    print(" For UDP server: udpin://<our_ip>:<port>")
    print(" For UDP client: udpout://<remote_ip>:<port>")
    print(" For Serial : serial://</path/to/serial/dev>:<baudrate>]")
    print("For example, to connect to the simulator use URL: udpin://0.0.0.0:14540")

def make_mission_item(
    latitude_deg,
    longitude_deg, 
    relative_altitude_m,
    speed_m_s,
    is_fly_through,
    gimbal_pitch_deg,
    gimbal_yaw_deg,
    camera_action
):
    """Create a mission item with the given parameters"""
    item = MissionItem()
    item.latitude_deg = latitude_deg
    item.longitude_deg = longitude_deg
    item.relative_altitude_m = relative_altitude_m
    item.speed_m_s = speed_m_s
    item.is_fly_through = is_fly_through
    item.gimbal_pitch_deg = gimbal_pitch_deg
    item.gimbal_yaw_deg = gimbal_yaw_deg
    item.camera_action = camera_action
    item.loiter_time_s = 0.0
    item.camera_photo_interval_s = 1.0
    item.acceptance_radius_m = 1.0
    item.yaw_deg = 0.0
    item.camera_photo_distance_m = 0.0
    item.vehicle_action = MissionItem.VehicleAction.NONE
    return item

def main():
    if len(sys.argv) != 2:
        usage(sys.argv[0])
        return 1

    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)

    # Add connection
    connection_result = mavsdk.add_any_connection(sys.argv[1])

    if connection_result != ConnectionResult.SUCCESS:
        print(f"Connection failed: {connection_result}")
        return 1

    print("Waiting for system...")

    # Wait for autopilot system
    drone = None
    while drone is None:
        time.sleep(1)

        systems = mavsdk.get_systems()
        for system in systems:
            if system.has_autopilot() and system.is_connected():
                drone = system
                break

    print("Found autopilot system!")

    # Create plugins
    action = Action(drone)
    mission = Mission(drone)
    telemetry = Telemetry(drone)

    # Wait for system to be ready
    health_ok = False
    while not health_ok:
        print("Waiting for system to be ready")
        time.sleep(1)
        health = telemetry.health()
        health_ok = (health.is_gyrometer_calibration_ok and
                    health.is_accelerometer_calibration_ok and
                    health.is_magnetometer_calibration_ok and
                    health.is_local_position_ok and
                    health.is_global_position_ok and
                    health.is_home_position_ok)

    print("System ready")
    print("Creating and uploading mission")

    # Create mission items
    mission_items = [
        make_mission_item(
            47.398170327054473, 8.5456490218639658, 10.0, 5.0, False,
            20.0, 60.0, MissionItem.CameraAction.NONE
        ),
        make_mission_item(
            47.398241338125118, 8.5455360114574432, 10.0, 2.0, True,
            0.0, -60.0, MissionItem.CameraAction.TAKE_PHOTO
        ),
        make_mission_item(
            47.398139363821485, 8.5453846156597137, 10.0, 5.0, True,
            -45.0, 0.0, MissionItem.CameraAction.START_VIDEO
        ),
        make_mission_item(
            47.398058617228855, 8.5454618036746979, 10.0, 2.0, False,
            -90.0, 30.0, MissionItem.CameraAction.STOP_VIDEO
        ),
        make_mission_item(
            47.398100366082858, 8.5456969141960144, 10.0, 5.0, False,
            -45.0, -30.0, MissionItem.CameraAction.START_PHOTO_INTERVAL
        ),
        make_mission_item(
            47.398001890458097, 8.5455576181411743, 10.0, 5.0, False,
            0.0, 0.0, MissionItem.CameraAction.STOP_PHOTO_INTERVAL
        )
    ]

    # Create mission plan
    mission_plan = MissionPlan(mission_items)

    # Upload mission
    print("Uploading mission...")
    upload_result = mission.upload_mission(mission_plan)

    if upload_result != MissionResult.SUCCESS:
        print(f"Mission upload failed: {upload_result}, exiting.")
        return 1

    # Arm
    print("Arming...")
    arm_result = action.arm()
    if arm_result != ActionResult.SUCCESS:
        print(f"Arming failed: {arm_result}")
        return 1
    print("Armed.")

    # Subscribe to mission progress before starting
    want_to_pause = False

    def mission_progress_callback(progress, user_data=None):
        print(f"Mission status update: {progress.current} / {progress.total}")

        if progress.current >= 2:
            nonlocal want_to_pause
            want_to_pause = True

    progress_handle = mission.subscribe_mission_progress(mission_progress_callback)

    # Start mission
    print("Starting mission...")
    start_result = mission.start_mission()
    if start_result != MissionResult.SUCCESS:
        print(f"Starting mission failed: {start_result}")
        mission.unsubscribe_mission_progress(progress_handle)
        return 1

    # Wait for pause condition
    while not want_to_pause:
        time.sleep(1)

    # Pause mission
    print("Pausing mission...")
    pause_result = mission.pause_mission()
    if pause_result != MissionResult.SUCCESS:
        print(f"Failed to pause mission: {pause_result}")
    print("Mission paused.")

    # Pause for 5 seconds
    time.sleep(5)

    # Continue mission
    print("Continuing mission...")
    continue_result = mission.start_mission()
    if continue_result != MissionResult.SUCCESS:
        print(f"Starting mission again failed: {continue_result}")
        mission.unsubscribe_mission_progress(progress_handle)
        return 1

    # Wait for mission to finish
    is_finished = False
    while not is_finished:
        time.sleep(1)
        is_finished = mission.is_mission_finished()

    # Return to launch
    print("Commanding RTL...")
    rtl_result = action.return_to_launch()
    if rtl_result != ActionResult.SUCCESS:
        print(f"Failed to command RTL: {rtl_result}")
        mission.unsubscribe_mission_progress(progress_handle)
        return 1
    print("Commanded RTL.")

    # Wait a bit for armed state to be correct
    time.sleep(2)

    # Wait until disarmed
    armed = True
    while armed:
        time.sleep(1)
        armed = telemetry.armed()

    print("Disarmed, exiting.")

    # Cleanup
    mission.unsubscribe_mission_progress(progress_handle)

    return 0

if __name__ == "__main__":
    sys.exit(main())
