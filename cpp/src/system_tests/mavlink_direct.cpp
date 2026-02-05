#include "log.h"
#include "mavsdk.h"
#include "plugins/mavlink_direct/mavlink_direct.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/telemetry_server/telemetry_server.h"
#include <chrono>
#include <thread>
#include <future>
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

    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    // Ground station subscribes to receive GLOBAL_POSITION_INT from autopilot
    auto handle = receiver_mavlink_direct.subscribe_message(
        "GLOBAL_POSITION_INT", [&prom](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received GLOBAL_POSITION_INT: " << message.fields_json;
            prom.set_value(message);
        });

    // Wait a bit more for subscription to be properly registered
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Create a complex test message with multiple field types
    MavlinkDirect::MavlinkMessage test_message;
    test_message.message_name = "GLOBAL_POSITION_INT";
    test_message.system_id = 1;
    test_message.component_id = 1;
    test_message.target_system_id = 0;
    test_message.target_component_id = 0;
    test_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    LogInfo() << "Sending GLOBAL_POSITION_INT message...";
    auto result = sender_mavlink_direct.send_message(test_message);
    LogInfo() << "Send result: "
              << (result == MavlinkDirect::Result::Success ? "Success" : "Error");
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    // Wait for message to be received
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    auto received_message = fut.get();

    EXPECT_EQ(received_message.message_name, test_message.message_name);
    EXPECT_EQ(received_message.system_id, test_message.system_id);
    EXPECT_EQ(received_message.component_id, test_message.component_id);

    // Note: For now we only check message_name since field extraction is simplified
    EXPECT_TRUE(received_message.fields_json.find("GLOBAL_POSITION_INT") != std::string::npos);

    receiver_mavlink_direct.unsubscribe_message(handle);
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

    auto compact_prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto compact_fut = compact_prom.get_future();
    auto full_prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto full_fut = full_prom.get_future();

    bool compact_received = false;

    // Subscribe to SYS_STATUS messages
    auto handle = receiver_mavlink_direct.subscribe_message(
        "SYS_STATUS", [&](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received SYS_STATUS: " << message.fields_json;
            if (!compact_received) {
                compact_received = true;
                compact_prom.set_value(message);
            } else {
                full_prom.set_value(message);
            }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Test 1: Send SYS_STATUS in compact form (no extensions)
    MavlinkDirect::MavlinkMessage compact_message;
    compact_message.message_name = "SYS_STATUS";
    compact_message.system_id = 1;
    compact_message.component_id = 1;
    compact_message.target_system_id = 0;
    compact_message.target_component_id = 0;
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
    full_message.target_system_id = 0;
    full_message.target_component_id = 0;
    full_message.fields_json =
        R"({"onboard_control_sensors_present":1,"onboard_control_sensors_enabled":1,"onboard_control_sensors_health":1,"load":500,"voltage_battery":12000,"current_battery":1000,"battery_remaining":75,"drop_rate_comm":0,"errors_comm":0,"errors_count1":0,"errors_count2":0,"errors_count3":0,"errors_count4":0,"onboard_control_sensors_present_extended":123,"onboard_control_sensors_enabled_extended":456,"onboard_control_sensors_health_extended":789})";

    auto result2 = sender_mavlink_direct.send_message(full_message);
    EXPECT_EQ(result2, MavlinkDirect::Result::Success);

    // Wait for both messages to be received
    ASSERT_EQ(compact_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    ASSERT_EQ(full_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_compact = compact_fut.get();
    auto received_full = full_fut.get();

    // Verify both messages were received and parsing worked
    EXPECT_EQ(received_compact.message_name, "SYS_STATUS");
    EXPECT_EQ(received_full.message_name, "SYS_STATUS");

    // Parse JSON to verify field values
    Json::Value compact_json, full_json;
    Json::Reader reader;

    ASSERT_TRUE(reader.parse(received_compact.fields_json, compact_json));
    ASSERT_TRUE(reader.parse(received_full.fields_json, full_json));

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

    receiver_mavlink_direct.unsubscribe_message(handle);
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

    auto prom = std::promise<Telemetry::Position>();
    auto fut = prom.get_future();

    // Subscribe to position updates via Telemetry
    auto handle = receiver_telemetry.subscribe_position([&prom](Telemetry::Position position) {
        LogInfo() << "Received position via Telemetry: lat=" << position.latitude_deg
                  << " lon=" << position.longitude_deg;
        prom.set_value(position);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send GLOBAL_POSITION_INT via MavlinkDirect
    MavlinkDirect::MavlinkMessage test_message;
    test_message.message_name = "GLOBAL_POSITION_INT";
    test_message.system_id = 1;
    test_message.component_id = 1;
    test_message.target_system_id = 0;
    test_message.target_component_id = 0;
    test_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    auto result = sender_mavlink_direct.send_message(test_message);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    // Wait for position to be received
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_position = fut.get();

    // Verify the position data (lat/lon are in degrees * 1e7 in MAVLink)
    EXPECT_NEAR(received_position.latitude_deg, 47.3977418, 0.0001);
    EXPECT_NEAR(received_position.longitude_deg, -122.3974560, 0.0001);

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

    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    // Subscribe to GLOBAL_POSITION_INT via MavlinkDirect
    auto handle = receiver_mavlink_direct.subscribe_message(
        "GLOBAL_POSITION_INT", [&prom](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received GLOBAL_POSITION_INT via MavlinkDirect: " << message.fields_json;
            prom.set_value(message);
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
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_message = fut.get();

    EXPECT_EQ(received_message.message_name, "GLOBAL_POSITION_INT");
    EXPECT_EQ(received_message.system_id, 1);
    EXPECT_EQ(received_message.component_id, 1);

    // Verify the JSON contains the message info (since we're using simplified parsing)
    EXPECT_TRUE(received_message.fields_json.find("GLOBAL_POSITION_INT") != std::string::npos);

    receiver_mavlink_direct.unsubscribe_message(handle);
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

    auto partial_prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto partial_fut = partial_prom.get_future();
    auto full_prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto full_fut = full_prom.get_future();

    bool partial_received = false;

    // Subscribe to GPS_STATUS messages
    auto handle = receiver_mavlink_direct.subscribe_message(
        "GPS_STATUS", [&](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received GPS_STATUS: " << message.fields_json;
            if (!partial_received) {
                partial_received = true;
                partial_prom.set_value(message);
            } else {
                full_prom.set_value(message);
            }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Test 1: Send GPS_STATUS with partial array data (3 satellites)
    MavlinkDirect::MavlinkMessage partial_message;
    partial_message.message_name = "GPS_STATUS";
    partial_message.system_id = 1;
    partial_message.component_id = 1;
    partial_message.target_system_id = 0;
    partial_message.target_component_id = 0;
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
    full_message.target_system_id = 0;
    full_message.target_component_id = 0;
    full_message.fields_json =
        R"({"satellites_visible":20,"satellite_prn":[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20],"satellite_used":[1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0],"satellite_elevation":[45,60,30,75,20,85,50,40,65,35,25,55,70,15,80,10,90,5,45,60],"satellite_azimuth":[0,36,72,108,144,180,216,252,28,64,100,136,172,208,244,20,56,92,128,164],"satellite_snr":[25,30,15,35,20,40,28,22,32,18,26,31,16,38,24,19,33,21,29,27]})";

    auto result2 = sender_mavlink_direct.send_message(full_message);
    EXPECT_EQ(result2, MavlinkDirect::Result::Success);

    // Wait for both messages to be received
    ASSERT_EQ(partial_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);
    ASSERT_EQ(full_fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_partial = partial_fut.get();
    auto received_full = full_fut.get();

    // Verify both messages were received and parsing worked
    EXPECT_EQ(received_partial.message_name, "GPS_STATUS");
    EXPECT_EQ(received_full.message_name, "GPS_STATUS");

    // Parse JSON to verify field values
    Json::Value partial_json, full_json;
    Json::Reader reader;

    ASSERT_TRUE(reader.parse(received_partial.fields_json, partial_json));
    ASSERT_TRUE(reader.parse(received_full.fields_json, full_json));

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

    receiver_mavlink_direct.unsubscribe_message(handle);
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

    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    // Subscribe to the custom message
    auto handle = receiver_mavlink_direct.subscribe_message(
        "CUSTOM_TEST_MESSAGE", [&prom](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received CUSTOM_TEST_MESSAGE: " << message.fields_json;
            prom.set_value(message);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send the custom message
    MavlinkDirect::MavlinkMessage custom_message;
    custom_message.message_name = "CUSTOM_TEST_MESSAGE";
    custom_message.system_id = 1;
    custom_message.component_id = 1;
    custom_message.target_system_id = 0;
    custom_message.target_component_id = 0;
    custom_message.fields_json = R"({"test_value":42,"counter":1337,"status":5})";

    auto send_result = sender_mavlink_direct.send_message(custom_message);
    EXPECT_EQ(send_result, MavlinkDirect::Result::Success);

    // Wait for message to be received
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_message = fut.get();

    EXPECT_EQ(received_message.message_name, "CUSTOM_TEST_MESSAGE");

    // Parse JSON to verify field values
    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message.fields_json, json));

    // Verify custom message fields
    EXPECT_EQ(json["test_value"].asUInt(), 42u);
    EXPECT_EQ(json["counter"].asUInt(), 1337u);
    EXPECT_EQ(json["status"].asUInt(), 5u);

    receiver_mavlink_direct.unsubscribe_message(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MavlinkDirectArdupilotmegaMessage)
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

    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    // Ground station subscribes to receive MEMINFO from autopilot (ArduPilot-specific message)
    auto handle = receiver_mavlink_direct.subscribe_message(
        "MEMINFO", [&prom](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received MEMINFO: " << message.fields_json;
            prom.set_value(message);
        });

    // Autopilot sends MEMINFO message (ID 152 from ardupilotmega.xml)
    MavlinkDirect::MavlinkMessage meminfo_msg;
    meminfo_msg.message_name = "MEMINFO";
    meminfo_msg.fields_json = R"({"brkval": 32768, "freemem": 8192})";

    auto result = sender_mavlink_direct.send_message(meminfo_msg);
    ASSERT_EQ(result, MavlinkDirect::Result::Success);

    // Wait for message to be received
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_message = fut.get();

    // Verify the message was received
    EXPECT_EQ(received_message.message_name, "MEMINFO");

    // Parse JSON to verify field values
    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message.fields_json, json));

    // Verify MEMINFO message fields
    EXPECT_EQ(json["brkval"].asUInt(), 32768u); // Heap top
    EXPECT_EQ(json["freemem"].asUInt(), 8192u); // Free memory

    LogInfo() << "Successfully tested ArduPilot-specific MEMINFO message from ardupilotmega.xml";
    receiver_mavlink_direct.unsubscribe_message(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MavlinkDirectNanInfinityJsonHandling)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17006"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17006"), ConnectionResult::Success);

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

    // Define custom XML with a test message containing float fields
    std::string custom_xml = R"(<?xml version="1.0"?>
<mavlink>
    <version>3</version>
    <dialect>0</dialect>
    <messages>
        <message id="421" name="FLOAT_TEST_MESSAGE">
            <description>A test message for NaN/infinity handling in JSON</description>
            <field type="float" name="normal_float">Normal float field</field>
            <field type="float" name="nan_float">NaN float field</field>
            <field type="float" name="pos_inf_float">Positive infinity float field</field>
            <field type="float" name="neg_inf_float">Negative infinity float field</field>
            <field type="double" name="normal_double">Normal double field</field>
            <field type="double" name="nan_double">NaN double field</field>
            <field type="float[4]" name="float_array">Array with some NaN/inf values</field>
        </message>
    </messages>
</mavlink>)";

    // Load custom XML on both sender and receiver
    auto result1 = sender_mavlink_direct.load_custom_xml(custom_xml);
    EXPECT_EQ(result1, MavlinkDirect::Result::Success);

    auto result2 = receiver_mavlink_direct.load_custom_xml(custom_xml);
    EXPECT_EQ(result2, MavlinkDirect::Result::Success);

    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    // Subscribe to the custom message
    auto handle = receiver_mavlink_direct.subscribe_message(
        "FLOAT_TEST_MESSAGE", [&prom](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received FLOAT_TEST_MESSAGE: " << message.fields_json;
            prom.set_value(message);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Create test message with NaN and infinity values
    // Note: We can't send these directly in JSON, so we'll simulate the scenario
    // where they would be received from a MAVLink message containing such values

    // For testing, we'll send a normal message and verify that the libmav receiver
    // handles NaN/inf correctly. The actual test will be done by checking that
    // valid JSON is produced even when NaN/inf values are present in libmav parsing.

    MavlinkDirect::MavlinkMessage test_message;
    test_message.message_name = "FLOAT_TEST_MESSAGE";
    test_message.system_id = 1;
    test_message.component_id = 1;
    test_message.target_system_id = 0;
    test_message.target_component_id = 0;
    test_message.fields_json =
        R"({"normal_float":3.14,"nan_float":null,"pos_inf_float":null,"neg_inf_float":null,"normal_double":2.718,"nan_double":null,"float_array":[1.0,null,null,4.0]})";

    auto send_result = sender_mavlink_direct.send_message(test_message);
    EXPECT_EQ(send_result, MavlinkDirect::Result::Success);

    // Wait for message to be received
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_message = fut.get();

    EXPECT_EQ(received_message.message_name, "FLOAT_TEST_MESSAGE");

    // Parse JSON to verify it's valid JSON (most important test)
    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message.fields_json, json))
        << "JSON parsing failed, indicating invalid JSON was generated. "
        << "JSON content: " << received_message.fields_json;

    // Verify all expected fields are present
    EXPECT_TRUE(json.isMember("normal_float"));
    EXPECT_TRUE(json.isMember("nan_float"));
    EXPECT_TRUE(json.isMember("pos_inf_float"));
    EXPECT_TRUE(json.isMember("neg_inf_float"));
    EXPECT_TRUE(json.isMember("normal_double"));
    EXPECT_TRUE(json.isMember("nan_double"));
    EXPECT_TRUE(json.isMember("float_array"));

    // Verify that normal values are preserved correctly
    EXPECT_TRUE(json["normal_float"].isNumeric());
    EXPECT_NEAR(json["normal_float"].asFloat(), 3.14f, 0.001f);
    EXPECT_TRUE(json["normal_double"].isNumeric());
    EXPECT_NEAR(json["normal_double"].asDouble(), 2.718, 0.001);

    // The key test: verify that null values in input JSON were converted to NaN
    // in the MAVLink message, then back to null in the output JSON
    EXPECT_TRUE(json["nan_float"].isNull())
        << "nan_float should be null, got: " << json["nan_float"];
    EXPECT_TRUE(json["pos_inf_float"].isNull())
        << "pos_inf_float should be null, got: " << json["pos_inf_float"];
    EXPECT_TRUE(json["neg_inf_float"].isNull())
        << "neg_inf_float should be null, got: " << json["neg_inf_float"];
    EXPECT_TRUE(json["nan_double"].isNull())
        << "nan_double should be null, got: " << json["nan_double"];

    // Verify array handling: normal values preserved, null values round-trip as null (via NaN)
    EXPECT_TRUE(json["float_array"].isArray());
    EXPECT_EQ(json["float_array"].size(), 4u);
    EXPECT_TRUE(json["float_array"][0].isNumeric());
    EXPECT_NEAR(json["float_array"][0].asFloat(), 1.0f, 0.001f);
    EXPECT_TRUE(json["float_array"][1].isNull())
        << "float_array[1] should be null (converted from NaN), got: " << json["float_array"][1];
    EXPECT_TRUE(json["float_array"][2].isNull())
        << "float_array[2] should be null (converted from NaN), got: " << json["float_array"][2];
    EXPECT_TRUE(json["float_array"][3].isNumeric());
    EXPECT_NEAR(json["float_array"][3].asFloat(), 4.0f, 0.001f);

    LogInfo() << "Successfully verified that float/double JSON handling produces valid JSON";
    receiver_mavlink_direct.unsubscribe_message(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MavlinkDirectMessageFiltering)
{
    // Test that message filtering works correctly: when subscribed to one message type,
    // only that message type is received, not others

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17008"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17008"), ConnectionResult::Success);

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

    auto groundstation_system = mavsdk_autopilot.systems().at(0);
    ASSERT_EQ(groundstation_system->component_ids()[0], 190);

    // Create separate MavlinkDirect instances
    auto receiver_mavlink_direct = MavlinkDirect{system};
    auto sender_mavlink_direct = MavlinkDirect{groundstation_system};

    // Set up counters to track what messages are received
    std::atomic<int> heartbeat_count{0};
    std::atomic<int> global_position_count{0};
    std::atomic<int> sys_status_count{0};
    std::atomic<int> other_message_count{0};

    // Subscribe ONLY to HEARTBEAT messages
    auto handle = receiver_mavlink_direct.subscribe_message(
        "HEARTBEAT", [&](MavlinkDirect::MavlinkMessage message) {
            if (message.message_name == "HEARTBEAT") {
                heartbeat_count++;
                LogInfo() << "Received expected HEARTBEAT message";
            } else if (message.message_name == "GLOBAL_POSITION_INT") {
                global_position_count++;
                LogErr() << "BUG: Received GLOBAL_POSITION_INT when subscribed to HEARTBEAT!";
            } else if (message.message_name == "SYS_STATUS") {
                sys_status_count++;
                LogErr() << "BUG: Received SYS_STATUS when subscribed to HEARTBEAT!";
            } else {
                other_message_count++;
                LogErr() << "BUG: Received unexpected message: " << message.message_name;
            }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send different message types - the subscription should only receive HEARTBEAT

    // 1. Send HEARTBEAT (should be received)
    MavlinkDirect::MavlinkMessage heartbeat_message;
    heartbeat_message.message_name = "HEARTBEAT";
    heartbeat_message.system_id = 1;
    heartbeat_message.component_id = 1;
    heartbeat_message.fields_json =
        R"({"type":2,"autopilot":3,"base_mode":81,"custom_mode":0,"system_status":4,"mavlink_version":3})";

    LogInfo() << "Sending HEARTBEAT message (should be received)";
    EXPECT_EQ(
        sender_mavlink_direct.send_message(heartbeat_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 2. Send GLOBAL_POSITION_INT (should NOT be received)
    MavlinkDirect::MavlinkMessage gps_message;
    gps_message.message_name = "GLOBAL_POSITION_INT";
    gps_message.system_id = 1;
    gps_message.component_id = 1;
    gps_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    LogInfo() << "Sending GLOBAL_POSITION_INT message (should NOT be received)";
    EXPECT_EQ(sender_mavlink_direct.send_message(gps_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 3. Send SYS_STATUS (should NOT be received)
    MavlinkDirect::MavlinkMessage sys_status_message;
    sys_status_message.message_name = "SYS_STATUS";
    sys_status_message.system_id = 1;
    sys_status_message.component_id = 1;
    sys_status_message.fields_json =
        R"({"onboard_control_sensors_present":1,"onboard_control_sensors_enabled":1,"onboard_control_sensors_health":1,"load":500,"voltage_battery":12000,"current_battery":1000,"battery_remaining":75,"drop_rate_comm":0,"errors_comm":0,"errors_count1":0,"errors_count2":0,"errors_count3":0,"errors_count4":0})";

    LogInfo() << "Sending SYS_STATUS message (should NOT be received)";
    EXPECT_EQ(
        sender_mavlink_direct.send_message(sys_status_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 4. Send another HEARTBEAT (should be received)
    LogInfo() << "Sending second HEARTBEAT message (should be received)";
    EXPECT_EQ(
        sender_mavlink_direct.send_message(heartbeat_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify filtering worked correctly
    EXPECT_GE(heartbeat_count.load(), 2) << "Should have received at least 2 HEARTBEAT messages";
    EXPECT_EQ(global_position_count.load(), 0)
        << "Should NOT have received any GLOBAL_POSITION_INT messages";
    EXPECT_EQ(sys_status_count.load(), 0) << "Should NOT have received any SYS_STATUS messages";
    EXPECT_EQ(other_message_count.load(), 0) << "Should NOT have received any other message types";

    LogInfo() << "Message filtering test results:";
    LogInfo() << "  HEARTBEAT received: " << heartbeat_count.load() << " (expected: >= 2)";
    LogInfo() << "  GLOBAL_POSITION_INT received: " << global_position_count.load()
              << " (expected: 0)";
    LogInfo() << "  SYS_STATUS received: " << sys_status_count.load() << " (expected: 0)";
    LogInfo() << "  Other messages received: " << other_message_count.load() << " (expected: 0)";

    receiver_mavlink_direct.unsubscribe_message(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MavlinkDirectEmptyStringFiltering)
{
    // Test that subscribing with empty string ("") receives all message types

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17009"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17009"), ConnectionResult::Success);

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

    auto groundstation_system = mavsdk_autopilot.systems().at(0);
    ASSERT_EQ(groundstation_system->component_ids()[0], 190);

    // Create separate MavlinkDirect instances
    auto receiver_mavlink_direct = MavlinkDirect{system};
    auto sender_mavlink_direct = MavlinkDirect{groundstation_system};

    // Set up counters to track what messages are received
    std::atomic<int> heartbeat_count{0};
    std::atomic<int> global_position_count{0};
    std::atomic<int> sys_status_count{0};
    std::atomic<int> total_messages{0};

    // Subscribe to ALL messages with empty string
    auto handle =
        receiver_mavlink_direct.subscribe_message("", [&](MavlinkDirect::MavlinkMessage message) {
            total_messages++;
            if (message.message_name == "HEARTBEAT") {
                heartbeat_count++;
                LogInfo() << "Received HEARTBEAT via empty string subscription";
            } else if (message.message_name == "GLOBAL_POSITION_INT") {
                global_position_count++;
                LogInfo() << "Received GLOBAL_POSITION_INT via empty string subscription";
            } else if (message.message_name == "SYS_STATUS") {
                sys_status_count++;
                LogInfo() << "Received SYS_STATUS via empty string subscription";
            } else {
                LogInfo() << "Received other message via empty string subscription: "
                          << message.message_name;
            }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send different message types - all should be received with empty string subscription

    // 1. Send HEARTBEAT
    MavlinkDirect::MavlinkMessage heartbeat_message;
    heartbeat_message.message_name = "HEARTBEAT";
    heartbeat_message.system_id = 1;
    heartbeat_message.component_id = 1;
    heartbeat_message.fields_json =
        R"({"type":2,"autopilot":3,"base_mode":81,"custom_mode":0,"system_status":4,"mavlink_version":3})";

    LogInfo() << "Sending HEARTBEAT message";
    EXPECT_EQ(
        sender_mavlink_direct.send_message(heartbeat_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 2. Send GLOBAL_POSITION_INT
    MavlinkDirect::MavlinkMessage gps_message;
    gps_message.message_name = "GLOBAL_POSITION_INT";
    gps_message.system_id = 1;
    gps_message.component_id = 1;
    gps_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    LogInfo() << "Sending GLOBAL_POSITION_INT message";
    EXPECT_EQ(sender_mavlink_direct.send_message(gps_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 3. Send SYS_STATUS
    MavlinkDirect::MavlinkMessage sys_status_message;
    sys_status_message.message_name = "SYS_STATUS";
    sys_status_message.system_id = 1;
    sys_status_message.component_id = 1;
    sys_status_message.fields_json =
        R"({"onboard_control_sensors_present":1,"onboard_control_sensors_enabled":1,"onboard_control_sensors_health":1,"load":500,"voltage_battery":12000,"current_battery":1000,"battery_remaining":75,"drop_rate_comm":0,"errors_comm":0,"errors_count1":0,"errors_count2":0,"errors_count3":0,"errors_count4":0})";

    LogInfo() << "Sending SYS_STATUS message";
    EXPECT_EQ(
        sender_mavlink_direct.send_message(sys_status_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify empty string subscription received ALL message types
    EXPECT_GE(heartbeat_count.load(), 1) << "Should have received at least 1 HEARTBEAT message";
    EXPECT_GE(global_position_count.load(), 1)
        << "Should have received at least 1 GLOBAL_POSITION_INT message";
    EXPECT_GE(sys_status_count.load(), 1) << "Should have received at least 1 SYS_STATUS message";
    EXPECT_GE(total_messages.load(), 3) << "Should have received at least 3 total messages";

    LogInfo() << "Empty string filtering test results:";
    LogInfo() << "  HEARTBEAT received: " << heartbeat_count.load() << " (expected: >= 1)";
    LogInfo() << "  GLOBAL_POSITION_INT received: " << global_position_count.load()
              << " (expected: >= 1)";
    LogInfo() << "  SYS_STATUS received: " << sys_status_count.load() << " (expected: >= 1)";
    LogInfo() << "  Total messages received: " << total_messages.load() << " (expected: >= 3)";

    receiver_mavlink_direct.unsubscribe_message(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MavlinkDirectMultipleSubscriptions)
{
    // Test that having multiple subscriptions works correctly:
    // - One subscription for all messages ("")
    // - One subscription for specific message ("HEARTBEAT")
    // This should expose any issues with shared CallbackList behavior

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17010"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17010"), ConnectionResult::Success);

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

    auto groundstation_system = mavsdk_autopilot.systems().at(0);
    ASSERT_EQ(groundstation_system->component_ids()[0], 190);

    // Create separate MavlinkDirect instances
    auto receiver_mavlink_direct = MavlinkDirect{system};
    auto sender_mavlink_direct = MavlinkDirect{groundstation_system};

    // Set up counters for the "all messages" subscription
    std::atomic<int> all_messages_heartbeat{0};
    std::atomic<int> all_messages_gps{0};
    std::atomic<int> all_messages_sys_status{0};
    std::atomic<int> all_messages_total{0};

    // Set up counters for the "HEARTBEAT only" subscription
    std::atomic<int> heartbeat_only_heartbeat{0};
    std::atomic<int> heartbeat_only_other{0};

    // Subscription 1: All messages (empty string)
    auto handle_all =
        receiver_mavlink_direct.subscribe_message("", [&](MavlinkDirect::MavlinkMessage message) {
            all_messages_total++;
            if (message.message_name == "HEARTBEAT") {
                all_messages_heartbeat++;
                LogInfo() << "ALL subscription received HEARTBEAT";
            } else if (message.message_name == "GLOBAL_POSITION_INT") {
                all_messages_gps++;
                LogInfo() << "ALL subscription received GLOBAL_POSITION_INT";
            } else if (message.message_name == "SYS_STATUS") {
                all_messages_sys_status++;
                LogInfo() << "ALL subscription received SYS_STATUS";
            } else {
                LogInfo() << "ALL subscription received other: " << message.message_name;
            }
        });

    // Subscription 2: HEARTBEAT only
    auto handle_heartbeat = receiver_mavlink_direct.subscribe_message(
        "HEARTBEAT", [&](MavlinkDirect::MavlinkMessage message) {
            if (message.message_name == "HEARTBEAT") {
                heartbeat_only_heartbeat++;
                LogInfo() << "HEARTBEAT-only subscription received HEARTBEAT";
            } else {
                heartbeat_only_other++;
                LogErr() << "BUG: HEARTBEAT-only subscription received: " << message.message_name;
            }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send different message types

    // 1. Send HEARTBEAT (both subscriptions should receive it)
    MavlinkDirect::MavlinkMessage heartbeat_message;
    heartbeat_message.message_name = "HEARTBEAT";
    heartbeat_message.system_id = 1;
    heartbeat_message.component_id = 1;
    heartbeat_message.fields_json =
        R"({"type":2,"autopilot":3,"base_mode":81,"custom_mode":0,"system_status":4,"mavlink_version":3})";

    LogInfo() << "Sending HEARTBEAT message";
    EXPECT_EQ(
        sender_mavlink_direct.send_message(heartbeat_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 2. Send GLOBAL_POSITION_INT (only ALL subscription should receive it)
    MavlinkDirect::MavlinkMessage gps_message;
    gps_message.message_name = "GLOBAL_POSITION_INT";
    gps_message.system_id = 1;
    gps_message.component_id = 1;
    gps_message.fields_json =
        R"({"time_boot_ms":12345,"lat":473977418,"lon":-1223974560,"alt":100500,"relative_alt":50250,"vx":100,"vy":-50,"vz":25,"hdg":18000})";

    LogInfo() << "Sending GLOBAL_POSITION_INT message";
    EXPECT_EQ(sender_mavlink_direct.send_message(gps_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 3. Send SYS_STATUS (only ALL subscription should receive it)
    MavlinkDirect::MavlinkMessage sys_status_message;
    sys_status_message.message_name = "SYS_STATUS";
    sys_status_message.system_id = 1;
    sys_status_message.component_id = 1;
    sys_status_message.fields_json =
        R"({"onboard_control_sensors_present":1,"onboard_control_sensors_enabled":1,"onboard_control_sensors_health":1,"load":500,"voltage_battery":12000,"current_battery":1000,"battery_remaining":75,"drop_rate_comm":0,"errors_comm":0,"errors_count1":0,"errors_count4":0})";

    LogInfo() << "Sending SYS_STATUS message";
    EXPECT_EQ(
        sender_mavlink_direct.send_message(sys_status_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // 4. Send another HEARTBEAT (both subscriptions should receive it)
    LogInfo() << "Sending second HEARTBEAT message";
    EXPECT_EQ(
        sender_mavlink_direct.send_message(heartbeat_message), MavlinkDirect::Result::Success);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Verify correct filtering behavior
    LogInfo() << "Multiple subscriptions test results:";
    LogInfo() << "  ALL subscription - HEARTBEAT: " << all_messages_heartbeat.load()
              << " (expected: >= 2)";
    LogInfo() << "  ALL subscription - GPS: " << all_messages_gps.load() << " (expected: >= 1)";
    LogInfo() << "  ALL subscription - SYS_STATUS: " << all_messages_sys_status.load()
              << " (expected: >= 1)";
    LogInfo() << "  ALL subscription - Total: " << all_messages_total.load() << " (expected: >= 4)";
    LogInfo() << "  HEARTBEAT-only - HEARTBEAT: " << heartbeat_only_heartbeat.load()
              << " (expected: >= 2)";
    LogInfo() << "  HEARTBEAT-only - Other: " << heartbeat_only_other.load() << " (expected: 0)";

    // ALL subscription should receive everything
    EXPECT_GE(all_messages_heartbeat.load(), 2)
        << "ALL subscription should receive HEARTBEAT messages";
    EXPECT_GE(all_messages_gps.load(), 1) << "ALL subscription should receive GLOBAL_POSITION_INT";
    EXPECT_GE(all_messages_sys_status.load(), 1) << "ALL subscription should receive SYS_STATUS";
    EXPECT_GE(all_messages_total.load(), 4) << "ALL subscription should receive all sent messages";

    // HEARTBEAT-only subscription should only receive HEARTBEAT
    EXPECT_GE(heartbeat_only_heartbeat.load(), 2)
        << "HEARTBEAT-only should receive HEARTBEAT messages";
    EXPECT_EQ(heartbeat_only_other.load(), 0)
        << "HEARTBEAT-only should NOT receive any other messages";

    receiver_mavlink_direct.unsubscribe_message(handle_all);
    receiver_mavlink_direct.unsubscribe_message(handle_heartbeat);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MavlinkDirectLargeUint64)
{
    // Test GPS_RAW_INT with time_usec field > 2^32 to verify proper uint64 handling

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17011"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17011"), ConnectionResult::Success);

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

    auto groundstation_system = mavsdk_autopilot.systems().at(0);
    ASSERT_EQ(groundstation_system->component_ids()[0], 190);

    auto receiver_mavlink_direct = MavlinkDirect{system};
    auto sender_mavlink_direct = MavlinkDirect{groundstation_system};

    auto prom = std::promise<MavlinkDirect::MavlinkMessage>();
    auto fut = prom.get_future();

    // Subscribe to GPS_RAW_INT messages
    auto handle = receiver_mavlink_direct.subscribe_message(
        "GPS_RAW_INT", [&prom](MavlinkDirect::MavlinkMessage message) {
            LogInfo() << "Received GPS_RAW_INT: " << message.fields_json;
            prom.set_value(message);
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send GPS_RAW_INT with time_usec > 2^32 (5000000000 microseconds = ~83 minutes)
    // This value requires full uint64 representation
    MavlinkDirect::MavlinkMessage gps_raw_message;
    gps_raw_message.message_name = "GPS_RAW_INT";
    gps_raw_message.system_id = 1;
    gps_raw_message.component_id = 1;
    gps_raw_message.target_system_id = 0;
    gps_raw_message.target_component_id = 0;
    gps_raw_message.fields_json =
        R"({"time_usec":5000000000,"fix_type":3,"lat":473977418,"lon":-1223974560,"alt":100500,"eph":100,"epv":150,"vel":500,"cog":18000,"satellites_visible":12})";

    LogInfo() << "Sending GPS_RAW_INT with time_usec=5000000000 (> 2^32)";
    auto result = sender_mavlink_direct.send_message(gps_raw_message);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    // Wait for message to be received
    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_message = fut.get();

    EXPECT_EQ(received_message.message_name, "GPS_RAW_INT");
    EXPECT_EQ(received_message.system_id, 1);
    EXPECT_EQ(received_message.component_id, 1);

    // Parse JSON to verify uint64 field value is preserved
    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(received_message.fields_json, json))
        << "Failed to parse received JSON: " << received_message.fields_json;

    // Verify time_usec field is present and has the correct large value
    ASSERT_TRUE(json.isMember("time_usec")) << "time_usec field missing from JSON";
    EXPECT_EQ(json["time_usec"].asUInt64(), 5000000000ULL)
        << "time_usec value incorrect: expected 5000000000, got " << json["time_usec"].asUInt64();

    // Verify other fields for completeness
    EXPECT_EQ(json["fix_type"].asUInt(), 3u);
    EXPECT_EQ(json["lat"].asInt(), 473977418);
    EXPECT_EQ(json["lon"].asInt(), -1223974560);
    EXPECT_EQ(json["alt"].asInt(), 100500);
    EXPECT_EQ(json["satellites_visible"].asUInt(), 12u);

    LogInfo() << "Successfully verified uint64 handling for time_usec > 2^32";
    receiver_mavlink_direct.unsubscribe_message(handle);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
