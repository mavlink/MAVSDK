#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include <chrono>
#include <thread>
#include <future>
#include <gtest/gtest.h>
#include <json/json.h>

using namespace mavsdk;

// Send a message from an autopilot server component (broadcast) and receive it on
// the ground station via the MavlinkDirect client plugin.
//
// Unlike the MavlinkDirect-to-MavlinkDirect tests, the sender side here does NOT
// need to discover the ground station as a system first: the server component can
// broadcast straight away.
TEST(MavlinkDirectServer, BroadcastToClient)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17020"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17020"), ConnectionResult::Success);

    // Ground station discovers the autopilot system (needed for the client receiver).
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    // The sender just uses the autopilot's own server component - no discovery dance.
    auto sender = MavlinkDirectServer{mavsdk_autopilot.server_component()};
    auto receiver = MavlinkDirect{system};

    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    auto handle = receiver.subscribe_message(
        "GLOBAL_POSITION_INT", [&prom](MavlinkDirect::MavlinkMessage message) {
            LogInfo("Received GLOBAL_POSITION_INT: {}", message.fields_json);
            prom.set_value(message);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    MavlinkDirectServer::MavlinkMessage test_message;
    test_message.message_name = "GLOBAL_POSITION_INT";
    test_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    LogInfo("Broadcasting GLOBAL_POSITION_INT from server component...");
    EXPECT_EQ(sender.send_message(test_message), MavlinkDirectServer::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_message = fut.get();
    EXPECT_EQ(received_message.message_name, "GLOBAL_POSITION_INT");
    // The autopilot sends as system 1, component 1 (MAV_COMP_ID_AUTOPILOT1).
    EXPECT_EQ(received_message.system_id, 1);
    EXPECT_EQ(received_message.component_id, 1);

    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message.fields_json, json));
    EXPECT_EQ(json["lat"].asInt(), 473977418);
    EXPECT_EQ(json["lon"].asInt(), -1223974560);

    receiver.unsubscribe_message(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Subscribe on the server component and receive a message sent by a client.
// The server receive path is not scoped to a single system.
TEST(MavlinkDirectServer, SubscribeFromClient)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17022"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17022"), ConnectionResult::Success);

    // The ground station discovers the autopilot so it can send via the client plugin.
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    // The autopilot subscribes on its server component - no discovery needed.
    auto receiver = MavlinkDirectServer{mavsdk_autopilot.server_component()};
    auto sender = MavlinkDirect{system};

    auto prom = std::promise<MavlinkDirectServer::MavlinkMessage>();
    auto fut = prom.get_future();

    auto handle = receiver.subscribe_message(
        "GLOBAL_POSITION_INT", [&prom](MavlinkDirectServer::MavlinkMessage message) {
            LogInfo("Server received GLOBAL_POSITION_INT: {}", message.fields_json);
            prom.set_value(message);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    MavlinkDirect::MavlinkMessage test_message;
    test_message.message_name = "GLOBAL_POSITION_INT";
    test_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    EXPECT_EQ(sender.send_message(test_message), MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_message = fut.get();
    EXPECT_EQ(received_message.message_name, "GLOBAL_POSITION_INT");
    // The ground station sends as system 245, component 190 (MAV_COMP_ID_MISSIONPLANNER).
    EXPECT_EQ(received_message.component_id, 190);

    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message.fields_json, json));
    EXPECT_EQ(json["lat"].asInt(), 473977418);

    receiver.unsubscribe_message(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

// Load a custom message definition through the server plugin and send it.
TEST(MavlinkDirectServer, LoadCustomXml)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17021"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17021"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto sender = MavlinkDirectServer{mavsdk_autopilot.server_component()};
    auto receiver = MavlinkDirect{system};

    std::string custom_xml = R"(<?xml version="1.0"?>
<mavlink>
    <version>3</version>
    <dialect>0</dialect>
    <messages>
        <message id="420" name="CUSTOM_TEST_MESSAGE">
            <description>A test custom message for LoadCustomXml</description>
            <field type="uint32_t" name="test_value">Test value field</field>
            <field type="uint16_t" name="counter">Counter field</field>
            <field type="uint8_t" name="status">Status byte</field>
        </message>
    </messages>
</mavlink>)";

    // Loading on the server affects the shared MessageSet for the whole instance.
    EXPECT_EQ(sender.load_custom_xml(custom_xml), MavlinkDirectServer::Result::Success);
    EXPECT_EQ(receiver.load_custom_xml(custom_xml), MavlinkDirect::Result::Success);

    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    auto handle = receiver.subscribe_message(
        "CUSTOM_TEST_MESSAGE", [&prom](MavlinkDirect::MavlinkMessage message) {
            LogInfo("Received CUSTOM_TEST_MESSAGE: {}", message.fields_json);
            prom.set_value(message);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    MavlinkDirectServer::MavlinkMessage custom_message;
    custom_message.message_name = "CUSTOM_TEST_MESSAGE";
    custom_message.fields_json = R"({"test_value":42,"counter":1337,"status":5})";

    EXPECT_EQ(sender.send_message(custom_message), MavlinkDirectServer::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_message = fut.get();
    EXPECT_EQ(received_message.message_name, "CUSTOM_TEST_MESSAGE");

    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message.fields_json, json));
    EXPECT_EQ(json["test_value"].asUInt(), 42u);
    EXPECT_EQ(json["counter"].asUInt(), 1337u);
    EXPECT_EQ(json["status"].asUInt(), 5u);

    receiver.unsubscribe_message(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
