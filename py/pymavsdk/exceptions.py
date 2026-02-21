"""Custom exceptions for pymavsdk"""


class MavsdkError(Exception):
    """Base exception for pymavsdk errors"""

    pass


class LibraryNotFoundError(MavsdkError):
    """Raised when the cmavsdk library cannot be found"""

    pass


class ConnectionError(MavsdkError):
    """Raised when a connection operation fails"""

    pass
