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

    mavlink_message_t &get_last_message()
    {
        return _last_message;
    }

    mavlink_status_t &get_status()
    {
        return _status;
    }

    void set_new_datagram(char *datagram, unsigned datagram_len);

    bool parse_message();

private:
    uint8_t _channel;
    mavlink_message_t _last_message = {};
    mavlink_status_t _status = {};
    char *_datagram = nullptr;
    unsigned _datagram_len = 0;
};

} // namespace dronelink
