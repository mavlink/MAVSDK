import time

from pymavsdk import *
from pymavsdk.plugins.action import *

def main():
    # Disable the MAVSDK log output
    log_subscribe(lambda level, msg, file, line: True)

    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)
    mavsdk.add_any_connection("udpin://0.0.0.0:14540")
    
    drone = None
    
    def on_new_system(user_data=None):
        nonlocal drone
        print("New system discovered event received")
        
        # Get all current systems
        current_systems = mavsdk.get_systems()
        print(f"Current systems count: {len(current_systems)}")
        
        # Check for new valid systems
        for system in current_systems:
            print(f"Checking new system: {system}")
            
            # Check if this system is what we want
            if system.has_autopilot() and system.is_connected():
                print("✓ Found valid autopilot system!")
                drone = system
                break
            else:
                print(f"  - System missing autopilot or not connected (autopilot: {system.has_autopilot()}, connected: {system.is_connected()})")
    
    # Subscribe to new systems
    subscription_handle = mavsdk.subscribe_on_new_system(on_new_system)
    
    print("Waiting for a drone...")
    
    # Wait indefinitely for a valid system
    while drone is None:
        time.sleep(1)
    
    # Unsubscribe since we found our system
    mavsdk.unsubscribe_on_new_system(subscription_handle)
    
    if drone:
        print(f"Using system: {drone}")
        action = Action(drone)

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
