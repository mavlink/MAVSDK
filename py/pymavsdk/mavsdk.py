"""Main ctypes wrapper for cmavsdk library"""

import ctypes

from typing import Optional, List, Callable, Any

from .cmavsdk_loader import _cmavsdk_lib
from .connection_result import ConnectionResult
from .component_type import ComponentType
from .exceptions import ConnectionError
from .server_component import ServerComponent
from .system import System
from .types import (
    ConnectionResultWithHandle,
    NewSystemCallback,
)


class Configuration:
    """Wrapper for mavsdk_configuration_t"""

    def __init__(self, handle: ctypes.c_void_p):
        self._lib = _cmavsdk_lib
        self._handle = handle

    @classmethod
    def create_with_component_type(cls, component_type: ComponentType):
        """Create configuration with component type"""
        handle = _cmavsdk_lib.mavsdk_configuration_create_with_component_type(
            int(component_type)
        )
        return cls(handle)

    @classmethod
    def create_manual(
        cls, system_id: int, component_id: int, always_send_heartbeats: bool
    ):
        """Create manual configuration"""
        handle = _cmavsdk_lib.mavsdk_configuration_create_manual(
            system_id, component_id, int(always_send_heartbeats)
        )
        return cls(handle)

    def destroy(self):
        """Destroy configuration"""
        if self._handle:
            self._lib.mavsdk_configuration_destroy(self._handle)
            self._handle = None

    @property
    def system_id(self) -> int:
        return self._lib.mavsdk_configuration_get_system_id(self._handle)

    @system_id.setter
    def system_id(self, value: int):
        self._lib.mavsdk_configuration_set_system_id(self._handle, value)

    @property
    def component_id(self) -> int:
        return self._lib.mavsdk_configuration_get_component_id(self._handle)

    @component_id.setter
    def component_id(self, value: int):
        self._lib.mavsdk_configuration_set_component_id(self._handle, value)

    def __del__(self):
        self.destroy()


class Mavsdk:
    """Python wrapper for cmavsdk library"""

    def __init__(self, configuration: Configuration):
        """
        Initialize the cmavsdk wrapper

        Args:
            configuration: Configuration
        """
        self._lib = _cmavsdk_lib

        self._callbacks = {}  # Keep references to prevent GC: { handle: callback }

        self._handle = self._lib.mavsdk_create(configuration._handle)
        self._destroyed = False

    def version(self) -> str:
        """Get MAVSDK version string"""
        version_bytes = self._lib.mavsdk_version(self._handle)
        return version_bytes.decode("utf-8") if version_bytes else ""

    def add_any_connection(self, connection_url: str) -> ConnectionResult:
        """Add a connection"""
        result = self._lib.mavsdk_add_any_connection(
            self._handle, connection_url.encode("utf-8")
        )
        return ConnectionResult(result)

    def add_any_connection_with_handle(self, connection_url: str):
        """Add a connection and get handle"""
        result = self._lib.mavsdk_add_any_connection_with_handle(
            self._handle, connection_url.encode("utf-8")
        )
        if result.result != ConnectionResult.SUCCESS:
            raise ConnectionError(
                f"Connection failed: {ConnectionResult(result.result).name}"
            )
        return result.handle

    def remove_connection(self, handle: ctypes.c_void_p):
        """Remove a connection"""
        self._lib.mavsdk_remove_connection(self._handle, handle)

    def system_count(self) -> int:
        """Get number of discovered systems"""
        return self._lib.mavsdk_system_count(self._handle)

    def get_systems(self) -> List[System]:
        """Get all discovered systems"""
        count = ctypes.c_size_t()
        systems_ptr = self._lib.mavsdk_get_systems(self._handle, ctypes.byref(count))

        if not systems_ptr:
            return []

        systems = [System(self._lib, systems_ptr[i]) for i in range(count.value)]

        self._lib.mavsdk_free_systems_array(systems_ptr)
        return systems

    def first_autopilot(self, timeout_s: float = 3.0) -> Optional[System]:
        """Wait for and return first autopilot system"""
        handle = self._lib.mavsdk_first_autopilot(self._handle, timeout_s)
        if handle:
            return System(self._lib, handle)
        return None

    def subscribe_on_new_system(
        self, callback: Callable[[Any], None], user_data: Any = None
    ):
        """Subscribe to new system discoveries"""
        c_callback = NewSystemCallback(lambda ud: callback(user_data))

        handle = self._lib.mavsdk_subscribe_on_new_system(
            self._handle, c_callback, None
        )

        self._callbacks[handle] = c_callback

        return handle

    def unsubscribe_on_new_system(self, handle: ctypes.c_void_p):
        """Unsubscribe from new system discoveries"""
        self._lib.mavsdk_unsubscribe_on_new_system(self._handle, handle)
        self._callbacks.pop(handle, None)

    def server_component(self, instance: int = 0):
        """Get server component by instance"""
        handle = self._lib.mavsdk_server_component(self._handle, instance)
        if handle:
            return ServerComponent(self._lib, handle)
        return None

    def server_component_by_type(
        self, component_type: ComponentType, instance: int = 0
    ):
        """Get server component by type and instance"""
        handle = self._lib.mavsdk_server_component_by_type(
            self._handle, int(component_type), instance
        )
        if handle:
            return ServerComponent(self._lib, handle)
        return None

    def server_component_by_id(self, component_id: int):
        """Get server component by component ID"""
        handle = self._lib.mavsdk_server_component_by_id(self._handle, component_id)
        if handle:
            return ServerComponent(self._lib, handle)
        return None

    def destroy(self):
        """Destroy the Mavsdk instance"""
        if not self._destroyed and self._handle:
            self._callbacks.clear()
            self._lib.mavsdk_destroy(self._handle)
            self._handle = None
            self._destroyed = True

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.destroy()


