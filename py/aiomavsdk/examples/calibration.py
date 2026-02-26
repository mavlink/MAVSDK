import asyncio
from aiomavsdk import Mavsdk, Configuration, ComponentType
from aiomavsdk.plugins.calibration import CalibrationAsync, CalibrationResult


async def calibrate_accelerometer(calibration: CalibrationAsync):
    print("Calibrating accelerometer...")
    async for result, progress_data in calibration.calibrate_accelerometer():
        if result == CalibrationResult.NEXT:
            if progress_data.has_progress:
                print(f"    Progress: {int(progress_data.progress * 100)}%")
            if progress_data.has_status_text:
                print(f"    Instruction: {progress_data.status_text}")
        elif result == CalibrationResult.SUCCESS:
            print("--- Calibration succeeded!")
            return
        else:
            print(f"--- Calibration failed: {result}")
            return


async def calibrate_gyro(calibration: CalibrationAsync):
    print("Calibrating gyro...")
    async for result, progress_data in calibration.calibrate_gyro():
        if result == CalibrationResult.NEXT:
            if progress_data.has_progress:
                print(f"    Progress: {int(progress_data.progress * 100)}%")
            if progress_data.has_status_text:
                print(f"    Instruction: {progress_data.status_text}")
        elif result == CalibrationResult.SUCCESS:
            print("--- Calibration succeeded!")
            return
        else:
            print(f"--- Calibration failed: {result}")
            return


async def calibrate_magnetometer(calibration: CalibrationAsync):
    print("Calibrating magnetometer...")
    async for result, progress_data in calibration.calibrate_magnetometer():
        if result == CalibrationResult.NEXT:
            if progress_data.has_progress:
                print(f"    Progress: {int(progress_data.progress * 100)}%")
            if progress_data.has_status_text:
                print(f"    Instruction: {progress_data.status_text}")
        elif result == CalibrationResult.SUCCESS:
            print("--- Calibration succeeded!")
            return
        else:
            print(f"--- Calibration failed: {result}")
            return


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

    await calibrate_accelerometer(calibration)
    await calibrate_gyro(calibration)
    await calibrate_magnetometer(calibration)

    print("All calibrations completed!")


if __name__ == "__main__":
    asyncio.run(main())
