import ctypes

from typing import Any, Callable, List

from .autopilot import Autopilot
from .cmavsdk_loader import _cmavsdk_lib
from .vehicle import Vehicle


class System:
    """Wrapper for mavsdk_system_t

    Each instance owns its handle. ``mavsdk_get_systems`` and
    ``mavsdk_first_autopilot`` hand out a fresh ``shared_ptr<System>`` per call --
    ``mavsdk_free_systems_array`` only frees the array, not the elements -- so every
    wrapper must release exactly one handle. Instances are registered with the owning
    :class:`~mavsdk.mavsdk.Mavsdk`, which destroys any that are still alive before it
    destroys itself, because Python's garbage collector gives no ordering guarantee
    between the two.
    """

    def __init__(self, lib: ctypes.CDLL, handle: ctypes.c_void_p):
        self._lib = lib
        self._handle = handle
        # Keep references to prevent GC: { subscription handle: callback }
        self._callbacks = {}

    def destroy(self) -> None:
        """Release the underlying system handle. Idempotent."""
        if self._handle:
            # Unsubscribe before releasing the handle. Dropping our shared_ptr does
            # not destroy the C++ System -- MavsdkImpl owns it -- so any subscription
            # left active would keep firing into ctypes trampolines that are about to
            # be garbage collected.
            for subscription_handle in list(self._callbacks):
                self._lib.mavsdk_system_unsubscribe_is_connected(
                    self._handle, subscription_handle
                )
            self._callbacks.clear()

            self._lib.mavsdk_system_destroy(self._handle)
            self._handle = None

    def __del__(self):
        self.destroy()

    def _require_handle(self) -> ctypes.c_void_p:
        """Fail loudly rather than dereferencing a null handle in C.

        Before the handle was ever released this could not happen; now that it is,
        using a System after its Mavsdk is gone would segfault without this check.
        """
        if not self._handle:
            raise RuntimeError(
                "System has been destroyed (its Mavsdk was destroyed, "
                "or destroy() was called explicitly)"
            )
        return self._handle

    def has_autopilot(self) -> bool:
        """Check if system has autopilot"""
        return self._lib.mavsdk_system_has_autopilot(self._require_handle())

    def is_standalone(self) -> bool:
        """Check if system is standalone"""
        return self._lib.mavsdk_system_is_standalone(self._require_handle())

    def has_camera(self, camera_id: int = -1) -> bool:
        """Check if system has camera"""
        return self._lib.mavsdk_system_has_camera(self._require_handle(), camera_id)

    def has_gimbal(self) -> bool:
        """Check if system has gimbal"""
        return self._lib.mavsdk_system_has_gimbal(self._require_handle())

    def is_connected(self) -> bool:
        """Check if system is connected"""
        return self._lib.mavsdk_system_is_connected(self._require_handle())

    def get_system_id(self) -> int:
        """Get system ID"""
        return self._lib.mavsdk_system_get_system_id(self._require_handle())

    def component_ids(self) -> List[int]:
        """Get list of component IDs"""
        count = ctypes.c_size_t()
        ids_ptr = self._lib.mavsdk_system_component_ids(
            self._require_handle(), ctypes.byref(count)
        )

        if not ids_ptr:
            return []

        ids = [ids_ptr[i] for i in range(count.value)]
        self._lib.mavsdk_system_free_component_ids(ids_ptr)
        return ids

    def subscribe_is_connected(
        self, callback: Callable[[bool, Any], None], user_data: Any = None
    ) -> ctypes.c_void_p:
        """Subscribe to connection state changes

        Returns the subscription handle to pass to
        :meth:`unsubscribe_is_connected`.
        """
        c_callback = IsConnectedCallback(
            lambda connected, ud: callback(connected, user_data)
        )

        subscription_handle = self._lib.mavsdk_system_subscribe_is_connected(
            self._require_handle(), c_callback, None
        )

        # Held until unsubscribe: if the trampoline is collected while MAVSDK still
        # holds the pointer, the next callback jumps into freed memory.
        self._callbacks[subscription_handle] = c_callback

        return subscription_handle

    def unsubscribe_is_connected(self, handle: ctypes.c_void_p) -> None:
        """Unsubscribe from connection state changes

        A no-op if the system is already destroyed, which unsubscribed everything
        anyway. Callers unsubscribe from ``finally`` blocks that race teardown, so
        this must not raise there.
        """
        if not self._handle:
            return
        self._lib.mavsdk_system_unsubscribe_is_connected(self._handle, handle)
        self._callbacks.pop(handle, None)

    def enable_timesync(self) -> None:
        """Enable time synchronization using the TIMESYNC messages."""
        self._lib.mavsdk_system_enable_timesync(self._require_handle())

    def autopilot_type(self) -> Autopilot:
        """Get the autopilot type reported by this system"""
        return Autopilot(self._lib.mavsdk_system_autopilot_type(self._require_handle()))

    def vehicle_type(self) -> Vehicle:
        """Get the vehicle type reported by this system"""
        return Vehicle(self._lib.mavsdk_system_vehicle_type(self._require_handle()))


IsConnectedCallback = ctypes.CFUNCTYPE(None, ctypes.c_bool, ctypes.c_void_p)

_cmavsdk_lib.mavsdk_system_destroy.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_system_destroy.restype = None

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
