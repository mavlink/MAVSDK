import asyncio
from aiomavsdk import Mavsdk, Configuration, ComponentType
from aiomavsdk.plugins.action import ActionAsync
from pymavsdk.logging import log_subscribe

async def main():
    log_subscribe(lambda level, msg, file, line: True)

    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)

    mavsdk = Mavsdk(configuration)
    await mavsdk.add_any_connection("udpin://0.0.0.0:14540")

    print("Waiting for a drone...")
    drone = None
    async for _ in mavsdk.on_new_system():
        systems = await mavsdk.get_systems()

        print(f"New system discovered event received, systems count: {len(systems)}")

        for system in systems:
            print(f"Checking new system: {system}")

            if await system.has_autopilot() and await system.is_connected():
                print("✓ Found valid autopilot system!")
                drone = system
                break

        if drone is not None:
            break

    if drone:
        action = ActionAsync(drone)

        print(f"Using system: {drone}")

        takeoff_alt = await action.get_takeoff_altitude()
        rtl_alt = await action.get_return_to_launch_altitude()

        print(f"Takeoff altitude: {takeoff_alt}")
        print(f"RTL altitude: {rtl_alt}")
        print("Arming...")

        await action.arm()
        print("Taking off...")

        await action.takeoff()
        print("Flying for 2 seconds...")

        await asyncio.sleep(2)
        print("Landing...")

        await action.land()
        print("Mission complete!")
    else:
        print("No valid system found!")

if __name__ == "__main__":
    asyncio.run(main())
