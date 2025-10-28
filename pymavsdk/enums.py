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


class ConnectionResult(IntEnum):
    """Connection result codes"""
    SUCCESS = 0
    TIMEOUT = 1
    SOCKET_ERROR = 2
    BIND_ERROR = 3
    SOCKET_CONNECTION_ERROR = 4
    CONNECTION_ERROR = 5
    NOT_IMPLEMENTED = 6
    SYSTEM_NOT_CONNECTED = 7
    SYSTEM_BUSY = 8
    COMMAND_DENIED = 9
    DESTINATION_IP_UNKNOWN = 10
    CONNECTIONS_EXHAUSTED = 11
    CONNECTION_URL_INVALID = 12
    BAUDRATE_UNKNOWN = 13


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


class LogLevel(IntEnum):
    """Log levels"""
    DEBUG = 0
    INFO = 1
    WARN = 2
    ERROR = 3
