#include "tcp_connection.h"
#include "global_include.h"

namespace dronelink {

TcpConnection::TcpConnection(DroneLinkImpl *parent, const std::string &ip, int port_number) :
    Connection(parent)
{
    UNUSED(ip);
    UNUSED(port_number);
}

bool TcpConnection::is_ok() const
{
    return true;
}

DroneLink::ConnectionResult TcpConnection::start()
{
    return DroneLink::ConnectionResult::SUCCESS;
}

DroneLink::ConnectionResult TcpConnection::stop()
{
    return DroneLink::ConnectionResult::SUCCESS;
}

bool TcpConnection::send_message(const mavlink_message_t &message)
{
    UNUSED(message);
    Debug() << "Not implemented";
    return false;
}


} // namespace dronelink
