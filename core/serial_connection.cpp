#include "serial_connection.h"
#include "global_include.h"

namespace dronecore {

SerialConnection::SerialConnection(DroneCoreImpl *parent, const std::string &path, int baudrate) :
    Connection(parent)
{
    UNUSED(path);
    UNUSED(baudrate);
}

bool SerialConnection::is_ok() const
{
    return true;
}

DroneCore::ConnectionResult SerialConnection::start()
{
    return DroneCore::ConnectionResult::SUCCESS;
}

DroneCore::ConnectionResult SerialConnection::stop()
{
    return DroneCore::ConnectionResult::SUCCESS;
}

bool SerialConnection::send_message(const mavlink_message_t &message)
{
    UNUSED(message);
    Debug() << "Not implemented";
    return false;
}


} // namespace dronecore
