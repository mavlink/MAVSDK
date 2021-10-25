#!/usr/bin/env python3
import sys

"""
Dummy air quality sensor control script
"""

def sensor_control(signal):
    if (signal == "on"):
        print("\n - Air quality sensor turned on!\n");
    elif (signal == "off"):
        print("\n - Air quality sensor turned off!\n");

def main():
    sensor_control(sys.argv[1])


main()
