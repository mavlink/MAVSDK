#pragma once

#include <string_view>

#include "mavsdk_export.hpp"

/**
 * @brief Namespace for all mavsdk types.
 */
namespace mavsdk {

/**
 * @brief Result type returned when adding a connection.
 *
 * **Note**: Mavsdk does not throw exceptions. Instead a result of this type
 *           will be returned.
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
 * @brief Convert `ConnectionResult` to string.
 *
 * @return A string representation of the enum.
 */
MAVSDK_PUBLIC std::string_view to_string(const ConnectionResult& result);

/**
 * @brief Stream operator to print information about a `ConnectionResult`.
 *
 * @return A reference to the stream.
 */
MAVSDK_PUBLIC std::ostream& operator<<(std::ostream& str, const ConnectionResult& result);

} // namespace mavsdk
