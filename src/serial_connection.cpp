#include "serial_connection.h"
#include "global_include.h"

namespace dronelink {

SerialConnection::SerialConnection(DroneLinkImpl *parent, const std::string &path, int baudrate) :
    Connection(parent)
{
    UNUSED(path);
    UNUSED(baudrate);
}

bool SerialConnection::is_ok() const
{
    return true;
}

DroneLink::ConnectionResult SerialConnection::start()
{
    return DroneLink::ConnectionResult::SUCCESS;
}

DroneLink::ConnectionResult SerialConnection::stop()
{
    return DroneLink::ConnectionResult::SUCCESS;
}

bool SerialConnection::send_message(const mavlink_message_t &message)
{
    UNUSED(message);
    Debug() << "Not implemented";
    return false;
}


} // namespace dronelink
