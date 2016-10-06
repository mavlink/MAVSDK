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
        if (mavlink_parse_char(_channel, datagram[i], &_message, &_status) == 1) {
            return true;
        }
    }
    return false;
}

} // namespace dronelink
