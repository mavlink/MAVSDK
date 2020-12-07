#!/usr/bin/env python

import mavsdk2
import time

m = mavsdk2.Mavsdk()

m.add_any_connection("udp://")

while len(m.systems()) == 0:
    print("Waiting for system to connect")
    time.sleep(1)

action = mavsdk2.Action(m.systems()[0])

print("Arming...")
action.arm()
print("Taking off...")
action.takeoff()

telemetry = mavsdk2.Telemetry(m.systems()[0])

for _ in range(10):
    print("Altitude: {} m".format(telemetry.position().relative_altitude_m))
    time.sleep(1.0)

print("Landing...")
action.land()

for _ in range(10):
    print("Altitude: {} m".format(telemetry.position().relative_altitude_m))
    time.sleep(1.0)

while telemetry.armed():
    print("Waiting until system is disarmed")
    time.sleep(1.0)
