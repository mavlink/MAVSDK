import faulthandler
faulthandler.enable()

import asyncio
from aiomavsdk import Mavsdk, Configuration, ComponentType
from pymavsdk.logging import log_subscribe


async def main():
    # Disable the MAVSDK log output
    log_subscribe(lambda level, msg, file, line: True)

    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)

    async with Mavsdk(configuration) as mavsdk:
        await mavsdk.add_any_connection("udpin://0.0.0.0:14540")

        print("Waiting for a drone...")

        drone = None

        async for _ in mavsdk.on_new_system():
            systems = await mavsdk.get_systems()
            print(f"New system discovered event received, systems count: {len(systems)}")

            for system in systems:
                if await system.has_autopilot() and await system.is_connected():
                    print("✓ Found valid autopilot system!")
                    drone = system
                    break

            if drone is not None:
                break  # stop listening for new systems

        if drone:
            print(f"System ID: {await drone.get_system_id()}")
            print("Ready for further commands.")
        else:
            print("No valid system found!")


if __name__ == "__main__":
    asyncio.run(main())
