"""
Helpful errors for code written against the old gRPC-based ``mavsdk`` package.

Up to 3.x, ``pip install mavsdk`` installed MAVSDK-Python, a gRPC wrapper with a
different API. That wrapper now lives on as ``mavsdk-grpc`` (imported as
``mavsdk_grpc``). Code written for it fails against this package, so instead of a
bare ``ModuleNotFoundError`` or ``TypeError`` we point at the way forward.
"""

import importlib.abc
import sys

MIGRATION_URL = "https://mavsdk.mavlink.io/main/en/python/migration.html"

MIGRATION_HINT = (
    "Since v4, the 'mavsdk' package is the native MAVSDK binding, which has a "
    "different API from the gRPC-based MAVSDK-Python (mavsdk<4).\n"
    "  To keep the old API, install 'mavsdk-grpc' and use "
    "'import mavsdk_grpc as mavsdk'.\n"
    "  To stay on the old package without changes, pin 'mavsdk<4'.\n"
    f"  To migrate to the native binding, see {MIGRATION_URL}"
)

# Top-level modules of the gRPC wrapper that do not exist in this package.
# Its "system" module is not listed: this package has one too, and old code
# using it is caught by the check in System.__init__ instead.
_GRPC_PLUGINS = (
    "action",
    "action_server",
    "arm_authorizer_server",
    "calibration",
    "camera",
    "camera_server",
    "component_metadata",
    "component_metadata_server",
    "core",
    "events",
    "failure",
    "follow_me",
    "ftp",
    "ftp_server",
    "geofence",
    "gimbal",
    "gripper",
    "info",
    "log_files",
    "log_streaming",
    "manual_control",
    "mavlink_direct",
    "mission",
    "mission_raw",
    "mission_raw_server",
    "mocap",
    "offboard",
    "param",
    "param_server",
    "rtk",
    "server_utility",
    "shell",
    "telemetry",
    "telemetry_server",
    "tracking_server",
    "transponder",
    "tune",
    "winch",
)

_GRPC_MODULES = frozenset(
    ["async_plugin_manager", "mavsdk_options_pb2", "mavsdk_options_pb2_grpc"]
    + list(_GRPC_PLUGINS)
    + [f"{name}_pb2" for name in _GRPC_PLUGINS]
    + [f"{name}_pb2_grpc" for name in _GRPC_PLUGINS]
)


class LegacyModuleError(ImportError):
    """Raised when importing a module that only existed in the gRPC wrapper.

    Not a ModuleNotFoundError, because `from mavsdk import action` would swallow
    that and replace it with a generic "cannot import name" error.
    """


def legacy_module_error(fullname):
    return LegacyModuleError(
        f"No module named '{fullname}'. {MIGRATION_HINT}", name=fullname
    )


def check_system_args(valid, kwargs):
    """Reject `System()` as called by code written for the gRPC wrapper."""
    if kwargs or not valid:
        raise TypeError(
            "System is not constructed directly, get it from Mavsdk instead. "
            + MIGRATION_HINT
        )


class _LegacyModuleFinder(importlib.abc.MetaPathFinder):
    def __init__(self, package):
        self.package = package
        self._prefix = package + "."

    def find_spec(self, fullname, path=None, target=None):
        if not fullname.startswith(self._prefix):
            return None
        if fullname[len(self._prefix) :] not in _GRPC_MODULES:
            return None
        raise legacy_module_error(fullname)


def install(package):
    """Report imports of old gRPC wrapper modules from ``package``."""
    if any(
        isinstance(finder, _LegacyModuleFinder) and finder.package == package
        for finder in sys.meta_path
    ):
        return
    # Appended, so that a real module of the same name always wins.
    sys.meta_path.append(_LegacyModuleFinder(package))

