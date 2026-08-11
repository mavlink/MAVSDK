#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/action/action.hpp"
#include "plugins/action_server/action_server.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include <chrono>
#include <future>
#include <thread>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace mavsdk;

// 32 bit system IDs, as proposed in ArduPilot/pymavlink#1229. A system ID
// above 255 no longer fits the MAVLink 2 header's single sysid byte, so the
// sender sets MAVLINK_IFLAG_SYSID32 and the header grows by 3 bytes. A target
// above 255 likewise moves into an extended header behind
// MAVLINK_IFLAG_TARGETTED.
//
// 0x0A000001 is 10.0.0.1, which is the point of the feature: an IPv4 address
// used directly as a system ID.
static constexpr uint32_t autopilot_sysid = 0x0A000001;
static constexpr uint32_t groundstation_sysid = 0x0A000002;

// The 32 bit cases below need the MAVLink C library to carry a wide system ID,
// which arrives with the generator changes from that PR. Until MAVLINK_HASH is
// bumped to a build that has them, only the 8 bit compatibility case can run:
// the rest would be testing a truncation we already know is there.
#ifdef MAVLINK_IFLAG_TARGETTED

TEST(Sysid32, Discovery)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17010"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17010"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    // The full 32 bit value has to survive discovery. Truncating would report
    // system 1 here, which is a different (and very common) system.
    EXPECT_EQ(maybe_system.value()->get_system_id(), autopilot_sysid);
}

TEST(Sysid32, CommandRoundtrip)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17011"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17011"), ConnectionResult::Success);

    auto action_server = ActionServer{mavsdk_autopilot.server_component()};
    action_server.set_armable(true, true);
    action_server.set_disarmable(true, true);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto action = Action{maybe_system.value()};

    // A command is targeted, so this only works if the target system ID makes
    // it into the extended header and the ack finds its way back. Both sides
    // route on the 32 bit value.
    EXPECT_EQ(action.arm(), Action::Result::Success);
    EXPECT_EQ(action.disarm(), Action::Result::Success);
}

TEST(Sysid32, MavlinkDirectRoundtrip)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17012"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17012"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    auto receiver = MavlinkDirect{maybe_system.value()};
    auto sender = MavlinkDirectServer{mavsdk_autopilot.server_component()};

    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    auto handle = receiver.subscribe_message(
        "GLOBAL_POSITION_INT",
        [&prom](MavlinkDirect::MavlinkMessage message) { prom.set_value(message); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    MavlinkDirectServer::MavlinkMessage message;
    message.message_name = "GLOBAL_POSITION_INT";
    message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,)"
        R"("relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    ASSERT_EQ(sender.send_message(message), MavlinkDirectServer::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    auto received = fut.get();

    EXPECT_EQ(received.system_id, autopilot_sysid);
    EXPECT_EQ(nlohmann::json::parse(received.fields_json)["lat"], 473977418);

    receiver.unsubscribe_message(handle);
}

TEST(Sysid32, TargetAboveEightBits)
{
    Mavsdk mavsdk_groundstation{
        Mavsdk::Configuration{groundstation_sysid, MAV_COMP_ID_MISSIONPLANNER, false}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{autopilot_sysid, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17013"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17013"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);

    auto receiver = MavlinkDirectServer{mavsdk_autopilot.server_component()};
    auto sender = MavlinkDirect{maybe_system.value()};

    auto prom = std::promise<MavlinkDirectServer::MavlinkMessage>();
    auto fut = prom.get_future();

    auto handle = receiver.subscribe_message(
        "COMMAND_LONG",
        [&prom](MavlinkDirectServer::MavlinkMessage message) { prom.set_value(message); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // The payload's target_system field is only 8 bits wide, so this target
    // has to travel in the extended header instead. Were it truncated it would
    // arrive as 1, and were it zeroed it would look like a broadcast.
    MavlinkDirect::MavlinkMessage message;
    message.message_name = "COMMAND_LONG";
    message.target_system_id = autopilot_sysid;
    message.target_component_id = MAV_COMP_ID_AUTOPILOT1;
    message.fields_json =
        R"({"command":400,"confirmation":0,"param1":1.0,"param2":0.0,"param3":0.0,)"
        R"("param4":0.0,"param5":0.0,"param6":0.0,"param7":0.0})";

    ASSERT_EQ(sender.send_message(message), MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    auto received = fut.get();

    EXPECT_EQ(received.system_id, groundstation_sysid);
    EXPECT_EQ(received.target_system_id, autopilot_sysid);
    EXPECT_EQ(received.target_component_id, MAV_COMP_ID_AUTOPILOT1);

    receiver.unsubscribe_message(handle);
}

#endif // MAVLINK_IFLAG_TARGETTED

TEST(Sysid32, EightBitPeerStillWorks)
{
    // A system ID that fits in 8 bits must not set any of the new incompat
    // flags, otherwise every peer that predates this feature drops our frames.
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{42, MAV_COMP_ID_AUTOPILOT1, true}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17014"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17014"), ConnectionResult::Success);

    auto prom = std::promise<void>();
    auto fut = prom.get_future();
    bool fulfilled = false;

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    EXPECT_EQ(maybe_system.value()->get_system_id(), 42);

    auto receiver = MavlinkDirect{maybe_system.value()};
    auto handle = receiver.subscribe_message(
        "HEARTBEAT", [&prom, &fulfilled](MavlinkDirect::MavlinkMessage message) {
            if (!fulfilled && message.system_id == 42) {
                fulfilled = true;
                prom.set_value();
            }
        });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    receiver.unsubscribe_message(handle);
}
