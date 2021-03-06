#include "connection.h"
#include "mavsdk_impl.h"
#include "mavlink_channels.h"
#include "global_include.h"

namespace mavsdk {

std::atomic<unsigned> Connection::_forwarding_connections_count = 0;

Connection::Connection(receiver_callback_t receiver_callback, ForwardingOption forwarding_option) :
    _receiver_callback(receiver_callback),
    _mavlink_receiver(),
    _forwarding_option(forwarding_option)
{
    if (forwarding_option == ForwardingOption::ForwardingOn) {
        _forwarding_connections_count++;
    }
}

Connection::~Connection()
{
    // Just in case a specific connection didn't call it already.
    stop_mavlink_receiver();
    _receiver_callback = {};
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

void Connection::receive_message(mavlink_message_t& message, Connection* connection)
{
    _receiver_callback(message, connection);
}

bool Connection::should_forward_messages() const
{
    return _forwarding_option == ForwardingOption::ForwardingOn;
}

unsigned Connection::forwarding_connections_count() const
{
    return _forwarding_connections_count;
}

} // namespace mavsdk
