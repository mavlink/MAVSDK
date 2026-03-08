import ctypes

from typing import Any, Callable, List

from .cmavsdk_loader import _cmavsdk_lib


class System:
    """Wrapper for mavsdk_system_t"""

    def __init__(self, lib: ctypes.CDLL, handle: ctypes.c_void_p):
        self._lib = lib
        self._handle = handle
        self._callbacks = []  # Keep references to prevent GC

    def has_autopilot(self) -> bool:
        """Check if system has autopilot"""
        return self._lib.mavsdk_system_has_autopilot(self._handle)

    def is_standalone(self) -> bool:
        """Check if system is standalone"""
        return self._lib.mavsdk_system_is_standalone(self._handle)

    def has_camera(self, camera_id: int = -1) -> bool:
        """Check if system has camera"""
        return self._lib.mavsdk_system_has_camera(self._handle, camera_id)

    def has_gimbal(self) -> bool:
        """Check if system has gimbal"""
        return self._lib.mavsdk_system_has_gimbal(self._handle)

    def is_connected(self) -> bool:
        """Check if system is connected"""
        return self._lib.mavsdk_system_is_connected(self._handle)

    def get_system_id(self) -> int:
        """Get system ID"""
        return self._lib.mavsdk_system_get_system_id(self._handle)

    def component_ids(self) -> List[int]:
        """Get list of component IDs"""
        count = ctypes.c_size_t()
        ids_ptr = self._lib.mavsdk_system_component_ids(
            self._handle, ctypes.byref(count)
        )

        if not ids_ptr:
            return []

        ids = [ids_ptr[i] for i in range(count.value)]
        self._lib.mavsdk_system_free_component_ids(ids_ptr)
        return ids

    def subscribe_is_connected(
        self, callback: Callable[[bool, Any], None], user_data: Any = None
    ):
        """Subscribe to connection state changes"""
        c_callback = IsConnectedCallback(
            lambda connected, ud: callback(connected, user_data)
        )
        self._callbacks.append(c_callback)


IsConnectedCallback = ctypes.CFUNCTYPE(None, ctypes.c_bool, ctypes.c_void_p)

_cmavsdk_lib.mavsdk_system_has_autopilot.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_has_autopilot.restype = ctypes.c_bool

_cmavsdk_lib.mavsdk_system_is_standalone.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_is_standalone.restype = ctypes.c_bool

_cmavsdk_lib.mavsdk_system_has_camera.argtypes = [ctypes.c_void_p, ctypes.c_int]
_cmavsdk_lib.mavsdk_system_has_camera.restype = ctypes.c_bool

_cmavsdk_lib.mavsdk_system_has_gimbal.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_has_gimbal.restype = ctypes.c_bool

_cmavsdk_lib.mavsdk_system_is_connected.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_is_connected.restype = ctypes.c_bool

_cmavsdk_lib.mavsdk_system_get_system_id.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_get_system_id.restype = ctypes.c_uint8

_cmavsdk_lib.mavsdk_system_component_ids.argtypes = [
    ctypes.c_void_p,
    ctypes.POINTER(ctypes.c_size_t),
]
_cmavsdk_lib.mavsdk_system_component_ids.restype = ctypes.POINTER(ctypes.c_uint8)

_cmavsdk_lib.mavsdk_system_free_component_ids.argtypes = [
    ctypes.POINTER(ctypes.c_uint8)
]
_cmavsdk_lib.mavsdk_system_free_component_ids.restype = None

_cmavsdk_lib.mavsdk_system_subscribe_is_connected.argtypes = [
    ctypes.c_void_p,
    IsConnectedCallback,
    ctypes.c_void_p,
]
_cmavsdk_lib.mavsdk_system_subscribe_is_connected.restype = ctypes.c_void_p

_cmavsdk_lib.mavsdk_system_unsubscribe_is_connected.argtypes = [
    ctypes.c_void_p,
    ctypes.c_void_p,
]
_cmavsdk_lib.mavsdk_system_unsubscribe_is_connected.restype = None

_cmavsdk_lib.mavsdk_system_enable_timesync.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_enable_timesync.restype = None

_cmavsdk_lib.mavsdk_system_autopilot_type.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_autopilot_type.restype = ctypes.c_int

_cmavsdk_lib.mavsdk_system_vehicle_type.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_vehicle_type.restype = ctypes.c_int
