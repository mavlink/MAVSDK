#include "connection.h"
#include "dronecode_sdk_impl.h"
#include "mavlink_channels.h"
#include "global_include.h"

namespace dronecode_sdk {

Connection::Connection(receiver_callback_t receiver_callback) :
    _receiver_callback(receiver_callback),
    _mavlink_receiver()
{}

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

void Connection::receive_message(const mavlink_message_t &message)
{
    _receiver_callback(message);
}

  /**
   * @brief Returns a human-readable English string for a ConnectionResult.
   *
   * @param result The enum value for which a human readable string is required.
   * @return Human readable string for the ConnectionResult.
   */   
   inline const char * Connection::result_str(const Result result)
   {

    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::SOCKET_ERROR:
            return "Socket error";
        case Result::BIND_ERROR:
            return "Bind error";
        case Result::SOCKET_CONNECTION_ERROR:
            return "Socket connection error";
        case Result::CONNECTION_ERROR:
            return "Connection error";
        case Result::NOT_IMPLEMENTED:
            return "Not implemented";
        case Result::SYSTEM_NOT_CONNECTED:
            return "System not connected";
        case Result::SYSTEM_BUSY:
            return "System busy";
        case Result::COMMAND_DENIED:
            return "Command denied";
        case Result::DESTINATION_IP_UNKNOWN:
            return "Destination IP unknown";
        case Result::CONNECTIONS_EXHAUSTED:
            return "Connections exhausted";
        case Result::CONNECTION_URL_INVALID:
            return "Invalid connection URL";
        default:
            return "Unknown";
    }
    
  }

  
} // namespace dronecode_sdk
