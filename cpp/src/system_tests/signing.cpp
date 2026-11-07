#include "log.hpp"
#include "mavsdk.hpp"
#include "mavlink_include.hpp"
#include "plugins/action/action.hpp"
#include "plugins/action_server/action_server.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include <asio/io_context.hpp>
#include <asio/ip/udp.hpp>
#include <array>
#include <atomic>
#include <chrono>
#include <cstring>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

namespace {

std::array<uint8_t, Mavsdk::SIGNING_KEY_LENGTH> make_key(uint8_t seed)
{
    std::array<uint8_t, Mavsdk::SIGNING_KEY_LENGTH> key{};
    for (std::size_t i = 0; i < key.size(); ++i) {
        key[i] = static_cast<uint8_t>(seed + i);
    }
    return key;
}

Action::Result arm_and_disarm(Mavsdk& mavsdk_groundstation, Mavsdk& mavsdk_autopilot)
{
    auto action_server = ActionServer{mavsdk_autopilot.server_component()};
    action_server.set_armable(true, true);
    action_server.set_disarmable(true, true);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    EXPECT_TRUE(maybe_system);
    if (!maybe_system) {
        return Action::Result::NoSystem;
    }
    auto action = Action{maybe_system.value()};

    const auto result = action.arm();
    if (result != Action::Result::Success) {
        return result;
    }
    return action.disarm();
}

// Not in any dialect the C library knows, so only libmav can parse it.
// The ID is from the 43000-49999 range, which no mavlink dialect claims.
const char* custom_xml = R"(<?xml version="1.0"?>
<mavlink>
    <version>3</version>
    <dialect>0</dialect>
    <messages>
        <message id="44010" name="CUSTOM_SIGNING_TEST">
            <description>Test message for signed messages only libmav knows</description>
            <field type="uint32_t" name="test_id">Test identifier</field>
        </message>
    </messages>
</mavlink>)";

// Sends CUSTOM_SIGNING_TEST from sender and returns how many receiver got.
int send_custom_message(MavlinkDirectServer& sender, std::atomic<int>& received)
{
    const int before = received;
    MavlinkDirectServer::MavlinkMessage message;
    message.message_name = "CUSTOM_SIGNING_TEST";
    message.system_id = 1;
    message.component_id = 1;
    message.fields_json = R"({"test_id":42})";
    EXPECT_EQ(sender.send_message(message), MavlinkDirectServer::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    return received - before;
}

} // namespace

TEST(Signing, CommandRoundtrip)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_groundstation.enable_signing(make_key(1));
    mavsdk_autopilot.enable_signing(make_key(1));
    EXPECT_TRUE(mavsdk_groundstation.is_signing_enabled());

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17100"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17100"), ConnectionResult::Success);

    EXPECT_EQ(arm_and_disarm(mavsdk_groundstation, mavsdk_autopilot), Action::Result::Success);
}

TEST(Signing, WrongKeyRejected)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_groundstation.enable_signing(make_key(1));
    mavsdk_autopilot.enable_signing(make_key(2));

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17101"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17101"), ConnectionResult::Success);

    // Not even the heartbeats get through.
    EXPECT_FALSE(mavsdk_groundstation.first_autopilot(3.0));
}

TEST(Signing, UnsignedRejected)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.enable_signing(make_key(1));

    // The ground station takes the signed messages, so it discovers the autopilot,
    // but the autopilot drops its unsigned commands.
    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17102"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17102"), ConnectionResult::Success);

    EXPECT_EQ(arm_and_disarm(mavsdk_groundstation, mavsdk_autopilot), Action::Result::Timeout);
}

TEST(Signing, DisableAgain)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_groundstation.enable_signing(make_key(1));
    mavsdk_groundstation.disable_signing();
    EXPECT_FALSE(mavsdk_groundstation.is_signing_enabled());

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17103"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17103"), ConnectionResult::Success);

    EXPECT_EQ(arm_and_disarm(mavsdk_groundstation, mavsdk_autopilot), Action::Result::Success);
}

TEST(Signing, Sysid32CommandRoundtrip)
{
    // 10.0.0.1 and 10.0.0.2
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{0x0A000002, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{0x0A000001, MAV_COMP_ID_AUTOPILOT1, true}};
    mavsdk_groundstation.enable_signing(make_key(1));
    mavsdk_autopilot.enable_signing(make_key(1));

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17104"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17104"), ConnectionResult::Success);

    EXPECT_EQ(arm_and_disarm(mavsdk_groundstation, mavsdk_autopilot), Action::Result::Success);
}

