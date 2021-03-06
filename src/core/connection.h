#pragma once

#include "mavsdk.h"
#include "mavlink_receiver.h"
#include <memory>

namespace mavsdk {

class Connection {
public:
    typedef std::function<void(mavlink_message_t& message, Connection* connection)>
        receiver_callback_t;

    Connection(
        receiver_callback_t receiver_callback,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);
    virtual ~Connection();

    virtual ConnectionResult start() = 0;
    virtual ConnectionResult stop() = 0;

    virtual bool send_message(const mavlink_message_t& message) = 0;

    bool should_forward_messages() const;
    unsigned forwarding_connections_count() const;

    // Non-copyable
    Connection(const Connection&) = delete;
    const Connection& operator=(const Connection&) = delete;

protected:
    bool start_mavlink_receiver();
    void stop_mavlink_receiver();
    void receive_message(mavlink_message_t& message, Connection* connection);

    receiver_callback_t _receiver_callback{};
    std::unique_ptr<MAVLinkReceiver> _mavlink_receiver;
    ForwardingOption _forwarding_option;

    static std::atomic<unsigned> _forwarding_connections_count;

    // void received_mavlink_message(mavlink_message_t &);
};

} // namespace mavsdk
