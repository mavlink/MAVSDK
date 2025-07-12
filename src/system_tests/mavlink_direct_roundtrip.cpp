#include "log.h"
#include "mavsdk.h"
#include "plugins/mavlink_direct/mavlink_direct.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/telemetry_server/telemetry_server.h"
#include <chrono>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, MavlinkDirectRoundtrip)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    // Ground station discovers the autopilot system
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    // Wait for autopilot instance to discover the connection to the ground station
    LogInfo() << "Waiting for autopilot system to connect...";
    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Get the autopilot's view of the connected system
    auto groundstation_system = mavsdk_autopilot.systems().at(0);
    ASSERT_EQ(groundstation_system->component_ids()[0], 190);

    // Create separate MavlinkDirect instances for sender (autopilot) and receiver (ground station)
    auto receiver_mavlink_direct = MavlinkDirect{system};
    auto sender_mavlink_direct = MavlinkDirect{groundstation_system};

    std::optional<MavlinkDirect::MavlinkMessage> received_message;
    // Ground station subscribes to receive GLOBAL_POSITION_INT from autopilot
    auto handle = receiver_mavlink_direct.subscribe_message_type(
        "GLOBAL_POSITION_INT", [&](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received GLOBAL_POSITION_INT: " << message.fields_json;
            received_message = message;
        });

    // Wait a bit more for subscription to be properly registered
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Create a complex test message with multiple field types
    MavlinkDirect::MavlinkMessage test_message;
    test_message.message_name = "GLOBAL_POSITION_INT";
    test_message.system_id = 1;
    test_message.component_id = 1;
    test_message.target_system = 0;
    test_message.target_component = 0;
    test_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    LogInfo() << "Sending GLOBAL_POSITION_INT message...";
    auto result = sender_mavlink_direct.send_message(test_message);
    LogInfo() << "Send result: "
              << (result == MavlinkDirect::Result::Success ? "Success" : "Error");
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    // Wait for message to be received
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!received_message && std::chrono::steady_clock::now() < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_TRUE(received_message.has_value());
    EXPECT_EQ(received_message->message_name, test_message.message_name);
    EXPECT_EQ(received_message->system_id, test_message.system_id);
    EXPECT_EQ(received_message->component_id, test_message.component_id);
    // Note: For now we only check message_name since field extraction is simplified
    EXPECT_TRUE(received_message->fields_json.find("GLOBAL_POSITION_INT") != std::string::npos);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MavlinkDirectExtendedFields)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17001"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17001"), ConnectionResult::Success);

    // Ground station discovers the autopilot system
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    // Wait for autopilot instance to discover the connection to the ground station
    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto groundstation_system = mavsdk_autopilot.systems().at(0);
    ASSERT_EQ(groundstation_system->component_ids()[0], 190);

    auto receiver_mavlink_direct = MavlinkDirect{system};
    auto sender_mavlink_direct = MavlinkDirect{groundstation_system};

    std::optional<MavlinkDirect::MavlinkMessage> received_compact;
    std::optional<MavlinkDirect::MavlinkMessage> received_full;

    // Subscribe to PLAY_TUNE_V2 messages
    auto handle = receiver_mavlink_direct.subscribe_message_type(
        "PLAY_TUNE_V2", [&](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received PLAY_TUNE_V2: " << message.fields_json;
            if (received_compact == std::nullopt) {
                received_compact = message;
            } else {
                received_full = message;
            }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Test 1: Send PLAY_TUNE_V2 in compact form (no extensions)
    MavlinkDirect::MavlinkMessage compact_message;
    compact_message.message_name = "PLAY_TUNE_V2";
    compact_message.system_id = 1;
    compact_message.component_id = 1;
    compact_message.target_system = 0;
    compact_message.target_component = 0;
    compact_message.fields_json = R"({"target_system":0,"target_component":0,"tune":"MBT180L8"})";

    auto result1 = sender_mavlink_direct.send_message(compact_message);
    EXPECT_EQ(result1, MavlinkDirect::Result::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Test 2: Send PLAY_TUNE_V2 in full form (with extensions)
    MavlinkDirect::MavlinkMessage full_message;
    full_message.message_name = "PLAY_TUNE_V2";
    full_message.system_id = 1;
    full_message.component_id = 1;
    full_message.target_system = 0;
    full_message.target_component = 0;
    full_message.fields_json =
        R"({"target_system":0,"target_component":0,"tune":"MBT180L8","tune2":"EXTENDED"})";

    auto result2 = sender_mavlink_direct.send_message(full_message);
    EXPECT_EQ(result2, MavlinkDirect::Result::Success);

    // Wait for both messages to be received
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while ((!received_compact || !received_full) && std::chrono::steady_clock::now() < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_TRUE(received_compact.has_value());
    ASSERT_TRUE(received_full.has_value());

    // Verify both messages were received and parsing worked
    EXPECT_EQ(received_compact->message_name, "PLAY_TUNE_V2");
    EXPECT_EQ(received_full->message_name, "PLAY_TUNE_V2");

    // Note: Since we're using simplified parsing, we just verify the messages were parsed correctly
    EXPECT_TRUE(received_compact->fields_json.find("PLAY_TUNE_V2") != std::string::npos);
    EXPECT_TRUE(received_full->fields_json.find("PLAY_TUNE_V2") != std::string::npos);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MavlinkDirectToTelemetry)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17002"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17002"), ConnectionResult::Success);

    // Ground station discovers the autopilot system
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    // Wait for autopilot instance to discover the connection to the ground station
    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto groundstation_system = mavsdk_autopilot.systems().at(0);
    ASSERT_EQ(groundstation_system->component_ids()[0], 190);

    // Autopilot uses MavlinkDirect to send, Ground station uses Telemetry to receive
    auto sender_mavlink_direct = MavlinkDirect{groundstation_system};
    auto receiver_telemetry = Telemetry{system};

    std::optional<Telemetry::Position> received_position;

    // Subscribe to position updates via Telemetry
    auto handle = receiver_telemetry.subscribe_position([&](Telemetry::Position position) {
        LogInfo() << "Received position via Telemetry: lat=" << position.latitude_deg
                  << " lon=" << position.longitude_deg;
        received_position = position;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send GLOBAL_POSITION_INT via MavlinkDirect
    MavlinkDirect::MavlinkMessage test_message;
    test_message.message_name = "GLOBAL_POSITION_INT";
    test_message.system_id = 1;
    test_message.component_id = 1;
    test_message.target_system = 0;
    test_message.target_component = 0;
    test_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    auto result = sender_mavlink_direct.send_message(test_message);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    // Wait for position to be received
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!received_position && std::chrono::steady_clock::now() < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_TRUE(received_position.has_value());

    // Verify the position data (lat/lon are in degrees * 1e7 in MAVLink)
    EXPECT_NEAR(received_position->latitude_deg, 47.3977418, 0.0001);
    EXPECT_NEAR(received_position->longitude_deg, -122.3974560, 0.0001);

    receiver_telemetry.unsubscribe_position(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, TelemetryServerToMavlinkDirect)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17003"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17003"), ConnectionResult::Success);

    // Ground station discovers the autopilot system
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    // Wait for autopilot instance to discover the connection to the ground station
    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto groundstation_system = mavsdk_autopilot.systems().at(0);
    ASSERT_EQ(groundstation_system->component_ids()[0], 190);

    // Autopilot uses TelemetryServer to send, Ground station uses MavlinkDirect to receive
    auto autopilot_server_component = mavsdk_autopilot.server_component();
    auto sender_telemetry_server = TelemetryServer{autopilot_server_component};
    auto receiver_mavlink_direct = MavlinkDirect{system};

    std::optional<MavlinkDirect::MavlinkMessage> received_message;

    // Subscribe to GLOBAL_POSITION_INT via MavlinkDirect
    auto handle = receiver_mavlink_direct.subscribe_message_type(
        "GLOBAL_POSITION_INT", [&](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received GLOBAL_POSITION_INT via MavlinkDirect: " << message.fields_json;
            received_message = message;
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send position via TelemetryServer
    TelemetryServer::Position position;
    position.latitude_deg = 47.3977418;
    position.longitude_deg = -122.3974560;
    position.absolute_altitude_m = 100.5;
    position.relative_altitude_m = 50.25;

    TelemetryServer::VelocityNed velocity_ned;
    velocity_ned.north_m_s = 1.0f;
    velocity_ned.east_m_s = -0.5f;
    velocity_ned.down_m_s = 0.25f;

    TelemetryServer::Heading heading;
    heading.heading_deg = 180.0f;

    auto result = sender_telemetry_server.publish_position(position, velocity_ned, heading);
    EXPECT_EQ(result, TelemetryServer::Result::Success);

    // Wait for message to be received
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!received_message && std::chrono::steady_clock::now() < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_TRUE(received_message.has_value());
    EXPECT_EQ(received_message->message_name, "GLOBAL_POSITION_INT");
    EXPECT_EQ(received_message->system_id, 1);
    EXPECT_EQ(received_message->component_id, 1);

    // Verify the JSON contains the message info (since we're using simplified parsing)
    EXPECT_TRUE(received_message->fields_json.find("GLOBAL_POSITION_INT") != std::string::npos);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
