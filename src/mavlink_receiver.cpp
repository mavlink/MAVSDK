#include "mavlink_receiver.h"
#include "global_include.h"

namespace dronelink {

MavlinkReceiver::MavlinkReceiver(uint8_t channel) :
    _channel(channel)
{
}

void MavlinkReceiver::set_new_datagram(char *datagram, unsigned datagram_len)
{
    _datagram = datagram;
    _datagram_len = datagram_len;
}

bool MavlinkReceiver::parse_message()
{
    // Note that one datagram can contain multiple mavlink messages.
    for (unsigned i = 0; i < _datagram_len; ++i) {
        if (mavlink_parse_char(_channel, _datagram[i], &_last_message, &_status) == 1) {

            // Move the pointer to the datagram forward by the amount parsed.
            _datagram += i;
            // And decrease the length, so we don't overshoot in the next round.
            _datagram_len -= i;

            // We have parsed one message, let's return so it can be handled.
            return true;
        }
    }

    // No (more) messages, let's give up.
    _datagram = nullptr;
    _datagram_len = 0;
    return false;
}

} // namespace dronelink
