#pragma once

#include "dronelink.h"
#include "dronelink_impl.h"
#include "connection.h"

namespace dronelink {

class TcpConnection : public Connection
{
public:
    explicit TcpConnection(DroneLinkImpl *parent, const std::string &ip, int port_number);
    bool is_ok() const;
    DroneLink::ConnectionResult start();
    DroneLink::ConnectionResult stop();

    bool send_message(const mavlink_message_t &message);

private:
};

} // namespace dronelink
