#include "connection.h"
#include "dronecore_impl.h"
#include "mavlink_channels.h"
#include "global_include.h"

namespace dronecore {

Connection::Connection(DroneCoreImpl &parent) :
    _parent(parent),
    _mavlink_receiver() {}

Connection::~Connection()
{
    // Just in case a specific connection didn't call it already.
    stop_mavlink_receiver();
}

bool Connection::start_mavlink_receiver()
{
    uint8_t channel;
    if (!MAVLinkChannels::Instance().checkout_free_channel(channel)) {
        return false;
    }

    _mavlink_receiver.reset(new MAVLinkReceiver(channel));
    return true;
}

void Connection::stop_mavlink_receiver()
{
    if (_mavlink_receiver) {

        uint8_t used_channel = _mavlink_receiver->get_channel();
        // Destroy receiver before giving the channel back.
        _mavlink_receiver.reset();
        MAVLinkChannels::Instance().checkin_used_channel(used_channel);
    }
}

void Connection::receive_message(const mavlink_message_t &message)
{
    _parent.receive_message(message);
}

} // namespace dronecore
