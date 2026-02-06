#include "log.h"
#include "mavsdk.h"
#include "plugins/mavlink_direct/mavlink_direct.h"
#include <chrono>
#include <thread>
#include <future>
#include <atomic>
#include <gtest/gtest.h>
#include <json/json.h>

using namespace mavsdk;

TEST(SystemTest, MavlinkDirectForwardingKnownMessage)
{
    // Test 3-instance message forwarding with a known standard message
    // Instance 1 (Sender) -> Instance 2 (Forwarder) -> Instance 3 (Receiver)

    // Instance 1: Sender (autopilot)
    Mavsdk mavsdk_sender{Mavsdk::Configuration{ComponentType::Autopilot}};

    // Instance 2: Forwarder (router with forwarding enabled)
    Mavsdk mavsdk_forwarder{Mavsdk::Configuration{ComponentType::CompanionComputer}};

    // Instance 3: Receiver (ground station)
    Mavsdk mavsdk_receiver{Mavsdk::Configuration{ComponentType::GroundStation}};

    // Set up connections: Sender -> Forwarder -> Receiver
    ASSERT_EQ(
        mavsdk_sender.add_any_connection("udpout://127.0.0.1:17010"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_forwarder.add_any_connection(
            "udpin://0.0.0.0:17010", ForwardingOption::ForwardingOn),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_forwarder.add_any_connection(
            "udpout://127.0.0.1:17011", ForwardingOption::ForwardingOn),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_receiver.add_any_connection("udpin://0.0.0.0:17011"), ConnectionResult::Success);

    LogInfo() << "Waiting for connections to establish...";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Receiver discovers the sender system (autopilot) through the forwarder
    auto receiver_target_system = mavsdk_receiver.first_autopilot(10.0);
    ASSERT_TRUE(receiver_target_system.has_value());
    auto system = receiver_target_system.value();
    ASSERT_TRUE(system->has_autopilot());

    // Sender waits to discover ground station systems
    while (mavsdk_sender.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Get sender's view of the receiver systems (for sending)
    auto sender_target_system = mavsdk_sender.systems().at(0);

    // Create MavlinkDirect instances
    auto sender_mavlink_direct = MavlinkDirect{sender_target_system};
    auto receiver_mavlink_direct = MavlinkDirect{system};

    // Set up receiver subscription for a known message (GLOBAL_POSITION_INT)
    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    auto handle = receiver_mavlink_direct.subscribe_message(
        "GLOBAL_POSITION_INT", [&prom](const MavlinkDirect::MavlinkMessage& message) {
            LogInfo() << "Receiver got forwarded known message: " << message.fields_json;
            prom.set_value(message);
        });

    // Send known message from sender
    LogInfo() << "Sending known GLOBAL_POSITION_INT message through forwarder...";
    MavlinkDirect::MavlinkMessage test_message;
    test_message.message_name = "GLOBAL_POSITION_INT";
    test_message.system_id = 1;
    test_message.component_id = 1;
    test_message.target_system_id = 0;
    test_message.target_component_id = 0;
    test_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    EXPECT_EQ(MavlinkDirect::Result::Success, sender_mavlink_direct.send_message(test_message));

    // Wait for message to be received through the forwarder
    auto wait_result = fut.wait_for(std::chrono::seconds(3));

    ASSERT_EQ(wait_result, std::future_status::ready);

    auto received_message = fut.get();

    // Verify the forwarded message
    EXPECT_EQ(received_message.message_name, "GLOBAL_POSITION_INT");
    EXPECT_EQ(received_message.system_id, 1);
    EXPECT_EQ(received_message.component_id, 1);

    // Parse and verify JSON content
    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message.fields_json, json));

    // The JSON format may vary but should contain the message information
    // For now, just verify it's not empty and contains the message name
    EXPECT_FALSE(received_message.fields_json.empty());
    EXPECT_TRUE(received_message.fields_json.find("GLOBAL_POSITION_INT") != std::string::npos);

    receiver_mavlink_direct.unsubscribe_message(handle);
}