TEST(Signing, OutgoingBytesSigned)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk.enable_signing(make_key(1));
    ASSERT_EQ(mavsdk.add_any_connection("raw://"), ConnectionResult::Success);

    auto prom = std::make_shared<std::promise<mavlink_message_t>>();
    auto fut = prom->get_future();
    auto flag = std::make_shared<std::once_flag>();

    auto on_bytes = [prom, flag](const char* bytes, size_t length) {
        mavlink_message_t buffer{};
        mavlink_status_t buffer_status{};
        mavlink_message_t message{};
        mavlink_status_t status{};

        for (size_t i = 0; i < length; ++i) {
            if (mavlink_frame_char_buffer(
                    &buffer, &buffer_status, static_cast<uint8_t>(bytes[i]), &message, &status) ==
                MAVLINK_FRAMING_OK) {
                std::call_once(*flag, [&]() { prom->set_value(message); });
            }
        }
    };
    auto handle = mavsdk.subscribe_raw_bytes_to_be_sent(on_bytes);

    // The heartbeat goes out once a second.
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(3)), std::future_status::ready);
    const auto message = fut.get();
    EXPECT_NE(message.incompat_flags & MAVLINK_IFLAG_SIGNED, 0);

    mavsdk.unsubscribe_raw_bytes_to_be_sent(handle);
}

