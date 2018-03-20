#pragma once

#include "mavlink_include.h"
#include "global_include.h"
#include <cstdint>

namespace dronecore {

class MAVLinkReceiver
{
public:
    explicit MAVLinkReceiver(uint8_t channel);

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

#if DROP_DEBUG == 1
    void debug_drop_rate();
    void print_line(const char *index, unsigned count, unsigned count_total,
                    unsigned overall_bytes, unsigned overall_bytes_total);
#endif

private:
    uint8_t _channel;
    mavlink_message_t _last_message = {};
    mavlink_status_t _status = {};
    char *_datagram = nullptr;
    unsigned _datagram_len = 0;

#if DROP_DEBUG == 1
    unsigned _bytes_received = 0;

    unsigned _bytes_sent_overall = 0;
    //unsigned _bytes_at_gimbal_overall = 0;
    unsigned _bytes_at_camera_overall = 0;
    unsigned _bytes_at_sdk_overall = 0;

    bool _first = true;
    dl_time_t _last_time;

    double _time_elapsed = 0.0;
#endif
};

} // namespace dronecore
