from enum import IntEnum


class Vehicle(IntEnum):
    """Vehicle types"""

    UNKNOWN = 0
    GENERIC = 1
    FIXED_WING = 2
    MULTICOPTER = 3
    ROVER = 4
    SUBMARINE = 5
