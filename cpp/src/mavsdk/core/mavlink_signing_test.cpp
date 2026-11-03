#include "mavlink_signing.hpp"
#include <gtest/gtest.h>
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

} // namespace

TEST(MavlinkSigning, DisabledAcceptsEverythingAndSignsNothing)
{
    MavlinkSigning signing;
    EXPECT_FALSE(signing.is_enabled());

    auto message = make_attitude();
    EXPECT_TRUE(signing.check_incoming(message));

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

    EXPECT_TRUE(receiver.check_incoming(parsed.value()));
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
    EXPECT_TRUE(receiver.check_incoming(parsed.value()));
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
    EXPECT_FALSE(receiver.check_incoming(parsed.value()));
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
    EXPECT_FALSE(receiver.check_incoming(parsed.value()));
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

    EXPECT_TRUE(receiver.check_incoming(parsed_first.value()));
    EXPECT_TRUE(receiver.check_incoming(parsed_second.value()));

    // Same timestamps again.
    EXPECT_FALSE(receiver.check_incoming(parsed_first.value()));
    EXPECT_FALSE(receiver.check_incoming(parsed_second.value()));
}

TEST(MavlinkSigning, UnsignedOnlyAcceptedForSomeMessages)
{
    MavlinkSigning receiver;
    receiver.enable(make_key(1));

    EXPECT_FALSE(receiver.check_incoming(make_attitude()));
    EXPECT_TRUE(receiver.check_incoming(make_heartbeat()));
}

TEST(MavlinkSigning, DisableAcceptsUnsignedAgain)
{
    MavlinkSigning receiver;
    receiver.enable(make_key(1));
    EXPECT_FALSE(receiver.check_incoming(make_attitude()));

    receiver.disable();
    EXPECT_FALSE(receiver.is_enabled());
    EXPECT_TRUE(receiver.check_incoming(make_attitude()));
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
