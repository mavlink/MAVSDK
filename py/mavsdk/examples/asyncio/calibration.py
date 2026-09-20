import asyncio
from mavsdk.asyncio import Mavsdk, Configuration, ComponentType
from mavsdk.asyncio.plugins.calibration import (
    CalibrationAsync,
    CalibrationError,
    CalibrationResult,
)


async def run_calibration(name, progress):
    print(f"Calibrating {name}...")
    try:
        async for result, progress_data in progress:
            if result == CalibrationResult.SUCCESS:
                print("--- Calibration succeeded!")
                continue
            if progress_data.has_progress:
                print(f"    Progress: {int(progress_data.progress * 100)}%")
            if progress_data.has_status_text:
                print(f"    Instruction: {progress_data.status_text}")
    except CalibrationError as e:
        print(f"--- Calibration failed: {e.result.name}")


async def main():
    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)
    await mavsdk.add_any_connection("udpin://0.0.0.0:14540")

    print("Waiting for autopilot...")

    drone = None
    async for _ in mavsdk.on_new_system():
        systems = await mavsdk.get_systems()
        for system in systems:
            if await system.has_autopilot() and await system.is_connected():
                print("Found autopilot system!")
                drone = system
                break
        if drone is not None:
            break

    if not drone:
        print("No valid system found!")
        return

    calibration = CalibrationAsync(drone)

    await run_calibration("accelerometer", calibration.calibrate_accelerometer())
    await run_calibration("gyro", calibration.calibrate_gyro())
    await run_calibration("magnetometer", calibration.calibrate_magnetometer())

    print("All calibrations completed!")


if __name__ == "__main__":
    asyncio.run(main())
