#pragma once

#include "dronecode_sdk.h"
#include "mavlink_receiver.h"
#include <memory>

namespace dronecode_sdk {

class Mavlink {
public:
    typedef std::function<void(const mavlink_message_t &message)> receiver_callback_t;

    Mavlink(receiver_callback_t receiver_callback);
    virtual ~Mavlink();

    virtual MavlinkResult start() = 0;
    virtual MavlinkResult stop() = 0;

    virtual bool send_message(const mavlink_message_t &message) = 0;

    // Non-copyable
    Mavlink(const Mavlink &) = delete;
    const Mavlink &operator=(const Mavlink &) = delete;

protected:
    bool start_mavlink_receiver();
    void stop_mavlink_receiver();
    void receive_message(const mavlink_message_t &message);

    receiver_callback_t _receiver_callback{};
    std::unique_ptr<MAVLinkReceiver> _mavlink_receiver;

    // void received_mavlink_message(mavlink_message_t &);
};

} // namespace dronecode_sdk
