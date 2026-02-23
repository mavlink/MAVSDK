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
