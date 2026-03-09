"""Custom exceptions for aiomavsdk."""

from mavsdk.exceptions import MavsdkError, LibraryNotFoundError


class MavsdkConnectionError(MavsdkError):
    """Raised when a connection operation fails."""

    pass


__all__ = [
    "MavsdkError",
    "LibraryNotFoundError",
    "MavsdkConnectionError",
]
