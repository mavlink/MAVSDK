import ctypes

from .cmavsdk_loader import _cmavsdk_lib


class ServerComponent:
    """Wrapper for mavsdk_server_component_t

    Like :class:`~mavsdk.system.System`, each instance owns its handle: the
    ``mavsdk_server_component*`` family allocates a fresh ``shared_ptr`` per call,
    even when the same underlying component is returned. Instances are registered
    with the owning Mavsdk so they are released before it destroys itself.
    """

    def __init__(self, lib: ctypes.CDLL, handle: ctypes.c_void_p):
        self._lib = lib
        self._handle = handle

    def destroy(self) -> None:
        """Release the underlying server component handle. Idempotent."""
        if self._handle:
            self._lib.mavsdk_server_component_destroy(self._handle)
            self._handle = None

    def __del__(self):
        self.destroy()

    def _require_handle(self) -> ctypes.c_void_p:
        if not self._handle:
            raise RuntimeError(
                "ServerComponent has been destroyed (its Mavsdk was destroyed, "
                "or destroy() was called explicitly)"
            )
        return self._handle


_cmavsdk_lib.mavsdk_server_component_destroy.argtypes = [ctypes.c_void_p]
_cmavsdk_lib.mavsdk_server_component_destroy.restype = None

_cmavsdk_lib.mavsdk_server_component.argtypes = [ctypes.c_void_p, ctypes.c_uint]
_cmavsdk_lib.mavsdk_server_component.restype = ctypes.c_void_p

_cmavsdk_lib.mavsdk_server_component_by_type.argtypes = [
    ctypes.c_void_p,
    ctypes.c_int,
    ctypes.c_uint,
]
_cmavsdk_lib.mavsdk_server_component_by_type.restype = ctypes.c_void_p

_cmavsdk_lib.mavsdk_server_component_by_id.argtypes = [ctypes.c_void_p, ctypes.c_uint8]
_cmavsdk_lib.mavsdk_server_component_by_id.restype = ctypes.c_void_p
