"""Type definitions for cmavsdk structures"""

import ctypes


class ConnectionError(ctypes.Structure):
    """Connection error information"""

    _fields_ = [
        ("error_description", ctypes.c_char_p),
        ("connection_handle", ctypes.c_void_p),
    ]


class MavsdkMessage(ctypes.Structure):
    """MAVLink message representation"""

    _fields_ = [
        ("message_name", ctypes.c_char_p),
        ("system_id", ctypes.c_uint32),
        ("component_id", ctypes.c_uint32),
        ("target_system_id", ctypes.c_uint32),
        ("target_component_id", ctypes.c_uint32),
        ("fields_json", ctypes.c_char_p),
    ]


class ConnectionResultWithHandle(ctypes.Structure):
    """Connection result with handle"""

    _fields_ = [
        ("result", ctypes.c_int),  # mavsdk_connection_result_t
        ("handle", ctypes.c_void_p),
    ]


# Callback type definitions
ConnectionErrorCallback = ctypes.CFUNCTYPE(
    None, ctypes.POINTER(ConnectionError), ctypes.c_void_p
)

NewSystemCallback = ctypes.CFUNCTYPE(None, ctypes.c_void_p)

InterceptJsonCallback = ctypes.CFUNCTYPE(ctypes.c_int, MavsdkMessage, ctypes.c_void_p)

ComponentDiscoveredCallback = ctypes.CFUNCTYPE(
    None,
    ctypes.c_int,  # mavsdk_component_type_t
    ctypes.c_void_p,
)

ComponentDiscoveredIdCallback = ctypes.CFUNCTYPE(
    None,
    ctypes.c_int,  # mavsdk_component_type_t
    ctypes.c_uint8,
    ctypes.c_void_p,
)
