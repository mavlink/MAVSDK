#pragma once

#include <cstdint>
#include "mavlink_include.hpp"

namespace mavsdk {

// Full target system ID, or 0 for broadcast. Use this rather than the payload field,
// which only holds a sentinel (255) for targets above 255.
inline uint32_t target_system_id(const mavlink_message_t& message)
{
    return mavlink_msg_get_target_sysid(&message, mavlink_get_msg_entry(message.msgid));
}

} // namespace mavsdk
