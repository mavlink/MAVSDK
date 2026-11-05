#include "mavlink_signing.hpp"
#include "mavlink_wire.hpp"
#include <gtest/gtest.h>
#include <cstring>
#include <optional>
#include <vector>

using namespace mavsdk;

namespace {

MavlinkSigning::Key make_key(uint8_t seed)
{
    MavlinkSigning::Key key{};
    for (std::size_t i = 0; i < key.size(); ++i) {
        key[i] = static_cast<uint8_t>(seed + i);
    }
    return key;
}

mavlink_message_t make_attitude(float roll = 0.1f)
{
    mavlink_message_t message{};
    mavlink_msg_attitude_pack(1, 1, &message, 1234, roll, 0.2f, 0.3f, 0.0f, 0.0f, 0.0f);
    return message;
}

mavlink_message_t make_heartbeat()
{
    mavlink_message_t message{};
    mavlink_msg_heartbeat_pack(
        1, 1, &message, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4, 0, 0, MAV_STATE_ACTIVE);
    return message;
}

void sign(MavlinkSigning& signing, mavlink_message_t& message)
{
    signing.sign_outgoing(message, mavlink_get_crc_extra(&message));
}

std::vector<uint8_t> to_bytes(const mavlink_message_t& message)
{
    std::vector<uint8_t> bytes(MAVLINK_MAX_PACKET_LEN);
    bytes.resize(mavlink_msg_to_send_buffer(bytes.data(), &message));
    return bytes;
}

// Frames like the receiver does: without a signing struct, so the signature is not checked.
std::optional<mavlink_message_t> parse(const std::vector<uint8_t>& bytes)
{
    mavlink_message_t buffer{};
    mavlink_status_t buffer_status{};
    mavlink_message_t message{};
    mavlink_status_t status{};

    for (const auto byte : bytes) {
        if (mavlink_frame_char_buffer(&buffer, &buffer_status, byte, &message, &status) ==
            MAVLINK_FRAMING_OK) {
            return message;
        }
    }
    return std::nullopt;
}

// Signed by the C library, as another system would, with the given link ID and timestamp.
mavlink_message_t make_signed_by_other(
    const MavlinkSigning::Key& key, uint8_t sysid, uint8_t link_id, uint64_t timestamp)
{
    mavlink_signing_t other{};
    std::memcpy(other.secret_key, key.data(), key.size());
    other.flags = MAVLINK_SIGNING_FLAG_SIGN_OUTGOING;
    other.link_id = link_id;
    other.timestamp = timestamp;

    constexpr uint8_t channel = MAVLINK_COMM_NUM_BUFFERS - 1;
    auto* status = mavlink_get_channel_status(channel);
    status->signing = &other;
    mavlink_message_t message{};
    mavlink_msg_attitude_pack_chan(
        sysid, 1, channel, &message, 1234, 0.1f, 0.2f, 0.3f, 0.0f, 0.0f, 0.0f);
    status->signing = nullptr;

    return parse(to_bytes(message)).value();
}

constexpr uint64_t one_minute = MavlinkSigning::NEW_STREAM_TIMESTAMP_LIMIT;

} // namespace

TEST(MavlinkSigning, DisabledAcceptsEverythingAndSignsNothing)
{
    MavlinkSigning signing;
    EXPECT_FALSE(signing.is_enabled());

    auto message = make_attitude();
    EXPECT_EQ(signing.check_incoming(message), MavlinkSigning::Result::Accepted);

    sign(signing, message);
    EXPECT_EQ(message.incompat_flags & MAVLINK_IFLAG_SIGNED, 0);
}

TEST(MavlinkSigning, SignedMessageRoundTrip)
{
    MavlinkSigning sender;
    MavlinkSigning receiver;
    sender.enable(make_key(1));
    receiver.enable(make_key(1));

    auto message = make_attitude();
    sign(sender, message);
    EXPECT_NE(message.incompat_flags & MAVLINK_IFLAG_SIGNED, 0);

    // The CRC has to cover the signed flag, otherwise the frame doesn't even parse.
    const auto parsed = parse(to_bytes(message));
    ASSERT_TRUE(parsed);
    EXPECT_NE(parsed->incompat_flags & MAVLINK_IFLAG_SIGNED, 0);

    EXPECT_EQ(receiver.check_incoming(parsed.value()), MavlinkSigning::Result::Accepted);
}

TEST(MavlinkSigning, TrimmedPayloadRoundTrip)
{
    MavlinkSigning sender;
    MavlinkSigning receiver;
    sender.enable(make_key(1));
    receiver.enable(make_key(1));

    // All the trailing fields are zero, so the payload gets trimmed on the wire.
    auto message = make_attitude(0.0f);
    message.len = MAVLINK_MSG_ID_ATTITUDE_LEN;
    sign(sender, message);
    EXPECT_LT(message.len, MAVLINK_MSG_ID_ATTITUDE_LEN);

    const auto parsed = parse(to_bytes(message));
    ASSERT_TRUE(parsed);
    EXPECT_EQ(receiver.check_incoming(parsed.value()), MavlinkSigning::Result::Accepted);
}

