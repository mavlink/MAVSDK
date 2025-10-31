"""Enumerations from cmavsdk"""

from enum import IntEnum


class ComponentType(IntEnum):
    """MAVLink component types"""
    AUTOPILOT = 0
    GROUND_STATION = 1
    COMPANION_COMPUTER = 2
    CAMERA = 3
    GIMBAL = 4
    REMOTE_ID = 5
    CUSTOM = 6


class ForwardingOption(IntEnum):
    """Message forwarding options"""
    OFF = 0
    ON = 1


class Autopilot(IntEnum):
    """Autopilot types"""
    UNKNOWN = 0
    PX4 = 1
    ARDUPILOT = 2
    GENERIC = 3


class Vehicle(IntEnum):
    """Vehicle types"""
    UNKNOWN = 0
    GENERIC = 1
    FIXED_WING = 2
    MULTICOPTER = 3
    ROVER = 4
    SUBMARINE = 5

