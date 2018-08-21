#!/usr/bin/env python3

from __future__ import print_function
import sys
if sys.version_info < (3,0):
    print("Error: Please run with python3")
    sys.exit(1)

import pydronecode_sdk
import time
import threading
import signal
import math


# We use an event to tell the thread to stop again.
should_exit = threading.Event()
thread = None

def signal_handler(sig, frame):
    print("You pressed Ctrl+C")
    exit_script()

signal.signal(signal.SIGINT, signal_handler)


def exit_script():
    print("Exiting...", end="")
    should_exit.set()
    if thread:
        thread.join()
    print("done.")
    sys.exit(0)


def listen_to_speed(telemetry, should_exit):
    """Just poll ground speed and print it."""
    while not should_exit.is_set():
        speed = telemetry.ground_speed_ned()
        print("horizontal: {}, vertical: {}".format(
            math.sqrt(speed.velocity_north_m_s**2 +
                      speed.velocity_north_m_s**2),
            -speed.velocity_down_m_s))
        time.sleep(1)


def main():
    """Connect, takeoff, and land again."""
    dc = pydronecode_sdk.DronecodeSDK()

    dc.add_udp_connection(14540)
    # We need to wait to make sure a system connects in the meantime.
    time.sleep(2)

    if len(dc.system_uuids()) == 0:
        print("No system found")
        sys.exit(1)

    if len(dc.system_uuids()) >= 2:
        print("More than one system found")
        sys.exit(1)

    action = pydronecode_sdk.Action(dc.system())
    mission = pydronecode_sdk.Mission(dc.system())
    telemetry = pydronecode_sdk.Telemetry(dc.system())

    thread = threading.Thread(target=listen_to_speed,
                         args=(telemetry, should_exit))
    thread.start()

    mission_items = []

    new_item = pydronecode_sdk.MissionItem()
    new_item.set_position(47.39821545, 8.54562609)
    new_item.set_relative_altitude(5)
    mission_items.append(new_item)

    new_item = pydronecode_sdk.MissionItem()
    new_item.set_position(47.39829534, 8.54488580)
    mission_items.append(new_item)

    new_item = pydronecode_sdk.MissionItem()
    new_item.set_position(47.39788139, 8.54488579)
    mission_items.append(new_item)

    new_item = pydronecode_sdk.MissionItem()
    new_item.set_position(47.39786685, 8.54559389)
    new_item.set_relative_altitude(2)
    mission_items.append(new_item)

    result = mission.upload_mission(mission_items)
    if result != pydronecode_sdk.Mission_Result.SUCCESS:
        print("Mission upload failed")

    print("Arming...", end="")
    action.arm()
    print("done.")

    print("Starting mission...", end="")
    mission.start_mission()
    print("done.")

    # Let it do it's mission.
    while mission.current_mission_item() < mission.total_mission_items():
        time.sleep(1)
        print("Waiting for mission to complete...")
    print("Mission completed.")

    print("Return to launch...", end="")
    action.return_to_launch()
    print("done.")

    exit_script()


if __name__ == '__main__':
    main()
