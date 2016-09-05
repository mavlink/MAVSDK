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
    Result start();
    Result stop();

    Result send_message(const mavlink_message_t &message);

private:
};

} // namespace dronelink