TEST(SystemTest, MavlinkDirectForwardingUnknownMessage)
{
    // Test 3-instance message forwarding where intermediate instance doesn't know the custom
    // message Instance 1 (Sender) -> Instance 2 (Forwarder) -> Instance 3 (Receiver) Only Instance
    // 1 and 3 have the custom XML loaded, Instance 2 must forward blindly

    // Instance 1: Sender (autopilot that knows custom message)
    Mavsdk mavsdk_sender{Mavsdk::Configuration{ComponentType::Autopilot}};

    // Instance 2: Forwarder (router that doesn't know custom message but forwards all)
    Mavsdk mavsdk_forwarder{Mavsdk::Configuration{ComponentType::CompanionComputer}};

    // Instance 3: Receiver (ground station that knows custom message)
    Mavsdk mavsdk_receiver{Mavsdk::Configuration{ComponentType::GroundStation}};

    // Set up connections: Sender -> Forwarder -> Receiver
    ASSERT_EQ(
        mavsdk_sender.add_any_connection("udpout://127.0.0.1:17006"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_forwarder.add_any_connection(
            "udpin://0.0.0.0:17006", ForwardingOption::ForwardingOn),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_forwarder.add_any_connection(
            "udpout://127.0.0.1:17007", ForwardingOption::ForwardingOn),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_receiver.add_any_connection("udpin://0.0.0.0:17007"), ConnectionResult::Success);

    // Define custom message that only sender and receiver will know about
    std::string custom_xml = R"(<?xml version="1.0"?>
<mavlink>
    <version>3</version>
    <dialect>0</dialect>
    <messages>
        <message id="421" name="CUSTOM_FORWARD_TEST">
            <description>Test message for forwarding unknown messages</description>
            <field type="uint32_t" name="test_id">Unique test identifier</field>
            <field type="uint16_t" name="sequence">Sequence number</field>
            <field type="uint8_t" name="status">Status code</field>
            <field type="char[32]" name="message">Status message</field>
        </message>
    </messages>
</mavlink>)";

    LogInfo() << "Waiting for connections to establish...";
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Receiver discovers the sender system (autopilot) through the forwarder
    auto receiver_target_system = mavsdk_receiver.first_autopilot(10.0);
    ASSERT_TRUE(receiver_target_system.has_value());
    auto system = receiver_target_system.value();
    ASSERT_TRUE(system->has_autopilot());

    // Sender waits to discover ground station systems
    while (mavsdk_sender.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Get sender's view of the receiver systems (for sending)
    auto sender_target_system = mavsdk_sender.systems().at(0);

    // Create MavlinkDirect instances: sender uses discovered system, receiver uses discovered
    // autopilot
    auto sender_mavlink_direct = MavlinkDirect{sender_target_system};
    auto receiver_mavlink_direct = MavlinkDirect{system};

    // Load custom XML only on sender and receiver (NOT on forwarder)
    EXPECT_EQ(MavlinkDirect::Result::Success, sender_mavlink_direct.load_custom_xml(custom_xml));
    EXPECT_EQ(MavlinkDirect::Result::Success, receiver_mavlink_direct.load_custom_xml(custom_xml));

    // Set up receiver subscription - handle potential duplicate messages
    std::atomic<bool> message_received{false};
    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    auto handle = receiver_mavlink_direct.subscribe_message(
        "CUSTOM_FORWARD_TEST",
        [&prom, &message_received](const MavlinkDirect::MavlinkMessage& message) {
            LogInfo() << "Receiver got forwarded custom message: " << message.fields_json;
            if (!message_received.exchange(true)) {
                // Only set the promise once, even if we receive the message multiple times
                prom.set_value(message);
            }
        });

    // Send custom message from sender
    LogInfo() << "Sending custom message through forwarder...";
    MavlinkDirect::MavlinkMessage test_message;
    test_message.message_name = "CUSTOM_FORWARD_TEST";
    test_message.system_id = 1;
    test_message.component_id = 1;
    test_message.target_system_id = 0;
    test_message.target_component_id = 0;
    test_message.fields_json =
        R"({"test_id":12345,"sequence":1,"status":42,"message":"Hello through forwarder!"})";

    EXPECT_EQ(MavlinkDirect::Result::Success, sender_mavlink_direct.send_message(test_message));

    // Wait for message to be received through the forwarder
    LogInfo() << "Waiting for forwarded message...";
    auto wait_result = fut.wait_for(std::chrono::seconds(3));

    ASSERT_EQ(wait_result, std::future_status::ready);

    auto received_message = fut.get();

    // Verify the forwarded message
    EXPECT_EQ(received_message.message_name, "CUSTOM_FORWARD_TEST");
    EXPECT_EQ(received_message.system_id, 1);
    EXPECT_EQ(received_message.component_id, 1);

    // Parse and verify JSON content
    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message.fields_json, json));

    EXPECT_EQ(json["test_id"].asUInt(), 12345u);
    EXPECT_EQ(json["sequence"].asUInt(), 1u);
    EXPECT_EQ(json["status"].asUInt(), 42u);
    EXPECT_EQ(json["message"].asString(), "Hello through forwarder!");

    receiver_mavlink_direct.unsubscribe_message(handle);
}
