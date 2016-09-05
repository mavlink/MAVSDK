#include "mavlink_receiver.h"
#include "global_include.h"

namespace dronelink {

MavlinkReceiver::MavlinkReceiver(uint8_t channel) :
    _channel(channel),
    _message{},
    _status{}
{
}

bool MavlinkReceiver::parse_datagram(const char *datagram, unsigned datagram_len)
{
    for (unsigned i = 0; i < datagram_len; ++i) {

        if (mavlink_parse_char(_channel, datagram[i], &_message, &_status)) {
            return true;
        }
    }
    return false;
}

Result MavlinkReceiver::send_message(const mavlink_message_t &message)
{
    UNUSED(message);
    return Result::CONNECTION_ERROR;
}

} // namespace dronelink
