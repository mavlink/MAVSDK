#include "mavlink_signing.hpp"
#include "log.hpp"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <string>

namespace mavsdk {

MavlinkSigning::MavlinkSigning(std::size_t max_streams) : _max_streams(max_streams) {}

void MavlinkSigning::enable(const Key& secret_key)
{
    std::lock_guard<std::mutex> lock(_mutex);
    // Streams seen with a previous key say nothing about this one. With the same key, they
    // are kept: starting over would accept anything up to a minute old again, replays too.
    if (!_enabled || std::memcmp(_signing.secret_key, secret_key.data(), KEY_LENGTH) != 0) {
        _streams.clear();
        _signing.timestamp = 0;
    }
    std::memcpy(_signing.secret_key, secret_key.data(), KEY_LENGTH);
    _signing.flags = MAVLINK_SIGNING_FLAG_SIGN_OUTGOING;
    _signing.link_id = 0;
    _signing.accept_unsigned_callback = nullptr;
    _enabled = true;
}

void MavlinkSigning::disable()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _enabled = false;
    _signing = {};
    _streams.clear();
}

MavlinkSigning::Result MavlinkSigning::check_incoming(const mavlink_message_t& message)
{
    if (!_enabled) {
        return Result::Accepted;
    }

    std::lock_guard<std::mutex> lock(_mutex);
    // Re-check under the lock, it might have been disabled in the meantime.
    if (!_enabled) {
        return Result::Accepted;
    }

    const auto result = check_incoming_locked(message);
    if (result != Result::Accepted) {
        warn_rejected(message, result);
    }
    return result;
}

MavlinkSigning::Result MavlinkSigning::check_incoming_locked(const mavlink_message_t& message)
{
    if (message.magic == MAVLINK_STX_MAVLINK1 ||
        (message.incompat_flags & MAVLINK_IFLAG_SIGNED) == 0) {
        return Result::Unsigned;
    }

    update_timestamp();

    // Without a stream table, the C library only checks the signature itself, and then
    // fails with NO_STREAMS. Its table is a fixed size (MAVLINK_MAX_SIGNING_STREAMS, which
    // would have to be the same in every file including mavlink_types.h to change it) and
    // never forgets a stream, so we keep our own.
    if (!mavlink_signature_check(&_signing, nullptr, &message) &&
        _signing.last_status != MAVLINK_SIGNING_STATUS_NO_STREAMS) {
        return Result::BadSignature;
    }

    const uint8_t link_id = message.signature[0];
    uint64_t timestamp = 0;
    for (int i = 5; i >= 0; --i) {
        timestamp = (timestamp << 8) | message.signature[1 + i];
    }

    const StreamId stream_id{link_id, message.sysid, message.compid};
    auto it = _streams.find(stream_id);
    if (it != _streams.end()) {
        if (timestamp <= it->second) {
            return Result::Replay;
        }
        it->second = timestamp;
    } else {
        if (timestamp + NEW_STREAM_TIMESTAMP_LIMIT < _signing.timestamp) {
            return Result::OldTimestamp;
        }

        if (_streams.size() >= _max_streams) {
            // A stream whose last timestamp is too old for a new stream can be forgotten:
            // a replay of it would be rejected as too old anyway.
            for (auto stale = _streams.begin(); stale != _streams.end();) {
                if (stale->second + NEW_STREAM_TIMESTAMP_LIMIT < _signing.timestamp) {
                    stale = _streams.erase(stale);
                } else {
                    ++stale;
                }
            }
            if (_streams.size() >= _max_streams) {
                return Result::TooManyStreams;
            }
        }

        _streams.emplace(stream_id, timestamp);
    }

    // Our next timestamp must be at least this timestamp.
    _signing.timestamp = std::max(_signing.timestamp, timestamp);
    return Result::Accepted;
}

void MavlinkSigning::warn_rejected(const mavlink_message_t& message, Result result)
{
    // Every message of a system with the wrong key is rejected, so only warn now and then,
    // but do warn: otherwise it just looks like there is no system.
    unsigned suppressed = 0;
    if (!_warning_limiter.should_log(suppressed)) {
        return;
    }

    LogWarn(
        "Signing: dropped message {} from {}/{}: {}{}",
        static_cast<uint32_t>(message.msgid),
        message.sysid,
        static_cast<int>(message.compid),
        result_str(result),
        suppressed > 0 ? " (and " + std::to_string(suppressed) + " more since the last warning)" :
                         std::string{});
}

const char* MavlinkSigning::result_str(Result result)
{
    switch (result) {
        case Result::Accepted:
            return "accepted";
        case Result::Unsigned:
            return "not signed";
        case Result::BadSignature:
            return "bad signature (different key?)";
        case Result::Replay:
            return "timestamp not newer than the last one (replay?)";
        case Result::OldTimestamp:
            return "timestamp more than a minute behind ours (clock behind?)";
        case Result::TooManyStreams:
            return "too many signing streams";
    }
    return "unknown";
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

    // Trimmed like the C library does when packing, the CRC and signature then cover
    // exactly the payload that goes on the wire.
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
