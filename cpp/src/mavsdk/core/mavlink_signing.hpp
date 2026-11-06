#pragma once

#include "log_rate_limiter.hpp"
#include "mavlink_include.hpp"
#include "mavsdk_export.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <tuple>

namespace mavsdk {

// MAVLink 2 message signing, see https://mavlink.io/en/guide/message_signing.html
//
// One instance is shared by all connections: incoming messages are checked against one
// set of streams, and outgoing messages are signed once, before they go out on every
// connection, with one link ID.
class MAVSDK_TEST_EXPORT MavlinkSigning {
public:
    static constexpr std::size_t KEY_LENGTH = 32;
    using Key = std::array<uint8_t, KEY_LENGTH>;

    // A stream is one sysid/compid/link ID combination, and vehicles sign each of their
    // links with its own link ID. Streams that can no longer be replayed are dropped
    // when the table is full, so this only limits how many are active at the same time.
    static constexpr std::size_t DEFAULT_MAX_STREAMS = 256;

    // A new stream is accepted if its timestamp is at most this far behind ours,
    // in signing timestamp units (10 microseconds). One minute, like the C library.
    static constexpr uint64_t NEW_STREAM_TIMESTAMP_LIMIT = 60ULL * 100000ULL;

    enum class Result {
        Accepted,
        Unsigned,
        BadSignature, // Wrong key, or tampered with.
        Replay, // Timestamp not newer than the last one of the stream.
        OldTimestamp, // New stream with a timestamp too far behind ours.
        TooManyStreams,
    };

    explicit MavlinkSigning(std::size_t max_streams = DEFAULT_MAX_STREAMS);

    // Enabling it again with the same key while enabled keeps the streams seen so far.
    // A different key, or enabling it after disable(), starts over.
    void enable(const Key& secret_key);
    void disable();

    bool is_enabled() const { return _enabled.load(); }

    // Whether a framed message is accepted, i.e. has a valid signature.
    // Always Accepted while signing is disabled. Rejections are logged, rate-limited.
    //
    // This is the (comparatively expensive) SHA-256 check, call it once per message.
    Result check_incoming(const mavlink_message_t& message);

    // Signs an outgoing MAVLink 2 message in place: sets the signed flag, which
    // changes the header and thereby the CRC, so it needs the message's CRC extra.
    // No-op while signing is disabled, for MAVLink 1 and for already signed messages.
    void sign_outgoing(mavlink_message_t& message, uint8_t crc_extra);

    // Current time in the units of the signing timestamp: 10 microseconds since
    // 1 January 2015 GMT.
    static uint64_t timestamp_now();

    static const char* result_str(Result result);

private:
    Result check_incoming_locked(const mavlink_message_t& message);
    void warn_rejected(const mavlink_message_t& message, Result result);

    // Also used for the incoming check: the next timestamp we sign with
    // has to be newer than anything we've accepted.
    void update_timestamp();

    std::atomic<bool> _enabled{false};

    std::mutex _mutex{};
    mavlink_signing_t _signing{};

    // link ID, sysid, compid -> last timestamp
    using StreamId = std::tuple<uint8_t, uint32_t, uint8_t>;
    std::map<StreamId, uint64_t> _streams{};
    const std::size_t _max_streams;

    LogRateLimiter _warning_limiter{};
    LogRateLimiter _radio_status_limiter{};
};

} // namespace mavsdk
