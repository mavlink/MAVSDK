
/**
 * @brief Returns a human-readable English string for a ConnectionResult.
 *
 * @param result The enum value for which a human readable string is required.
 * @return Human readable string for the ConnectionResult.
 */
inline const char *connection_result_str(const ConnectionResult result)
{
    switch (result) {
        case ConnectionResult::SUCCESS:
            return "Success";
        case ConnectionResult::TIMEOUT:
            return "Timeout";
        case ConnectionResult::SOCKET_ERROR:
            return "Socket error";
        case ConnectionResult::BIND_ERROR:
            return "Bind error";
        case ConnectionResult::SOCKET_CONNECTION_ERROR:
            return "Socket connection error";
        case ConnectionResult::CONNECTION_ERROR:
            return "Connection error";
        case ConnectionResult::NOT_IMPLEMENTED:
            return "Not implemented";
        case ConnectionResult::SYSTEM_NOT_CONNECTED:
            return "System not connected";
        case ConnectionResult::SYSTEM_BUSY:
            return "System busy";
        case ConnectionResult::COMMAND_DENIED:
            return "Command denied";
        case ConnectionResult::DESTINATION_IP_UNKNOWN:
            return "Destination IP unknown";
        case ConnectionResult::CONNECTIONS_EXHAUSTED:
            return "Connections exhausted";
        case ConnectionResult::CONNECTION_URL_INVALID:
            return "Invalid connection URL";
        default:
            return "Unknown";
    }
}

