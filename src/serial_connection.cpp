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

Result SerialConnection::start()
{
    return Result::SUCCESS;
}

Result SerialConnection::stop()
{
    return Result::SUCCESS;
}

Result SerialConnection::send_message(const mavlink_message_t &message)
{
    UNUSED(message);
    return Result::NOT_IMPLEMENTED;
}


} // namespace dronelink
