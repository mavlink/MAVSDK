#include "tcp_connection.h"
#include "global_include.h"

namespace dronecore {

TcpConnection::TcpConnection(DroneCoreImpl *parent, const std::string &ip, int port_number) :
    Connection(parent)
{
    UNUSED(ip);
    UNUSED(port_number);
}

bool TcpConnection::is_ok() const
{
    return true;
}

DroneCore::ConnectionResult TcpConnection::start()
{
    return DroneCore::ConnectionResult::SUCCESS;
}

DroneCore::ConnectionResult TcpConnection::stop()
{
    return DroneCore::ConnectionResult::SUCCESS;
}

bool TcpConnection::send_message(const mavlink_message_t &message)
{
    UNUSED(message);
    Debug() << "Not implemented";
    return false;
}


} // namespace dronecore