TEST(MavlinkSigning, WrongKeyRejected)
{
    MavlinkSigning sender;
    MavlinkSigning receiver;
    sender.enable(make_key(1));
    receiver.enable(make_key(2));

    auto message = make_attitude();
    sign(sender, message);

    const auto parsed = parse(to_bytes(message));
    ASSERT_TRUE(parsed);
    EXPECT_EQ(receiver.check_incoming(parsed.value()), MavlinkSigning::Result::BadSignature);
}

TEST(MavlinkSigning, TamperedSignatureRejected)
{
    MavlinkSigning sender;
    MavlinkSigning receiver;
    sender.enable(make_key(1));
    receiver.enable(make_key(1));

    auto message = make_attitude();
    sign(sender, message);

    auto bytes = to_bytes(message);
    // The last byte is part of the signature, which isn't covered by the CRC.
    bytes.back() ^= 0xFF;

    const auto parsed = parse(bytes);
    ASSERT_TRUE(parsed);
    EXPECT_EQ(receiver.check_incoming(parsed.value()), MavlinkSigning::Result::BadSignature);
}

TEST(MavlinkSigning, ReplayRejected)
{
    MavlinkSigning sender;
    MavlinkSigning receiver;
    sender.enable(make_key(1));
    receiver.enable(make_key(1));

    auto first = make_attitude();
    sign(sender, first);
    auto second = make_attitude();
    sign(sender, second);

    const auto parsed_first = parse(to_bytes(first));
    const auto parsed_second = parse(to_bytes(second));
    ASSERT_TRUE(parsed_first);
    ASSERT_TRUE(parsed_second);

    EXPECT_EQ(receiver.check_incoming(parsed_first.value()), MavlinkSigning::Result::Accepted);
    EXPECT_EQ(receiver.check_incoming(parsed_second.value()), MavlinkSigning::Result::Accepted);

    // Same timestamps again.
    EXPECT_EQ(receiver.check_incoming(parsed_first.value()), MavlinkSigning::Result::Replay);
    EXPECT_EQ(receiver.check_incoming(parsed_second.value()), MavlinkSigning::Result::Replay);
}

TEST(MavlinkSigning, UnsignedRejected)
{
    MavlinkSigning receiver;
    receiver.enable(make_key(1));

    EXPECT_EQ(receiver.check_incoming(make_attitude()), MavlinkSigning::Result::Unsigned);
    EXPECT_EQ(receiver.check_incoming(make_heartbeat()), MavlinkSigning::Result::Unsigned);
}

TEST(MavlinkSigning, DisableAcceptsUnsignedAgain)
{
    MavlinkSigning receiver;
    receiver.enable(make_key(1));
    EXPECT_EQ(receiver.check_incoming(make_attitude()), MavlinkSigning::Result::Unsigned);

    receiver.disable();
    EXPECT_FALSE(receiver.is_enabled());
    EXPECT_EQ(receiver.check_incoming(make_attitude()), MavlinkSigning::Result::Accepted);
}

TEST(MavlinkSigning, Mavlink1NotSigned)
{
    MavlinkSigning sender;
    sender.enable(make_key(1));

    auto message = make_attitude();
    message.magic = MAVLINK_STX_MAVLINK1;
    sign(sender, message);
    EXPECT_EQ(message.incompat_flags & MAVLINK_IFLAG_SIGNED, 0);
}

TEST(MavlinkSigning, TimestampAfterSigningEpoch)
{
    // 1 January 2020 in signing timestamp units, as a lower bound.
    EXPECT_GT(MavlinkSigning::timestamp_now(), 15776640000000ULL);
}

TEST(MavlinkSigning, MoreStreamsThanTheCLibraryTable)
{
    MavlinkSigning receiver;
    receiver.enable(make_key(1));

    const auto now = MavlinkSigning::timestamp_now();
    for (unsigned i = 0; i < 4 * MAVLINK_MAX_SIGNING_STREAMS; ++i) {
        const auto message = make_signed_by_other(
            make_key(1), static_cast<uint8_t>(1 + i / 4), static_cast<uint8_t>(i % 4), now);
        EXPECT_EQ(receiver.check_incoming(message), MavlinkSigning::Result::Accepted);
    }
}

