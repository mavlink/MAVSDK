#pragma once

#include "dronelink.h"
#include "mavlink_receiver.h"
#include <memory>

namespace dronelink {

class DroneLinkImpl;

class Connection
{
public:
    Connection(DroneLinkImpl *parent);
    virtual ~Connection();

    virtual DroneLink::ConnectionResult start() = 0;
    virtual DroneLink::ConnectionResult stop() = 0;
    virtual bool is_ok() const = 0;

    virtual bool send_message(const mavlink_message_t &message) = 0;

    // Non-copyable
    Connection(const Connection &) = delete;
    const Connection &operator=(const Connection &) = delete;

protected:
    bool start_mavlink_receiver();
    void stop_mavlink_receiver();
    void receive_message(const mavlink_message_t &message);
    DroneLinkImpl *_parent;
    std::unique_ptr<MavlinkReceiver> _mavlink_receiver;

    //void received_mavlink_message(mavlink_message_t &);
};

} // namespace dronelink
