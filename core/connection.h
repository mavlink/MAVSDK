#pragma once

#include "dronecore.h"
#include "mavlink_receiver.h"
#include <memory>

namespace dronecore {

class Connection {
public:
    typedef std::function<void(const mavlink_message_t &message)> receiver_callback_t;

    Connection(receiver_callback_t receiver_callback);
    virtual ~Connection();

    virtual ConnectionResult start() = 0;
    virtual ConnectionResult stop() = 0;

    virtual bool send_message(const mavlink_message_t &message) = 0;

    // Non-copyable
    Connection(const Connection &) = delete;
    const Connection &operator=(const Connection &) = delete;

protected:
    bool start_mavlink_receiver();
    void stop_mavlink_receiver();
    void receive_message(const mavlink_message_t &message);

    receiver_callback_t _receiver_callback{};
    std::unique_ptr<MAVLinkReceiver> _mavlink_receiver;

    // void received_mavlink_message(mavlink_message_t &);
};

} // namespace dronecore
