from enum import IntEnum


class Autopilot(IntEnum):
    """Autopilot types"""

    UNKNOWN = 0
    PX4 = 1
    ARDUPILOT = 2
    GENERIC = 3
