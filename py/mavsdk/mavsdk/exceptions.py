"""Custom exceptions for mavsdk"""


class MavsdkError(Exception):
    """Base exception for mavsdk errors"""

    pass


class LibraryNotFoundError(MavsdkError):
    """Raised when the cmavsdk library cannot be found"""

    pass


class ConnectionError(MavsdkError):
    """Raised when a connection operation fails"""

    pass
