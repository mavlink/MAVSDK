#!/usr/bin/env python

import mavsdk2
import time

m = mavsdk2.Mavsdk()

m.add_any_connection("udp://")

while len(m.systems()) == 0:
    print("Waiting for system to connect")
    time.sleep(1)

system = m.systems()[0]

print("Arming...")
system.action().arm()
print("Taking off...")
system.action().takeoff()


for _ in range(10):
    print("Altitude: {} m"
          .format(system.telemetry().position().relative_altitude_m))
    time.sleep(1.0)

print("Landing...")
system.action().land()

for _ in range(10):
    print("Altitude: {} m"
          .format(system.telemetry().position().relative_altitude_m))
    time.sleep(1.0)

while system.telemetry().armed():
    print("Waiting until system is disarmed")
    time.sleep(1.0)