def __del__(self):
    try:
        import sys

        if sys is not None and hasattr(self, "_handle") and self._handle:
            import warnings

            warnings.warn(
                "Mavsdk destroyed by garbage collector. Use context manager or call destroy() explicitly.",
                ResourceWarning,
                stacklevel=2,
            )
            self.destroy()
    except (ImportError, AttributeError):
        # Python is shutting down, skip cleanup silently
        pass


# Configuration functions
_cmavsdk_lib.mavsdk_configuration_create_with_component_type.argtypes = [ctypes.c_int]
_cmavsdk_lib.mavsdk_configuration_create_with_component_type.restype = ctypes.c_void_p

_cmavsdk_lib.mavsdk_configuration_create_manual.argtypes = [
    ctypes.c_uint8,
    ctypes.c_uint8,
    ctypes.c_int,
]
_cmavsdk_lib.mavsdk_configuration_create_manual.restype = ctypes.c_void_p

_cmavsdk_lib.mavsdk_configuration_destroy.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_configuration_destroy.restype = None

_cmavsdk_lib.mavsdk_configuration_get_system_id.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_configuration_get_system_id.restype = ctypes.c_uint8

_cmavsdk_lib.mavsdk_configuration_set_system_id.argtypes = [
    ctypes.c_void_p,
    ctypes.c_uint8,
]
_cmavsdk_lib.mavsdk_configuration_set_system_id.restype = None

_cmavsdk_lib.mavsdk_configuration_get_component_id.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_configuration_get_component_id.restype = ctypes.c_uint8

_cmavsdk_lib.mavsdk_configuration_set_component_id.argtypes = [
    ctypes.c_void_p,
    ctypes.c_uint8,
]
_cmavsdk_lib.mavsdk_configuration_set_component_id.restype = None

# Core functions
_cmavsdk_lib.mavsdk_create.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_create.restype = ctypes.c_void_p

_cmavsdk_lib.mavsdk_destroy.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_destroy.restype = None

_cmavsdk_lib.mavsdk_version.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_version.restype = ctypes.c_char_p

# Connection functions
_cmavsdk_lib.mavsdk_add_any_connection.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
_cmavsdk_lib.mavsdk_add_any_connection.restype = ctypes.c_int

_cmavsdk_lib.mavsdk_add_any_connection_with_handle.argtypes = [
    ctypes.c_void_p,
    ctypes.c_char_p,
]
_cmavsdk_lib.mavsdk_add_any_connection_with_handle.restype = ConnectionResultWithHandle

_cmavsdk_lib.mavsdk_remove_connection.argtypes = [ctypes.c_void_p, ctypes.c_void_p]
_cmavsdk_lib.mavsdk_remove_connection.restype = None

# System functions
_cmavsdk_lib.mavsdk_system_count.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_count.restype = ctypes.c_size_t

_cmavsdk_lib.mavsdk_get_systems.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_size_t),
]
_cmavsdk_lib.mavsdk_get_systems.restype = ctypes.POINTER(ctypes.c_void_p)

_cmavsdk_lib.mavsdk_free_systems_array.argtypes = [ctypes.POINTER(ctypes.c_void_p)]
_cmavsdk_lib.mavsdk_free_systems_array.restype = None

_cmavsdk_lib.mavsdk_first_autopilot.argtypes = [ctypes.c_void_p, ctypes.c_double]
_cmavsdk_lib.mavsdk_first_autopilot.restype = ctypes.c_void_p

# Subscription functions
_cmavsdk_lib.mavsdk_subscribe_on_new_system.argtypes = [
    ctypes.c_void_p,
    NewSystemCallback,
    ctypes.c_void_p,
]
_cmavsdk_lib.mavsdk_subscribe_on_new_system.restype = ctypes.c_void_p

_cmavsdk_lib.mavsdk_unsubscribe_on_new_system.argtypes = [
    ctypes.c_void_p,
    ctypes.c_void_p,
]
_cmavsdk_lib.mavsdk_unsubscribe_on_new_system.restype = None
