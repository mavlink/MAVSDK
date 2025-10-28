"""Custom exceptions for pymavsdk"""


class CMAVSDKError(Exception):
    """Base exception for cmavsdk wrapper errors"""
    pass


class LibraryNotFoundError(CMAVSDKError):
    """Raised when the cmavsdk library cannot be found"""
    pass


class ConnectionError(CMAVSDKError):
    """Raised when a connection operation fails"""
    pass

