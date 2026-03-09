import time
import sys

from mavsdk import *
from mavsdk.plugins.param import *


def print_usage(bin_name):
    print(f"Usage : {bin_name} <connection_url> <action> [args]")
    print()
    print("Connection URL format should be:")
    print(" For TCP server: tcpin://<our_ip>:<port>")
    print(" For TCP client: tcpout://<remote_ip>:<port>")
    print(" For UDP server: udpin://<our_ip>:<port>")
    print(" For UDP client: udpout://<remote_ip>:<port>")
    print(" For Serial : serial://</path/to/serial/dev>:<baudrate>]")
    print("For example, to connect to the simulator use URL: udpin://0.0.0.0:14540")
    print()
    print("Actions:")
    print("  get_all:          Print all parameters")
    print("  get <param name>: Get one param")
    print("  set <param name> <value>: Set one param")


def parse_args():
    if len(sys.argv) < 3:
        print("Insufficient arguments")
        return None

    connection = sys.argv[1]
    action_str = sys.argv[2]

    if action_str == "get_all":
        if len(sys.argv) != 3:
            print("get_all takes no additional arguments")
            return None
        return {"action": "get_all", "connection": connection}

    elif action_str == "get":
        if len(sys.argv) != 4:
            print("get requires a parameter name")
            return None
        return {"action": "get", "connection": connection, "param_name": sys.argv[3]}

    elif action_str == "set":
        if len(sys.argv) != 5:
            print("set requires a parameter name and value")
            return None
        return {"action": "set", "connection": connection, "param_name": sys.argv[3], "value": sys.argv[4]}

    else:
        print(f"Unknown action: {action_str}")
        return None


def get_all(param):
    all_params = param.get_all_params()

    print("Int params:")
    for p in all_params.int_params:
        print(f"{p.name}: {p.value}")

    print("Float params:")
    for p in all_params.float_params:
        print(f"{p.name}: {p.value}")


def get_param(param, name):
    print("Try as float param...", end="")
    try:
        float_value = param.get_param_float(name)
        print("Ok")
        print(f"{name}: {float_value}")
        return
    except Exception as e:
        if "WRONG_TYPE" not in str(e):
            print(f"Failed: {e}")
            return
        print("Wrong type")

    print("Try as int param next...", end="")
    try:
        int_value = param.get_param_int(name)
        print("Ok")
        print(f"{name}: {int_value}")
        return
    except Exception as e:
        if "WRONG_TYPE" not in str(e):
            print(f"Failed: {e}")
            return
        print("Failed")

    print("Neither int or float worked, should maybe try custom? (not implemented)")


def set_param(param, name, value):
    try:
        int_value = int(value)
        print(f"Setting {value} as integer...", end="")
        try:
            param.set_param_int(name, int_value)
            print("Ok")
            return
        except Exception as e:
            if "WRONG_TYPE" not in str(e):
                print(f"Failed: {e}")
                return
    except ValueError:
        pass

    try:
        float_value = float(value)
        print(f"Setting {value} as float...", end="")
        try:
            param.set_param_float(name, float_value)
            print("Ok")
            return
        except Exception as e:
            print(f"Failed: {e}")
            return
    except ValueError:
        pass

    print("Neither int or float worked, should maybe try custom? (not implemented)")


def main():
    args = parse_args()
    if not args:
        print_usage(sys.argv[0])
        return 1

    configuration = Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    mavsdk = Mavsdk(configuration)
    mavsdk.add_any_connection(args["connection"])

    print("Waiting to discover system...")

    drone = None
    wait_count = 0
    while drone is None and wait_count < 30:
        for system in mavsdk.get_systems():
            if system.has_autopilot() and system.is_connected():
                drone = system
                break
        if drone is None:
            time.sleep(1)
            wait_count += 1

    if not drone:
        print("No autopilot found, exiting.")
        return 1

    print("Discovered autopilot")

    param = Param(drone)

    if args["action"] == "get_all":
        get_all(param)
    elif args["action"] == "get":
        get_param(param, args["param_name"])
    elif args["action"] == "set":
        set_param(param, args["param_name"], args["value"])

    return 0


if __name__ == "__main__":
    sys.exit(main())
