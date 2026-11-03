#include "mavlink_signing.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>

namespace mavsdk {

void MavlinkSigning::enable(const Key& secret_key)
{
    std::lock_guard<std::mutex> lock(_mutex);
    std::memcpy(_signing.secret_key, secret_key.data(), KEY_LENGTH);
    _signing.flags = MAVLINK_SIGNING_FLAG_SIGN_OUTGOING;
    _signing.link_id = 0;
    _signing.timestamp = 0;
    _signing.accept_unsigned_callback = nullptr;
    // Streams seen with a previous key say nothing about this one.
    _streams = {};
    _enabled = true;
}

void MavlinkSigning::disable()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _enabled = false;
    _signing = {};
    _streams = {};
}

bool MavlinkSigning::check_incoming(const mavlink_message_t& message)
{
    if (!_enabled) {
        return true;
    }

    if ((message.incompat_flags & MAVLINK_IFLAG_SIGNED) == 0) {
        return accepts_unsigned(message.msgid);
    }

    std::lock_guard<std::mutex> lock(_mutex);
    // Re-check under the lock, it might have been disabled in the meantime.
    if (!_enabled) {
        return true;
    }

    update_timestamp();

    if (mavlink_signature_check(&_signing, &_streams, &message)) {
        return true;
    }

    // Like the C parser: a bad signature doesn't matter for messages that
    // don't need one in the first place.
    return accepts_unsigned(message.msgid);
}

void MavlinkSigning::sign_outgoing(mavlink_message_t& message, uint8_t crc_extra)
{
    if (!_enabled || message.magic == MAVLINK_STX_MAVLINK1 ||
        (message.incompat_flags & MAVLINK_IFLAG_SIGNED) != 0) {
        return;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    // Re-check under the lock, it might have been disabled in the meantime.
    if (!_enabled) {
        return;
    }

    // The payload has to be trimmed like mavlink_msg_to_send_buffer() will send it,
    // otherwise neither CRC nor signature match what goes on the wire.
    message.len = _mav_trim_payload(_MAV_PAYLOAD(&message), message.len);
    message.incompat_flags |= MAVLINK_IFLAG_SIGNED;

    uint8_t header[MAVLINK_MAX_HEADER_LEN];
    const uint8_t header_len = mavlink_header_to_send_buffer(header, &message);

    // The CRC doesn't include the magic byte.
    uint16_t checksum = crc_calculate(&header[1], header_len - 1);
    crc_accumulate_buffer(&checksum, _MAV_PAYLOAD(&message), message.len);
    crc_accumulate(crc_extra, &checksum);
    message.checksum = checksum;
    message.ck[0] = static_cast<uint8_t>(checksum & 0xFF);
    message.ck[1] = static_cast<uint8_t>(checksum >> 8);

    update_timestamp();

    mavlink_sign_packet(
        &_signing,
        message.signature,
        header,
        header_len,
        reinterpret_cast<const uint8_t*>(_MAV_PAYLOAD(&message)),
        message.len,
        message.ck);
}

bool MavlinkSigning::accepts_unsigned(uint32_t message_id)
{
    switch (message_id) {
        // Needed to discover a system and its link at all, but it means
        // that someone without the key can make a system appear.
        case MAVLINK_MSG_ID_HEARTBEAT:
        // Injected by radios, which can't sign.
        case MAVLINK_MSG_ID_RADIO_STATUS:
        // From ADS-B receivers, which can't sign either.
        case MAVLINK_MSG_ID_ADSB_VEHICLE:
        case MAVLINK_MSG_ID_COLLISION:
            return true;
        default:
            return false;
    }
}

uint64_t MavlinkSigning::timestamp_now()
{
    // 1 January 2015 00:00:00 GMT in seconds since the Unix epoch.
    constexpr uint64_t signing_epoch_s = 1420070400;

    const auto since_unix_epoch_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                         std::chrono::system_clock::now().time_since_epoch())
                                         .count();

    if (since_unix_epoch_us < 0 ||
        static_cast<uint64_t>(since_unix_epoch_us) < signing_epoch_s * 1000000) {
        return 0;
    }

    return (static_cast<uint64_t>(since_unix_epoch_us) - signing_epoch_s * 1000000) / 10;
}

void MavlinkSigning::update_timestamp()
{
    // The spec asks to go by the wall clock when there is one. mavlink_sign_packet() and
    // mavlink_signature_check() keep it strictly increasing on top of that.
    _signing.timestamp = std::max(_signing.timestamp, timestamp_now());
}

} // namespace mavsdk
