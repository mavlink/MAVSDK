#pragma once

#include "dronecore.h"
#include "mavlink_receiver.h"
#include <memory>

namespace dronecore {

class DroneCoreImpl;

class Connection
{
public:
    Connection(DroneCoreImpl &parent);
    virtual ~Connection();

    virtual ConnectionResult start() = 0;
    virtual ConnectionResult stop() = 0;
    virtual bool is_ok() const = 0;

    virtual bool send_message(const mavlink_message_t &message,
                              uint8_t target_sysid = 0,
                              uint8_t target_compid = MAV_COMP_ID_ALL) = 0;

    // Non-copyable
    Connection(const Connection &) = delete;
    const Connection &operator=(const Connection &) = delete;

protected:
    bool start_mavlink_receiver();
    void stop_mavlink_receiver();
    void receive_message(const mavlink_message_t &message);
    DroneCoreImpl &_parent;
    std::unique_ptr<MAVLinkReceiver> _mavlink_receiver;

    //void received_mavlink_message(mavlink_message_t &);
};

} // namespace dronecore
