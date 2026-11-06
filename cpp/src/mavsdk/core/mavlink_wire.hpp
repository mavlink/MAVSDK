#pragma once

#include "mavlink_include.hpp"
#include <cstdint>
#include <cstring>

namespace mavsdk {

// Serializes a message exactly as its fields say, unlike mavlink_msg_to_send_buffer() which
// trims the payload of MAVLink 2 messages. The CRC and signature of a received frame cover
// the payload as it was sent, so a frame that wasn't trimmed by its sender has to be
// forwarded as it is. Messages packed locally are trimmed already when they are finalized.
//
// buffer has to hold MAVLINK_MAX_PACKET_LEN bytes. Returns 0 if the message can't be
// represented.
inline uint16_t mavlink_message_to_wire(uint8_t* buffer, const mavlink_message_t& message)
{
    const uint8_t header_len = mavlink_header_to_send_buffer(buffer, &message);
    if (header_len == 0) {
        return 0;
    }
    uint16_t len = header_len;
    std::memcpy(&buffer[len], _MAV_PAYLOAD(&message), message.len);
    len += message.len;
    // Not ck: packing only sets checksum (and puts the CRC bytes after the payload).
    // For a parsed frame, checksum is the CRC as received: the parser puts the received one
    // there when it doesn't match the one it calculated, or can't calculate it.
    buffer[len++] = static_cast<uint8_t>(message.checksum & 0xFF);
    buffer[len++] = static_cast<uint8_t>(message.checksum >> 8);
    if (message.magic != MAVLINK_STX_MAVLINK1 &&
        (message.incompat_flags & MAVLINK_IFLAG_SIGNED) != 0) {
        std::memcpy(&buffer[len], message.signature, MAVLINK_SIGNATURE_BLOCK_LEN);
        len += MAVLINK_SIGNATURE_BLOCK_LEN;
    }
    return len;
}

} // namespace mavsdk
