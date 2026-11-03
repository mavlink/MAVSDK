#pragma once

#include "mavlink_include.hpp"
#include "mavsdk_export.h"
#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>

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

    void enable(const Key& secret_key);
    void disable();

    bool is_enabled() const { return _enabled.load(); }

    // Whether a framed message is accepted: messages with a valid signature, and unsigned
    // (or badly signed) messages that the spec allows without one, e.g. RADIO_STATUS.
    // Always true while signing is disabled.
    //
    // This is the (comparatively expensive) SHA-256 check, call it once per message.
    bool check_incoming(const mavlink_message_t& message);

    // Signs an outgoing MAVLink 2 message in place: sets the signed flag, which
    // changes the header and thereby the CRC, so it needs the message's CRC extra.
    // No-op while signing is disabled, for MAVLink 1 and for already signed messages.
    void sign_outgoing(mavlink_message_t& message, uint8_t crc_extra);

    // Messages accepted without a signature, the same as PX4 accepts.
    static bool accepts_unsigned(uint32_t message_id);

    // Current time in the units of the signing timestamp: 10 microseconds since
    // 1 January 2015 GMT.
    static uint64_t timestamp_now();

private:
    // Also used for the incoming check: the next timestamp we sign with
    // has to be newer than anything we've accepted.
    void update_timestamp();

    std::atomic<bool> _enabled{false};

    std::mutex _mutex{};
    mavlink_signing_t _signing{};
    mavlink_signing_streams_t _streams{};
};

} // namespace mavsdk
