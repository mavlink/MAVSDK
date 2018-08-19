#!/usr/bin/env python

import pydronecode_sdk
import time


def on_discover(uuid):
    print("Found {}".format(uuid))


def on_takeoff_result(result):
    print("Got takeoff callback with result {}".format(result))


def main():
    dc = pydronecode_sdk.DronecodeSDK()

    dc.register_on_discover(on_discover)
    dc.add_udp_connection(14540)
    time.sleep(2)

    action = pydronecode_sdk.Action(dc.system())
    action.arm()
    action.takeoff_async(on_takeoff_result)
    time.sleep(5)

    action.land()


if __name__ == '__main__':
    main()
