#!/usr/bin/env python

import dronecore
import time


def on_discover(uuid):
    print("Found {}".format(uuid))


def main():
    dc = dronecore.DroneCore()

    dc.register_on_discover(on_discover)

    dc.add_udp_connection()
    time.sleep(5)


if __name__ == '__main__':
    main()
