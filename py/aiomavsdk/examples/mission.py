import asyncio
import sys
from aiomavsdk import Mavsdk, Configuration, ComponentType
from aiomavsdk.plugins.action import ActionAsync
from aiomavsdk.plugins.mission import MissionAsync, MissionItem, MissionPlan
from aiomavsdk.plugins.telemetry import TelemetryAsync


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


async def main():
    if len(sys.argv) != 2:
        usage(sys.argv[0])
        return 1

    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)
    await mavsdk.add_any_connection(sys.argv[1])

    print("Waiting for system...")
    drone = None
    async for _ in mavsdk.on_new_system():
        for system in await mavsdk.get_systems():
            if await system.has_autopilot() and await system.is_connected():
                drone = system
                break
        if drone is not None:
            break

    print("Found autopilot system!")

    action = ActionAsync(drone)
    mission = MissionAsync(drone)
    telemetry = TelemetryAsync(drone)

    while True:
        print("Waiting for system to be ready")
        health = await telemetry.health()
        if (health.is_gyrometer_calibration_ok and
                health.is_accelerometer_calibration_ok and
                health.is_magnetometer_calibration_ok and
                health.is_local_position_ok and
                health.is_global_position_ok and
                health.is_home_position_ok):
            break
        await asyncio.sleep(1)

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
    await mission.upload_mission(mission_plan)

    print("Arming...")
    await action.arm()
    print("Armed.")

    want_to_pause = asyncio.Event()

    async def watch_progress():
        async for progress in mission.subscribe_mission_progress():
            print(f"Mission status update: {progress.current} / {progress.total}")
            if progress.current >= 2:
                want_to_pause.set()
                return

    progress_task = asyncio.create_task(watch_progress())

    print("Starting mission...")
    await mission.start_mission()

    await want_to_pause.wait()

    print("Pausing mission...")
    await mission.pause_mission()
    print("Mission paused.")

    await asyncio.sleep(5)

    print("Continuing mission...")
    await mission.start_mission()

    progress_task.cancel()
    try:
        await progress_task
    except asyncio.CancelledError:
        pass

    while not await mission.is_mission_finished():
        await asyncio.sleep(1)

    print("Commanding RTL...")
    await action.return_to_launch()
    print("Commanded RTL.")

    await asyncio.sleep(2)

    while await telemetry.armed():
        await asyncio.sleep(1)

    print("Disarmed, exiting.")
    return 0


if __name__ == "__main__":
    sys.exit(asyncio.run(main()))
