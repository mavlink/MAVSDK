import time
from mavsdk import *
from mavsdk.plugins.action import *

def main():
    log_subscribe(lambda level, msg, file, line: True)

    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)

    mavsdk = Mavsdk(configuration)
    mavsdk.add_any_connection("udpin://0.0.0.0:14540")

    drone = None

    def on_new_system(user_data=None):
        nonlocal drone

        print("New system discovered event received")

        for system in mavsdk.get_systems():
            print(f"Checking new system: {system}")

            if system.has_autopilot() and system.is_connected():
                print("✓ Found valid autopilot system!")
                drone = system
                break

    subscription_handle = mavsdk.subscribe_on_new_system(on_new_system)

    print("Waiting for a drone...")
    while drone is None:
        time.sleep(1)

    mavsdk.unsubscribe_on_new_system(subscription_handle)

    if drone:
        action = Action(drone)

        print(f"Using system: {drone}")
        print(f"Takeoff altitude: {action.get_takeoff_altitude()}")
        print(f"RTL altitude: {action.get_return_to_launch_altitude()}")

        print("Arming...")
        action.arm()

        print("Taking off...")
        action.takeoff()

        print("Flying for 2 seconds...")
        time.sleep(2)

        print("Landing...")
        action.land()

        print("Mission complete!")
    else:
        print("No valid system found!")

if __name__ == "__main__":
    main()
