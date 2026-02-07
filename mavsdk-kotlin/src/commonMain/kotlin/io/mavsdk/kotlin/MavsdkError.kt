package io.mavsdk.kotlin

/**
 * Base sealed class for all MAVSDK errors
 */
sealed class MavsdkError(message: String) : Exception(message) {
    /**
     * Library not found or failed to load
     */
    class LibraryNotFoundError(message: String) : MavsdkError(message)

    /**
     * Connection operation failed
     */
    class ConnectionError(
        val result: ConnectionResult,
        message: String = "Connection failed: ${result.name}"
    ) : MavsdkError(message)

    /**
     * Generic MAVSDK operation error
     */
    class OperationError(message: String) : MavsdkError(message)

    /**
     * Timeout during operation
     */
    class TimeoutError(message: String = "Operation timed out") : MavsdkError(message)

    /**
     * System not found or not connected
     */
    class SystemNotFoundError(message: String = "System not found") : MavsdkError(message)
}
