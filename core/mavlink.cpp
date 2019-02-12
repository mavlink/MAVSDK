#include "connection.h"
#include "dronecode_sdk_impl.h"
#include "mavlink_channels.h"
#include "global_include.h"

namespace dronecode_sdk {

Mavlink::Mavlink(receiver_callback_t receiver_callback) :
    _receiver_callback(receiver_callback),
    _mavlink_receiver()
{}

Mavlink::~Mavlink()
{
    // Just in case a specific connection didn't call it already.
    stop_mavlink_receiver();
    _receiver_callback = {};
}

bool Mavlink::start_mavlink_receiver()
{
    uint8_t channel;
    if (!MAVLinkChannels::Instance().checkout_free_channel(channel)) {
        return false;
    }

    _mavlink_receiver.reset(new MAVLinkReceiver(channel));
    return true;
}

void Mavlink::stop_mavlink_receiver()
{
    if (_mavlink_receiver) {
        uint8_t used_channel = _mavlink_receiver->get_channel();
        // Destroy receiver before giving the channel back.
        _mavlink_receiver.reset();
        MAVLinkChannels::Instance().checkin_used_channel(used_channel);
    }
}

void Mavlink::receive_message(const mavlink_message_t &message)
{
    _receiver_callback(message);
}

} // namespace dronecode_sdk
