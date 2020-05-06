#pragma once

/**
 * @brief Namespace for all mavsdk types.
 */
namespace mavsdk {

/**
 * @brief Result type returned when adding a connection.
 *
 * **Note**: Mavsdk does not throw exceptions. Instead a result of this type will be
 * returned when you add a connection: add_udp_connection().
 */
enum class ConnectionResult {
    Success = 0, /**< @brief %Connection succeeded. */
    Timeout, /**< @brief %Connection timed out. */
    SocketError, /**< @brief Socket error. */
    BindError, /**< @brief Bind error. */
    SocketConnectionError, /**< @brief Socket connection error. */
    ConnectionError, /**< @brief %Connection error. */
    NotImplemented, /**< @brief %Connection type not implemented. */
    SystemNotConnected, /**< @brief No system is connected. */
    SystemBusy, /**< @brief %System is busy. */
    CommandDenied, /**< @brief Command is denied. */
    DestinationIpUnknown, /**< @brief %Connection IP is unknown. */
    ConnectionsExhausted, /**< @brief %Connections exhausted. */
    ConnectionUrlInvalid, /**< @brief URL invalid. */
    BaudrateUnknown /**< @brief Baudrate unknown. */
};

/**
 * @brief Returns a human-readable English string for a ConnectionResult.
 *
 * @param result The enum value for which a human readable string is required.
 * @return Human readable string for the ConnectionResult.
 */
inline const char* connection_result_str(const ConnectionResult result)
{
    switch (result) {
        case ConnectionResult::Success:
            return "Success";
        case ConnectionResult::Timeout:
            return "Timeout";
        case ConnectionResult::SocketError:
            return "Socket error";
        case ConnectionResult::BindError:
            return "Bind error";
        case ConnectionResult::SocketConnectionError:
            return "Socket connection error";
        case ConnectionResult::ConnectionError:
            return "Connection error";
        case ConnectionResult::NotImplemented:
            return "Not implemented";
        case ConnectionResult::SystemNotConnected:
            return "System not connected";
        case ConnectionResult::SystemBusy:
            return "System busy";
        case ConnectionResult::CommandDenied:
            return "Command denied";
        case ConnectionResult::DestinationIpUnknown:
            return "Destination IP unknown";
        case ConnectionResult::ConnectionsExhausted:
            return "Connections exhausted";
        case ConnectionResult::ConnectionUrlInvalid:
            return "Invalid connection URL";
        case ConnectionResult::BaudrateUnknown:
            return "Baudrate unknown";
        default:
            return "Unknown";
    }
}

} // namespace mavsdk
