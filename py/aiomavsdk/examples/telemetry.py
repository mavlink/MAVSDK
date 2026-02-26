import asyncio
from aiomavsdk import Mavsdk, Configuration, ComponentType
from aiomavsdk.plugins.action import ActionAsync
from aiomavsdk.plugins.telemetry import TelemetryAsync, LandedState


async def main():
    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)

    async with Mavsdk(configuration) as mavsdk:
        await mavsdk.add_any_connection("udpin://0.0.0.0:14540")

        print("Waiting for a drone...")

        drone = None
        async for _ in mavsdk.on_new_system():
            systems = await mavsdk.get_systems()
            for system in systems:
                if await system.has_autopilot() and await system.is_connected():
                    print("✓ Found valid autopilot system!")
                    drone = system
                    break
            if drone is not None:
                break

        if not drone:
            print("No valid system found!")
            return

        action = ActionAsync(drone)
        telemetry = TelemetryAsync(drone)

        async def print_positions():
            async for position in telemetry.subscribe_position():
                print(f"Position update: Lat={position.latitude_deg:.6f}, Lon={position.longitude_deg:.6f}, Alt={position.relative_altitude_m:.1f}m")

        async def wait_for_landing():
            async for state in telemetry.subscribe_landed_state():
                print(f"Landed state: {state}")
                if state == LandedState.ON_GROUND:
                    return

        position_task = asyncio.create_task(print_positions())

        print("Arming...")
        await action.arm()

        print("Taking off...")
        await action.takeoff()

        print("Flying for 10 seconds...")
        await asyncio.sleep(10)

        print("Current position (sync poll):")
        current_pos = await telemetry.position()
        print(f"  Lat: {current_pos.latitude_deg:.6f}")
        print(f"  Lon: {current_pos.longitude_deg:.6f}")
        print(f"  Alt: {current_pos.relative_altitude_m:.1f}m")

        print("Landing...")
        await action.land()

        print("Waiting for landing completion...")
        await wait_for_landing()

        position_task.cancel()
        try:
            await position_task
        except asyncio.CancelledError:
            pass

        print("Mission complete!")


if __name__ == "__main__":
    asyncio.run(main())