TEST(MavlinkSigning, FullTableMakesRoomForNewStreams)
{
    MavlinkSigning receiver(2);
    receiver.enable(make_key(1));

    const auto now = MavlinkSigning::timestamp_now();
    EXPECT_EQ(
        receiver.check_incoming(make_signed_by_other(make_key(1), 1, 0, now)),
        MavlinkSigning::Result::Accepted);
    EXPECT_EQ(
        receiver.check_incoming(make_signed_by_other(make_key(1), 2, 0, now)),
        MavlinkSigning::Result::Accepted);
    EXPECT_EQ(
        receiver.check_incoming(make_signed_by_other(make_key(1), 3, 0, now)),
        MavlinkSigning::Result::TooManyStreams);

    // System 2 is two minutes ahead, so system 1, last seen now, can't be replayed anymore
    // and makes room.
    EXPECT_EQ(
        receiver.check_incoming(make_signed_by_other(make_key(1), 2, 0, now + 2 * one_minute)),
        MavlinkSigning::Result::Accepted);
    EXPECT_EQ(
        receiver.check_incoming(make_signed_by_other(make_key(1), 3, 0, now + 2 * one_minute)),
        MavlinkSigning::Result::Accepted);

    // And a replay of system 1 is rejected as too old for a new stream.
    EXPECT_EQ(
        receiver.check_incoming(make_signed_by_other(make_key(1), 1, 0, now + 1)),
        MavlinkSigning::Result::OldTimestamp);
}

TEST(MavlinkSigning, NewStreamWithOldTimestampRejected)
{
    MavlinkSigning receiver;
    receiver.enable(make_key(1));

    const auto now = MavlinkSigning::timestamp_now();
    EXPECT_EQ(
        receiver.check_incoming(make_signed_by_other(make_key(1), 1, 0, now - 2 * one_minute)),
        MavlinkSigning::Result::OldTimestamp);
    EXPECT_EQ(
        receiver.check_incoming(make_signed_by_other(make_key(1), 1, 0, now - one_minute / 2)),
        MavlinkSigning::Result::Accepted);
}

TEST(MavlinkSigning, EnableWithSameKeyKeepsStreams)
{
    MavlinkSigning receiver;
    receiver.enable(make_key(1));

    const auto now = MavlinkSigning::timestamp_now();
    const auto message = make_signed_by_other(make_key(1), 1, 0, now);
    EXPECT_EQ(receiver.check_incoming(message), MavlinkSigning::Result::Accepted);

    receiver.enable(make_key(1));
    EXPECT_EQ(receiver.check_incoming(message), MavlinkSigning::Result::Replay);

    // Another key starts over.
    receiver.enable(make_key(2));
    receiver.enable(make_key(1));
    EXPECT_EQ(receiver.check_incoming(message), MavlinkSigning::Result::Accepted);
}

TEST(MavlinkSigning, UntrimmedSignedFrameSurvivesForwarding)
{
    MavlinkSigning receiver;
    receiver.enable(make_key(1));

    // Signed by a sender that doesn't trim: zero bytes at the end of the payload.
    mavlink_signing_t other{};
    const auto key = make_key(1);
    std::memcpy(other.secret_key, key.data(), key.size());
    other.flags = MAVLINK_SIGNING_FLAG_SIGN_OUTGOING;
    other.timestamp = MavlinkSigning::timestamp_now();

    mavlink_message_t message{};
    mavlink_msg_attitude_pack(1, 1, &message, 1234, 0.1f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f);
    message.len = MAVLINK_MSG_ID_ATTITUDE_LEN;
    message.incompat_flags |= MAVLINK_IFLAG_SIGNED;
    uint8_t header[MAVLINK_MAX_HEADER_LEN];
    const uint8_t header_len = mavlink_header_to_send_buffer(header, &message);
    uint16_t checksum = crc_calculate(&header[1], header_len - 1);
    crc_accumulate_buffer(&checksum, _MAV_PAYLOAD(&message), message.len);
    crc_accumulate(MAVLINK_MSG_ID_ATTITUDE_CRC, &checksum);
    message.checksum = checksum;
    message.ck[0] = static_cast<uint8_t>(checksum & 0xFF);
    message.ck[1] = static_cast<uint8_t>(checksum >> 8);
    mavlink_sign_packet(
        &other,
        message.signature,
        header,
        header_len,
        reinterpret_cast<const uint8_t*>(_MAV_PAYLOAD(&message)),
        message.len,
        message.ck);

    std::vector<uint8_t> bytes(MAVLINK_MAX_PACKET_LEN);
    bytes.resize(mavlink_message_to_wire(bytes.data(), message));
    const auto parsed = parse(bytes);
    ASSERT_TRUE(parsed);
    EXPECT_EQ(parsed->len, MAVLINK_MSG_ID_ATTITUDE_LEN);

    // Forwarded as received.
    bytes.assign(MAVLINK_MAX_PACKET_LEN, 0);
    bytes.resize(mavlink_message_to_wire(bytes.data(), parsed.value()));
    const auto forwarded = parse(bytes);
    ASSERT_TRUE(forwarded);
    EXPECT_EQ(receiver.check_incoming(forwarded.value()), MavlinkSigning::Result::Accepted);
}
