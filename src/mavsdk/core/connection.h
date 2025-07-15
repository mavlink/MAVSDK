#pragma once

#include "mavsdk.h"
#include "mavlink_receiver.h"
#include "libmav_receiver.h"
#include <atomic>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>

namespace mavsdk {

class Connection {
public:
    using ReceiverCallback =
        std::function<void(mavlink_message_t& message, Connection* connection)>;
    using LibmavReceiverCallback =
        std::function<void(const LibmavMessage& message, Connection* connection)>;

    explicit Connection(
        ReceiverCallback receiver_callback,
        LibmavReceiverCallback libmav_receiver_callback,
        ForwardingOption forwarding_option = ForwardingOption::ForwardingOff);
    virtual ~Connection();

    virtual ConnectionResult start() = 0;
    virtual ConnectionResult stop() = 0;

    virtual std::pair<bool, std::string> send_message(const mavlink_message_t& message) = 0;

    bool has_system_id(uint8_t system_id);
    bool should_forward_messages() const;
    static unsigned forwarding_connections_count();

    // Access to libmav receiver for message creation
    LibmavReceiver* get_libmav_receiver() { return _libmav_receiver.get(); }

    // Non-copyable
    Connection(const Connection&) = delete;
    const Connection& operator=(const Connection&) = delete;

protected:
    bool start_mavlink_receiver();
    void stop_mavlink_receiver();
    void receive_message(mavlink_message_t& message, Connection* connection);

    bool start_libmav_receiver();
    void stop_libmav_receiver();
    void receive_libmav_message(const LibmavMessage& message, Connection* connection);

    ReceiverCallback _receiver_callback{};
    LibmavReceiverCallback _libmav_receiver_callback{};
    std::unique_ptr<MavlinkReceiver> _mavlink_receiver;
    std::unique_ptr<LibmavReceiver> _libmav_receiver;
    ForwardingOption _forwarding_option;
    std::unordered_set<uint8_t> _system_ids;

    bool _debugging = false;

    static std::atomic<unsigned> _forwarding_connections_count;

    // void received_mavlink_message(mavlink_message_t &);
};

} // namespace mavsdk
