#include "connection_result.h"
#include <ostream>

namespace mavsdk {

MAVSDK_PUBLIC std::string_view to_string(const ConnectionResult& result)
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

MAVSDK_PUBLIC std::ostream& operator<<(std::ostream& str, const ConnectionResult& result)
{
    return str << to_string(result);
}

} // namespace mavsdk
