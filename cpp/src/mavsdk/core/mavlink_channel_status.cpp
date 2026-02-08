#include "mavlink_include.h"

mavlink_status_t* mavlink_get_channel_status(uint8_t chan)
{
    static mavlink_status_t m_mavlink_status[MAVLINK_COMM_NUM_BUFFERS];
    return &m_mavlink_status[chan];
}
