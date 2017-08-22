#!/usr/bin/env python

import pydronecore
import time


def on_discover(uuid):
    print("Found {}".format(uuid))


def on_arm_result(result):
    print("Got arm result {}".format(result))


def on_takeoff_result(result):
    print("Got takeoff result {}".format(result))


def main():
    dc = pydronecore.DroneCore()

    dc.register_on_discover(on_discover)

    dc.add_udp_connection()
    time.sleep(2)

    dc.device().action().arm()
    dc.device().action().takeoff_async(on_takeoff_result)
    time.sleep(5)
    dc.device().action().land()


if __name__ == '__main__':
    main()
