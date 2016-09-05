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

Result TcpConnection::start()
{
    return Result::SUCCESS;
}

Result TcpConnection::stop()
{
    return Result::SUCCESS;
}

Result TcpConnection::send_message(const mavlink_message_t &message)
{
    UNUSED(message);
    return Result::NOT_IMPLEMENTED;
}


} // namespace dronelink
