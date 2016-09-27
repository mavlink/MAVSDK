#pragma once

#include "mavlink_include.h"
#include <cstdint>

namespace dronelink {

class MavlinkReceiver
{
public:
    explicit MavlinkReceiver(uint8_t channel);

    uint8_t get_channel()
    {
        return _channel;
    }

    mavlink_message_t &get_message()
    {
        return _message;
    }

    mavlink_status_t &get_status()
    {
        return _status;
    }

    bool parse_datagram(const char *datagram, unsigned datagram_len);

private:
    uint8_t _channel;
    mavlink_message_t _message;
    mavlink_status_t _status;
};

} // namespace dronelink