// MavlinkDirect gets its messages from a second parser, it must not see what signing rejected.
TEST(Signing, MavlinkDirectOnlyGetsAccepted)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_groundstation.enable_signing(make_key(1));
    mavsdk_autopilot.enable_signing(make_key(1));

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17105"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17105"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    auto receiver = MavlinkDirect{maybe_system.value()};
    auto sender = MavlinkDirectServer{mavsdk_autopilot.server_component()};

    std::atomic<int> received{0};
    auto handle = receiver.subscribe_message(
        "GLOBAL_POSITION_INT", [&received](MavlinkDirect::MavlinkMessage) { ++received; });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    MavlinkDirectServer::MavlinkMessage message;
    message.message_name = "GLOBAL_POSITION_INT";
    message.system_id = 1;
    message.component_id = 1;
    message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":85456490,"alt":488000,"relative_alt":10000,"vx":0,"vy":0,"vz":0,"hdg":0})";

    ASSERT_EQ(sender.send_message(message), MavlinkDirectServer::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    EXPECT_EQ(received, 1);

    // Now the autopilot sends unsigned, which the ground station drops.
    mavsdk_autopilot.disable_signing();
    ASSERT_EQ(sender.send_message(message), MavlinkDirectServer::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    EXPECT_EQ(received, 1);

    receiver.unsubscribe_message(handle);
}

TEST(Signing, CustomMessageOnlyGetsAccepted)
{
    // The C parser can't check the CRC of a message it doesn't know, but it can check the
    // signature, which doesn't depend on the dialect.
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_groundstation.enable_signing(make_key(1));
    mavsdk_autopilot.enable_signing(make_key(1));

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17118"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17118"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    auto receiver = MavlinkDirect{maybe_system.value()};
    auto sender = MavlinkDirectServer{mavsdk_autopilot.server_component()};
    ASSERT_EQ(receiver.load_custom_xml(custom_xml), MavlinkDirect::Result::Success);
    ASSERT_EQ(sender.load_custom_xml(custom_xml), MavlinkDirectServer::Result::Success);

    std::atomic<int> received{0};
    auto handle = receiver.subscribe_message(
        "CUSTOM_SIGNING_TEST", [&received](MavlinkDirect::MavlinkMessage message) {
            EXPECT_NE(message.fields_json.find("\"test_id\":42"), std::string::npos);
            ++received;
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(send_custom_message(sender, received), 1);

    // Now the autopilot sends unsigned, which the ground station drops.
    mavsdk_autopilot.disable_signing();
    EXPECT_EQ(send_custom_message(sender, received), 0);

    // And signed with the wrong key.
    mavsdk_autopilot.enable_signing(make_key(2));
    EXPECT_EQ(send_custom_message(sender, received), 0);

    receiver.unsubscribe_message(handle);
}

namespace {

// Autopilot -> router -> ground station, the router forwarding between its two connections.
struct RoutedSetup {
    Mavsdk groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk router{Mavsdk::Configuration{ComponentType::CompanionComputer}};
    Mavsdk autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    void connect(int port)
    {
        ASSERT_EQ(
            groundstation.add_any_connection("udpin://0.0.0.0:" + std::to_string(port)),
            ConnectionResult::Success);
        ASSERT_EQ(
            router.add_any_connection(
                "udpout://127.0.0.1:" + std::to_string(port), ForwardingOption::ForwardingOn),
            ConnectionResult::Success);
        ASSERT_EQ(
            router.add_any_connection(
                "udpin://0.0.0.0:" + std::to_string(port + 1), ForwardingOption::ForwardingOn),
            ConnectionResult::Success);
        ASSERT_EQ(
            autopilot.add_any_connection("udpout://127.0.0.1:" + std::to_string(port + 1)),
            ConnectionResult::Success);
    }
};

} // namespace

TEST(Signing, RouterForwardsSigned)
{
    RoutedSetup setup;
    setup.groundstation.enable_signing(make_key(1));
    setup.router.enable_signing(make_key(1));
    setup.autopilot.enable_signing(make_key(1));
    setup.connect(17110);

    EXPECT_EQ(arm_and_disarm(setup.groundstation, setup.autopilot), Action::Result::Success);
}

TEST(Signing, RouterWithoutSigningForwardsSigned)
{
    // The signatures are end to end, the router doesn't need the key.
    RoutedSetup setup;
    setup.groundstation.enable_signing(make_key(1));
    setup.autopilot.enable_signing(make_key(1));
    setup.connect(17112);

    EXPECT_EQ(arm_and_disarm(setup.groundstation, setup.autopilot), Action::Result::Success);
}

TEST(Signing, RouterDropsRejected)
{
    // The ground station doesn't check signatures, so the router has to drop what it rejects.
    RoutedSetup setup;
    setup.router.enable_signing(make_key(1));
    setup.autopilot.enable_signing(make_key(2));
    setup.connect(17114);

    EXPECT_FALSE(setup.groundstation.first_autopilot(3.0));
}

TEST(Signing, RouterForwardsSignedCustomMessage)
{
    // The router doesn't know the message, but it checks the signature before forwarding.
    RoutedSetup setup;
    setup.groundstation.enable_signing(make_key(1));
    setup.router.enable_signing(make_key(1));
    setup.autopilot.enable_signing(make_key(1));
    setup.connect(17120);

    auto maybe_system = setup.groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    auto receiver = MavlinkDirect{maybe_system.value()};
    auto sender = MavlinkDirectServer{setup.autopilot.server_component()};
    ASSERT_EQ(receiver.load_custom_xml(custom_xml), MavlinkDirect::Result::Success);
    ASSERT_EQ(sender.load_custom_xml(custom_xml), MavlinkDirectServer::Result::Success);

    std::atomic<int> received{0};
    auto handle = receiver.subscribe_message(
        "CUSTOM_SIGNING_TEST", [&received](MavlinkDirect::MavlinkMessage) { ++received; });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EXPECT_EQ(send_custom_message(sender, received), 1);

    // The ground station stops checking, so it's the router that has to drop unsigned ones.
    setup.groundstation.disable_signing();
    setup.autopilot.disable_signing();
    EXPECT_EQ(send_custom_message(sender, received), 0);

    receiver.unsubscribe_message(handle);
}

TEST(Signing, OldTimestampOnlyGetsHeartbeatBack)
{
    // A message signed with our key but too old might be a replay with a spoofed source, so
    // the sender must not become a destination: it only gets a heartbeat back, at most once
    // a second, so that a vehicle with its clock behind can catch up.
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    const auto key = make_key(1);
    mavsdk_groundstation.enable_signing(key);
    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://127.0.0.1:17116"),
        ConnectionResult::Success);

    // Signed with the right key, two minutes ago.
    mavlink_signing_t old_signing{};
    std::memcpy(old_signing.secret_key, key.data(), key.size());
    old_signing.flags = MAVLINK_SIGNING_FLAG_SIGN_OUTGOING;
    // In 10 us since 1 January 2015.
    const auto now = std::chrono::duration_cast<std::chrono::microseconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                             .count() /
                         10 -
                     1420070400LL * 100000LL;
    old_signing.timestamp = static_cast<uint64_t>(now) - 2 * 60 * 100000ULL;
    constexpr uint8_t channel = MAVLINK_COMM_NUM_BUFFERS - 1;
    auto* status = mavlink_get_channel_status(channel);
    status->signing = &old_signing;
    mavlink_message_t heartbeat{};
    mavlink_msg_heartbeat_pack_chan(
        1, 1, channel, &heartbeat, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_PX4, 0, 0, MAV_STATE_ACTIVE);
    status->signing = nullptr;
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    const auto len = mavlink_msg_to_send_buffer(buffer, &heartbeat);

    asio::io_context io_context;
    asio::ip::udp::socket socket(
        io_context, asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 0));
    const asio::ip::udp::endpoint groundstation(asio::ip::make_address("127.0.0.1"), 17116);

    std::array<uint8_t, 2048> receive_buffer{};
    asio::ip::udp::endpoint sender;
    unsigned signed_heartbeats = 0;
    unsigned others = 0;
    std::function<void()> receive = [&]() {
        socket.async_receive_from(
            asio::buffer(receive_buffer), sender, [&](const asio::error_code& ec, std::size_t n) {
                if (ec) {
                    return;
                }
                mavlink_message_t message{};
                mavlink_status_t parse_status{};
                bool parsed = false;
                for (std::size_t i = 0; i < n && !parsed; ++i) {
                    parsed = mavlink_parse_char(
                                 MAVLINK_COMM_NUM_BUFFERS - 2,
                                 receive_buffer[i],
                                 &message,
                                 &parse_status) != 0;
                }
                if (parsed && message.msgid == MAVLINK_MSG_ID_HEARTBEAT &&
                    (message.incompat_flags & MAVLINK_IFLAG_SIGNED) != 0) {
                    ++signed_heartbeats;
                } else {
                    ++others;
                }
                receive();
            });
    };
    receive();

    // Several in a row only get one heartbeat back.
    for (int i = 0; i < 5; ++i) {
        socket.send_to(asio::buffer(buffer, len), groundstation);
    }
    io_context.run_for(std::chrono::milliseconds(3500));

    // As a destination, it would have gotten a heartbeat every second.
    EXPECT_EQ(signed_heartbeats, 1u);
    EXPECT_EQ(others, 0u);
}
