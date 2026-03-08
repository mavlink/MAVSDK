"""Logging support for cmavsdk"""

import ctypes

from enum import IntEnum
from typing import Optional, Callable

from .cmavsdk_loader import _cmavsdk_lib


class LogLevel(IntEnum):
    """Log levels"""

    DEBUG = 0
    INFO = 1
    WARN = 2
    ERROR = 3


LogCallback = ctypes.CFUNCTYPE(
    ctypes.c_bool,
    ctypes.c_int,  # mavsdk_log_level_t
    ctypes.c_char_p,  # message
    ctypes.c_char_p,  # file
    ctypes.c_int,  # line
    ctypes.c_void_p,  # user_data
)

# Global reference to keep callback alive
_log_callback_ref: Optional[LogCallback] = None


def log_subscribe(callback: Callable[[LogLevel, str, Optional[str], int], bool]):
    """
    Subscribe to MAVSDK log messages

    Args:
        callback: Function that takes (level, message, file, line) and returns bool.
                  Return True to prevent default logging, False to keep it.

    Example:
        def my_log_handler(level, message, file, line):
            print(f"[{level.name}] {message}")
            if file:
                print(f"  at {file}:{line}")
            return True  # Suppress default logging

        log_subscribe(my_log_handler)
    """
    global _log_callback_ref

    lib = _cmavsdk_lib

    def c_callback(level: int, message: bytes, file: bytes, line: int, user_data):
        try:
            level_enum = LogLevel(level)
            msg_str = message.decode("utf-8") if message else ""
            file_str = file.decode("utf-8") if file else None

            return callback(level_enum, msg_str, file_str, line)
        except Exception as e:
            print(f"Error in log callback: {e}")
            return False

    # Keep reference to prevent garbage collection
    _log_callback_ref = LogCallback(c_callback)
    lib.mavsdk_log_subscribe(_log_callback_ref, None)


def log_unsubscribe():
    """Unsubscribe from log messages"""
    global _log_callback_ref

    lib = _cmavsdk_lib
    lib.mavsdk_log_unsubscribe()
    _log_callback_ref = None


_cmavsdk_lib.mavsdk_log_subscribe.argtypes = [LogCallback, ctypes.c_void_p]
_cmavsdk_lib.mavsdk_log_subscribe.restype = None

_cmavsdk_lib.mavsdk_log_unsubscribe.argtypes = []
_cmavsdk_lib.mavsdk_log_unsubscribe.restype = None
