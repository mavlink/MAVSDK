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
    latitude_deg, longitude_deg, relative_altitude_m, speed_m_s,
    is_fly_through, gimbal_pitch_deg, gimbal_yaw_deg, camera_action
):
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
    mavsdk.add_any_connection(sys.argv[1])

    print("Waiting for system...")
    drone = None
    while drone is None:
        time.sleep(1)
        for system in mavsdk.get_systems():
            if system.has_autopilot() and system.is_connected():
                drone = system
                break

    print("Found autopilot system!")

    action = Action(drone)
    mission = Mission(drone)
    telemetry = Telemetry(drone)

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

    mission_items = [
        make_mission_item(47.398170327054473, 8.5456490218639658, 10.0, 5.0, False, 20.0, 60.0, MissionItem.CameraAction.NONE),
        make_mission_item(47.398241338125118, 8.5455360114574432, 10.0, 2.0, True, 0.0, -60.0, MissionItem.CameraAction.TAKE_PHOTO),
        make_mission_item(47.398139363821485, 8.5453846156597137, 10.0, 5.0, True, -45.0, 0.0, MissionItem.CameraAction.START_VIDEO),
        make_mission_item(47.398058617228855, 8.5454618036746979, 10.0, 2.0, False, -90.0, 30.0, MissionItem.CameraAction.STOP_VIDEO),
        make_mission_item(47.398100366082858, 8.5456969141960144, 10.0, 5.0, False, -45.0, -30.0, MissionItem.CameraAction.START_PHOTO_INTERVAL),
        make_mission_item(47.398001890458097, 8.5455576181411743, 10.0, 5.0, False, 0.0, 0.0, MissionItem.CameraAction.STOP_PHOTO_INTERVAL),
    ]

    mission_plan = MissionPlan(mission_items)

    print("Uploading mission...")
    mission.upload_mission(mission_plan)

    print("Arming...")
    action.arm()
    print("Armed.")

    want_to_pause = False
    def mission_progress_callback(progress, user_data=None):
        nonlocal want_to_pause
        print(f"Mission status update: {progress.current} / {progress.total}")
        if progress.current >= 2:
            want_to_pause = True

    mission.subscribe_mission_progress(mission_progress_callback)

    print("Starting mission...")
    mission.start_mission()

    while not want_to_pause:
        time.sleep(1)

    print("Pausing mission...")
    mission.pause_mission()
    print("Mission paused.")

    time.sleep(5)

    print("Continuing mission...")
    mission.start_mission()

    while not mission.is_mission_finished():
        time.sleep(1)

    print("Commanding RTL...")
    action.return_to_launch()
    print("Commanded RTL.")

    time.sleep(2)

    while telemetry.armed():
        time.sleep(1)

    print("Disarmed, exiting.")
    return 0

if __name__ == "__main__":
    sys.exit(main())
