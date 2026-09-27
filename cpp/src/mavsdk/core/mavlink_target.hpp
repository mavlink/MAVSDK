#pragma once

#include <cstdint>
#include "mavlink_include.hpp"

namespace mavsdk {

// The full target system ID of a message, or 0 for a broadcast or a message without a target.
//
// A target above 255 travels in the extended header, and the payload's 8 bit target_system then
// holds MAVLINK_TARGET_SYSTEM_SENTINEL (255), which is a valid system ID as well. Checking whom a
// message is for therefore has to use this rather than the decoded payload field.
inline uint32_t target_system_id(const mavlink_message_t& message)
{
    return mavlink_msg_get_target_sysid(&message, mavlink_get_msg_entry(message.msgid));
}

} // namespace mavsdk
