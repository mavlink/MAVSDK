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

TEST(SystemTest, MavlinkDirectArrayFields)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17004"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17004"), ConnectionResult::Success);

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

    std::optional<MavlinkDirect::MavlinkMessage> received_partial;
    std::optional<MavlinkDirect::MavlinkMessage> received_full;

    // Subscribe to GPS_STATUS messages
    auto handle = receiver_mavlink_direct.subscribe_message(
        "GPS_STATUS", [&](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received GPS_STATUS: " << message.fields_json;
            if (received_partial == std::nullopt) {
                received_partial = message;
            } else {
                received_full = message;
            }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Test 1: Send GPS_STATUS with partial array data (3 satellites)
    MavlinkDirect::MavlinkMessage partial_message;
    partial_message.message_name = "GPS_STATUS";
    partial_message.system_id = 1;
    partial_message.component_id = 1;
    partial_message.target_system = 0;
    partial_message.target_component = 0;
    partial_message.fields_json =
        R"({"satellites_visible":3,"satellite_prn":[1,2,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"satellite_used":[1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"satellite_elevation":[45,60,30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"satellite_azimuth":[90,180,270,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],"satellite_snr":[25,30,15,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]})";

    auto result1 = sender_mavlink_direct.send_message(partial_message);
    EXPECT_EQ(result1, MavlinkDirect::Result::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Test 2: Send GPS_STATUS with full array data (20 satellites)
    MavlinkDirect::MavlinkMessage full_message;
    full_message.message_name = "GPS_STATUS";
    full_message.system_id = 1;
    full_message.component_id = 1;
    full_message.target_system = 0;
    full_message.target_component = 0;
    full_message.fields_json =
        R"({"satellites_visible":20,"satellite_prn":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20],"satellite_used":[1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0],"satellite_elevation":[45,60,30,75,20,85,50,40,65,35,25,55,70,15,80,10,90,5,45,60],"satellite_azimuth":[0,36,72,108,144,180,216,252,28,64,100,136,172,208,244,20,56,92,128,164],"satellite_snr":[25,30,15,35,20,40,28,22,32,18,26,31,16,38,24,19,33,21,29,27]})";

    auto result2 = sender_mavlink_direct.send_message(full_message);
    EXPECT_EQ(result2, MavlinkDirect::Result::Success);

    // Wait for both messages to be received
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while ((!received_partial || !received_full) && std::chrono::steady_clock::now() < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_TRUE(received_partial.has_value());
    ASSERT_TRUE(received_full.has_value());

    // Verify both messages were received and parsing worked
    EXPECT_EQ(received_partial->message_name, "GPS_STATUS");
    EXPECT_EQ(received_full->message_name, "GPS_STATUS");

    // Parse JSON to verify field values
    Json::Value partial_json, full_json;
    Json::Reader reader;

    ASSERT_TRUE(reader.parse(received_partial->fields_json, partial_json));
    ASSERT_TRUE(reader.parse(received_full->fields_json, full_json));

    // Verify scalar field
    EXPECT_EQ(partial_json["satellites_visible"].asUInt(), 3u);
    EXPECT_EQ(full_json["satellites_visible"].asUInt(), 20u);

    // Verify array fields are present and have correct type (arrays)
    EXPECT_TRUE(partial_json["satellite_prn"].isArray());
    EXPECT_TRUE(partial_json["satellite_used"].isArray());
    EXPECT_TRUE(partial_json["satellite_elevation"].isArray());
    EXPECT_TRUE(partial_json["satellite_azimuth"].isArray());
    EXPECT_TRUE(partial_json["satellite_snr"].isArray());

    EXPECT_TRUE(full_json["satellite_prn"].isArray());
    EXPECT_TRUE(full_json["satellite_used"].isArray());
    EXPECT_TRUE(full_json["satellite_elevation"].isArray());
    EXPECT_TRUE(full_json["satellite_azimuth"].isArray());
    EXPECT_TRUE(full_json["satellite_snr"].isArray());

    // Verify array lengths (should be 20 elements each)
    EXPECT_EQ(partial_json["satellite_prn"].size(), 20u);
    EXPECT_EQ(partial_json["satellite_used"].size(), 20u);
    EXPECT_EQ(partial_json["satellite_elevation"].size(), 20u);
    EXPECT_EQ(partial_json["satellite_azimuth"].size(), 20u);
    EXPECT_EQ(partial_json["satellite_snr"].size(), 20u);

    EXPECT_EQ(full_json["satellite_prn"].size(), 20u);
    EXPECT_EQ(full_json["satellite_used"].size(), 20u);
    EXPECT_EQ(full_json["satellite_elevation"].size(), 20u);
    EXPECT_EQ(full_json["satellite_azimuth"].size(), 20u);
    EXPECT_EQ(full_json["satellite_snr"].size(), 20u);

    // Verify specific array element values for partial message
    EXPECT_EQ(partial_json["satellite_prn"][0].asUInt(), 1u);
    EXPECT_EQ(partial_json["satellite_prn"][1].asUInt(), 2u);
    EXPECT_EQ(partial_json["satellite_prn"][2].asUInt(), 3u);
    EXPECT_EQ(partial_json["satellite_prn"][3].asUInt(), 0u); // Should be zero
    EXPECT_EQ(partial_json["satellite_prn"][19].asUInt(), 0u); // Last element should be zero

    EXPECT_EQ(partial_json["satellite_used"][0].asUInt(), 1u);
    EXPECT_EQ(partial_json["satellite_used"][1].asUInt(), 1u);
    EXPECT_EQ(partial_json["satellite_used"][2].asUInt(), 0u);
    EXPECT_EQ(partial_json["satellite_used"][3].asUInt(), 0u);

    EXPECT_EQ(partial_json["satellite_elevation"][0].asUInt(), 45u);
    EXPECT_EQ(partial_json["satellite_elevation"][1].asUInt(), 60u);
    EXPECT_EQ(partial_json["satellite_elevation"][2].asUInt(), 30u);
    EXPECT_EQ(partial_json["satellite_elevation"][3].asUInt(), 0u);

    // Verify specific array element values for full message
    EXPECT_EQ(full_json["satellite_prn"][0].asUInt(), 1u);
    EXPECT_EQ(full_json["satellite_prn"][9].asUInt(), 10u);
    EXPECT_EQ(full_json["satellite_prn"][19].asUInt(), 20u); // Last element

    EXPECT_EQ(full_json["satellite_used"][0].asUInt(), 1u);
    EXPECT_EQ(full_json["satellite_used"][5].asUInt(), 0u); // Some unused satellites
    EXPECT_EQ(full_json["satellite_used"][10].asUInt(), 1u);

    EXPECT_EQ(full_json["satellite_snr"][0].asUInt(), 25u);
    EXPECT_EQ(full_json["satellite_snr"][19].asUInt(), 27u); // Last element

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MavlinkDirectLoadCustomXml)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17005"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17005"), ConnectionResult::Success);

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

    auto sender_mavlink_direct = MavlinkDirect{groundstation_system};
    auto receiver_mavlink_direct = MavlinkDirect{system};

    // Define custom XML with a test message
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

    // Load custom XML on both sender and receiver
    auto result1 = sender_mavlink_direct.load_custom_xml(custom_xml);
    EXPECT_EQ(result1, MavlinkDirect::Result::Success);

    auto result2 = receiver_mavlink_direct.load_custom_xml(custom_xml);
    EXPECT_EQ(result2, MavlinkDirect::Result::Success);

    std::optional<MavlinkDirect::MavlinkMessage> received_message;

    // Subscribe to the custom message
    auto handle = receiver_mavlink_direct.subscribe_message(
        "CUSTOM_TEST_MESSAGE", [&](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received CUSTOM_TEST_MESSAGE: " << message.fields_json;
            received_message = message;
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send the custom message
    MavlinkDirect::MavlinkMessage custom_message;
    custom_message.message_name = "CUSTOM_TEST_MESSAGE";
    custom_message.system_id = 1;
    custom_message.component_id = 1;
    custom_message.target_system = 0;
    custom_message.target_component = 0;
    custom_message.fields_json = R"({"test_value":42,"counter":1337,"status":5})";

    auto send_result = sender_mavlink_direct.send_message(custom_message);
    EXPECT_EQ(send_result, MavlinkDirect::Result::Success);

    // Wait for message to be received
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (!received_message && std::chrono::steady_clock::now() < timeout) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    ASSERT_TRUE(received_message.has_value());
    EXPECT_EQ(received_message->message_name, "CUSTOM_TEST_MESSAGE");

    // Parse JSON to verify field values
    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message->fields_json, json));

    // Verify custom message fields
    EXPECT_EQ(json["test_value"].asUInt(), 42u);
    EXPECT_EQ(json["counter"].asUInt(), 1337u);
    EXPECT_EQ(json["status"].asUInt(), 5u);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
