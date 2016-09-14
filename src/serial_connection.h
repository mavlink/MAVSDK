#pragma once

#include "dronelink.h"
#include "dronelink_impl.h"
#include "connection.h"

namespace dronelink {

class SerialConnection : public Connection
{
public:
    explicit SerialConnection(DroneLinkImpl *parent, const std::string &path, int baudrate);
    bool is_ok() const;
    DroneLink::ConnectionResult start();
    DroneLink::ConnectionResult stop();

    bool send_message(const mavlink_message_t &message);

private:
};

} // namespace dronelink
