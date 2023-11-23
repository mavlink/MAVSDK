#pragma once

#include "mavsdk.h"
#include "mavlink_receiver.h"
#include <atomic>
#include <memory>
#include <unordered_set>

namespace mavsdk {

class Connection {
public:
    using ReceiverCallback =
        std::function<void(mavlink_message_t& message, Connection* connection)>;

    explicit Connection(
        ReceiverCallback receiver_callback,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);
    virtual ~Connection();

    virtual ConnectionResult start() = 0;
    virtual ConnectionResult stop() = 0;

    virtual bool send_message(const mavlink_message_t& message) = 0;

    bool has_system_id(uint8_t system_id);
    bool should_forward_messages() const;
    static unsigned forwarding_connections_count();

    // Non-copyable
    Connection(const Connection&) = delete;
    const Connection& operator=(const Connection&) = delete;

protected:
    bool start_mavlink_receiver();
    void stop_mavlink_receiver();
    void receive_message(mavlink_message_t& message, Connection* connection);

    ReceiverCallback _receiver_callback{};
    std::unique_ptr<MavlinkReceiver> _mavlink_receiver;
    ForwardingOption _forwarding_option;
    std::unordered_set<uint8_t> _system_ids;

    static std::atomic<unsigned> _forwarding_connections_count;

    // void received_mavlink_message(mavlink_message_t &);
};

} // namespace mavsdk
