#pragma once

#include "dronecore.h"
#include "dronecore_impl.h"
#include "connection.h"

namespace dronecore {

class SerialConnection : public Connection
{
public:
    explicit SerialConnection(DroneCoreImpl *parent, const std::string &path, int baudrate);
    bool is_ok() const;
    DroneCore::ConnectionResult start();
    DroneCore::ConnectionResult stop();

    bool send_message(const mavlink_message_t &message);

private:
};

} // namespace dronecore
