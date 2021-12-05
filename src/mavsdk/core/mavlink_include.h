#pragma once

#ifdef __GNUC__
#pragma GCC system_header
#endif

#define MAVLINK_GET_CHANNEL_STATUS

#include "mavlink/v2.0/mavlink_types.h"

extern mavlink_status_t* mavlink_get_channel_status(uint8_t chan);

#include "mavlink/v2.0/common/mavlink.h"
