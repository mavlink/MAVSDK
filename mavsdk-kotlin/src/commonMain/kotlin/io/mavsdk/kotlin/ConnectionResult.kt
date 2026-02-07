package io.mavsdk.kotlin

/**
 * Connection result codes
 */
enum class ConnectionResult(val value: Int) {
    SUCCESS(0),
    TIMEOUT(1),
    SOCKET_ERROR(2),
    BIND_ERROR(3),
    SOCKET_CONNECTION_ERROR(4),
    CONNECTION_ERROR(5),
    NOT_IMPLEMENTED(6),
    SYSTEM_NOT_CONNECTED(7),
    SYSTEM_BUSY(8),
    COMMAND_DENIED(9),
    DESTINATION_IP_UNKNOWN(10),
    CONNECTIONS_EXHAUSTED(11),
    CONNECTION_URL_INVALID(12),
    BAUDRATE_UNKNOWN(13);

    companion object {
        fun fromValue(value: Int): ConnectionResult =
            entries.find { it.value == value } ?: CONNECTION_ERROR
    }
}
