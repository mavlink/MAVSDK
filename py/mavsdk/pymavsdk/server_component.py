import ctypes

from .cmavsdk_loader import _cmavsdk_lib


class ServerComponent:
    """Wrapper for mavsdk_server_component_t"""

    def __init__(self, lib: ctypes.CDLL, handle: ctypes.c_void_p):
        self._lib = lib
        self._handle = handle


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
