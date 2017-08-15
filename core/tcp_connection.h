#pragma once

#include "dronecore.h"
#include "dronecore_impl.h"
#include "connection.h"

namespace dronecore {

class TcpConnection : public Connection
{
public:
    explicit TcpConnection(DroneCoreImpl *parent, const std::string &ip, int port_number);
    bool is_ok() const;
    DroneCore::ConnectionResult start();
    DroneCore::ConnectionResult stop();

    bool send_message(const mavlink_message_t &message);

private:
};

} // namespace dronecore
