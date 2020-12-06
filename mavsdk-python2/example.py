#!/usr/bin/env python

import mavsdk2
import time

m = mavsdk2.Mavsdk()

m.add_any_connection("udp://")

time.sleep(2)

action = mavsdk2.Action(m.systems()[0])
action.arm()
action.takeoff()

telemetry = mavsdk2.Telemetry(m.systems()[0])

for _ in range(100):
    print(telemetry.position().relative_altitude_m)
    time.sleep(0.1)

action.land()

for _ in range(100):
    print(telemetry.position().relative_altitude_m)
    time.sleep(0.1)
