"""
Python wrapper for cmavsdk C library using ctypes
"""

from . import _legacy

_legacy.install(__name__)

from .autopilot import Autopilot
from .connection_result import ConnectionResult
from .component_type import ComponentType
from .mavsdk import Mavsdk, Configuration
from .exceptions import MavsdkError
from .enums import (
    ForwardingOption,
)
from .logging import log_subscribe, log_unsubscribe, LogLevel
from .system import System
from .vehicle import Vehicle

try:
    from importlib.metadata import PackageNotFoundError, version as _dist_version

    __version__ = _dist_version("mavsdk")
except PackageNotFoundError:
    # Running from a source checkout rather than an installed package.
    __version__ = "0.0.0+unknown"

__all__ = [
    "Autopilot",
    "ConnectionResult",
    "ComponentType",
    "Mavsdk",
    "Configuration",
    "MavsdkError",
    "ForwardingOption",
    "log_subscribe",
    "log_unsubscribe",
    "LogLevel",
    "System",
    "Vehicle",
    "__version__",
]
