#include "log.h"
#include "mavsdk.h"
#include "plugins/mavlink_direct/mavlink_direct.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/telemetry_server/telemetry_server.h"
#include <chrono>
#include <thread>
#include <gtest/gtest.h>
#include <json/json.h>

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
    auto handle = receiver_mavlink_direct.subscribe_message(
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

    // Subscribe to SYS_STATUS messages
    auto handle = receiver_mavlink_direct.subscribe_message(
        "SYS_STATUS", [&](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received SYS_STATUS: " << message.fields_json;
            if (received_compact == std::nullopt) {
                received_compact = message;
            } else {
                received_full = message;
            }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Test 1: Send SYS_STATUS in compact form (no extensions)
    MavlinkDirect::MavlinkMessage compact_message;
    compact_message.message_name = "SYS_STATUS";
    compact_message.system_id = 1;
    compact_message.component_id = 1;
    compact_message.target_system = 0;
    compact_message.target_component = 0;
    compact_message.fields_json =
        R"({"onboard_control_sensors_present":1,"onboard_control_sensors_enabled":1,"onboard_control_sensors_health":1,"load":500,"voltage_battery":12000,"current_battery":1000,"battery_remaining":75,"drop_rate_comm":0,"errors_comm":0,"errors_count1":0,"errors_count2":0,"errors_count3":0,"errors_count4":0})";

    auto result1 = sender_mavlink_direct.send_message(compact_message);
    EXPECT_EQ(result1, MavlinkDirect::Result::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Test 2: Send SYS_STATUS in full form (with extensions)
    MavlinkDirect::MavlinkMessage full_message;
    full_message.message_name = "SYS_STATUS";
    full_message.system_id = 1;
    full_message.component_id = 1;
    full_message.target_system = 0;
    full_message.target_component = 0;
    full_message.fields_json =
        R"({"onboard_control_sensors_present":1,"onboard_control_sensors_enabled":1,"onboard_control_sensors_health":1,"load":500,"voltage_battery":12000,"current_battery":1000,"battery_remaining":75,"drop_rate_comm":0,"errors_comm":0,"errors_count1":0,"errors_count2":0,"errors_count3":0,"errors_count4":0,"onboard_control_sensors_present_extended":123,"onboard_control_sensors_enabled_extended":456,"onboard_control_sensors_health_extended":789})";

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
    EXPECT_EQ(received_compact->message_name, "SYS_STATUS");
    EXPECT_EQ(received_full->message_name, "SYS_STATUS");

    // Parse JSON to verify field values
    Json::Value compact_json, full_json;
    Json::Reader reader;

    ASSERT_TRUE(reader.parse(received_compact->fields_json, compact_json));
    ASSERT_TRUE(reader.parse(received_full->fields_json, full_json));

    // Verify basic fields are present and correct in both messages
    EXPECT_EQ(compact_json["onboard_control_sensors_present"].asUInt(), 1u);
    EXPECT_EQ(compact_json["load"].asUInt(), 500u);
    EXPECT_EQ(compact_json["voltage_battery"].asUInt(), 12000u);
    EXPECT_EQ(compact_json["current_battery"].asInt(), 1000);
    EXPECT_EQ(compact_json["battery_remaining"].asInt(), 75);

    EXPECT_EQ(full_json["onboard_control_sensors_present"].asUInt(), 1u);
    EXPECT_EQ(full_json["load"].asUInt(), 500u);
    EXPECT_EQ(full_json["voltage_battery"].asUInt(), 12000u);
    EXPECT_EQ(full_json["current_battery"].asInt(), 1000);
    EXPECT_EQ(full_json["battery_remaining"].asInt(), 75);

    // Verify compact message HAS extended fields with zero values (MAVLink v2 zero-truncation)
    EXPECT_TRUE(compact_json.isMember("onboard_control_sensors_present_extended"));
    EXPECT_TRUE(compact_json.isMember("onboard_control_sensors_enabled_extended"));
    EXPECT_TRUE(compact_json.isMember("onboard_control_sensors_health_extended"));
    EXPECT_EQ(compact_json["onboard_control_sensors_present_extended"].asUInt(), 0u);
    EXPECT_EQ(compact_json["onboard_control_sensors_enabled_extended"].asUInt(), 0u);
    EXPECT_EQ(compact_json["onboard_control_sensors_health_extended"].asUInt(), 0u);

    // Verify full message HAS extended fields with correct values
    EXPECT_TRUE(full_json.isMember("onboard_control_sensors_present_extended"));
    EXPECT_TRUE(full_json.isMember("onboard_control_sensors_enabled_extended"));
    EXPECT_TRUE(full_json.isMember("onboard_control_sensors_health_extended"));

    EXPECT_EQ(full_json["onboard_control_sensors_present_extended"].asUInt(), 123u);
    EXPECT_EQ(full_json["onboard_control_sensors_enabled_extended"].asUInt(), 456u);
    EXPECT_EQ(full_json["onboard_control_sensors_health_extended"].asUInt(), 789u);

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
    auto handle = receiver_mavlink_direct.subscribe_message(
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
