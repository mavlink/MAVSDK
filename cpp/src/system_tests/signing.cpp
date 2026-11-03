#include "log.hpp"
#include "mavsdk.hpp"
#include "mavlink_include.hpp"
#include "plugins/action/action.hpp"
#include "plugins/action_server/action_server.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include <atomic>
#include <chrono>
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

    // Heartbeats are accepted unsigned, so discovery works either way.
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

    EXPECT_EQ(arm_and_disarm(mavsdk_groundstation, mavsdk_autopilot), Action::Result::Timeout);
}

TEST(Signing, UnsignedRejected)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.enable_signing(make_key(1));

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
