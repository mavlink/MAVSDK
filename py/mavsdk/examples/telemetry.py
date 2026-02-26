import time
from pymavsdk import *
from pymavsdk.plugins.action import *
from pymavsdk.plugins.telemetry import *

global mavsdk, drone
mavsdk = None
drone = None

def on_new_system(user_data=None):
    print("New system discovered")
    for system in mavsdk.get_systems():
        print(f"Checking new system: {system}")
        if system.has_autopilot() and system.is_connected():
            print("✓ Found valid autopilot system!")
            global drone
            drone = system
            break
        else:
            print(f"  - System missing autopilot or not connected (autopilot: {system.has_autopilot()}, connected: {system.is_connected()})")

def on_position_update(position, user_data=None):
    print(f"Position update: Lat={position.latitude_deg:.6f}, Lon={position.longitude_deg:.6f}, Alt={position.relative_altitude_m:.1f}m")

def main():
    global mavsdk
    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)
    mavsdk.add_any_connection("udpin://0.0.0.0:14540")

    global drone
    drone = None

    subscription_handle = mavsdk.subscribe_on_new_system(on_new_system)

    print("Waiting for a drone...")
    while drone is None:
        time.sleep(1)

    mavsdk.unsubscribe_on_new_system(subscription_handle)

    if drone:
        print(f"Using system: {drone}")
        action = Action(drone)
        telemetry = Telemetry(drone)

        position_handle = telemetry.subscribe_position(on_position_update)

        print("Arming...")
        action.arm()

        print("Taking off...")
        action.takeoff()

        print("Flying for 10 seconds...")
        time.sleep(10)

        print("Current position (sync poll):")
        current_pos = telemetry.position()
        print(f"  Lat: {current_pos.latitude_deg:.6f}")
        print(f"  Lon: {current_pos.longitude_deg:.6f}")
        print(f"  Alt: {current_pos.relative_altitude_m:.1f}m")

        print("Landing...")
        action.land()

        landed = False
        def on_landed_state(state, user_data=None):
            nonlocal landed
            print(f"Landed state: {state}")
            if state == LandedState.ON_GROUND:
                landed = True

        landed_handle = telemetry.subscribe_landed_state(on_landed_state)

        print("Waiting for landing completion...")
        while not landed:
            time.sleep(0.5)

        print("Mission complete!")
    else:
        print("No valid system found!")

if __name__ == "__main__":
    main()
