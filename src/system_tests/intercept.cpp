#include "log.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/telemetry_server/telemetry_server.h"
#include <chrono>
#include <thread>
#include <future>
#include <gtest/gtest.h>
#include <json/json.h>

using namespace mavsdk;

TEST(SystemTest, InterceptIncomingModifyLocal)
{
    // Create 3 MAVSDK instances: Sender -> Interceptor/Forwarder -> Receiver
    Mavsdk mavsdk_sender{Mavsdk::Configuration{ComponentType::Autopilot}};
    Mavsdk mavsdk_interceptor{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_receiver{Mavsdk::Configuration{ComponentType::GroundStation}};

    // Set up connections with forwarding enabled on interceptor
    ASSERT_EQ(
        mavsdk_sender.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_interceptor.add_any_connection(
            "udpin://0.0.0.0:17000", ForwardingOption::ForwardingOn),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_interceptor.add_any_connection(
            "udpout://127.0.0.1:17001", ForwardingOption::ForwardingOn),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_receiver.add_any_connection("udpin://0.0.0.0:17001"), ConnectionResult::Success);

    // Wait for connections to establish
    LogInfo() << "Waiting for connections to establish...";

    // Interceptor discovers sender
    auto sender_system_opt = mavsdk_interceptor.first_autopilot(10.0);
    ASSERT_TRUE(sender_system_opt);
    auto sender_system = sender_system_opt.value();
    ASSERT_TRUE(sender_system->has_autopilot());

    // Receiver discovers sender (via forwarding)
    auto sender_system_receiver_opt = mavsdk_receiver.first_autopilot(10.0);
    ASSERT_TRUE(sender_system_receiver_opt);
    auto sender_system_receiver = sender_system_receiver_opt.value();
    ASSERT_TRUE(sender_system_receiver->has_autopilot());

    // Wait for sender to discover interceptor connection
    LogInfo() << "Waiting for sender to connect to interceptor...";
    while (mavsdk_sender.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto interceptor_system = mavsdk_sender.systems().at(0);

    // Create telemetry instances
    auto telemetry_interceptor = Telemetry{sender_system};
    auto telemetry_receiver = Telemetry{sender_system_receiver};
    auto telemetry_server = TelemetryServer{mavsdk_sender.server_component()};

    // Original position values
    const double original_lat = 47.3977421; // Zurich coordinates
    const double original_lon = 8.5455938;
    const float original_alt = 488.0f;

    // Modified position values (what intercept will change to)
    const double modified_lat = 37.7749295; // San Francisco coordinates
    const double modified_lon = -122.4194155;

    // Set up raw mavlink intercept callback on interceptor to modify incoming GLOBAL_POSITION_INT
    std::atomic<bool> intercept_called{false};

    mavsdk_interceptor.intercept_incoming_messages_async([&](mavlink_message_t& message) -> bool {
        if (message.msgid == MAVLINK_MSG_ID_GLOBAL_POSITION_INT) {
            LogInfo() << "Intercepting GLOBAL_POSITION_INT message from system "
                      << (int)message.sysid;

            // Decode the message
            mavlink_global_position_int_t pos;
            mavlink_msg_global_position_int_decode(&message, &pos);

            LogInfo() << "Original coordinates: lat=" << (pos.lat / 1e7)
                      << ", lon=" << (pos.lon / 1e7);

            // Modify the coordinates to San Francisco
            pos.lat = static_cast<int32_t>(modified_lat * 1e7);
            pos.lon = static_cast<int32_t>(modified_lon * 1e7);

            // Re-encode the modified message
            mavlink_msg_global_position_int_encode(message.sysid, message.compid, &message, &pos);

            LogInfo() << "Modified coordinates to San Francisco: lat=" << (pos.lat / 1e7)
                      << ", lon=" << (pos.lon / 1e7);

            intercept_called = true;
        }
        return true; // Keep the message
    });

    // Promise/future for interceptor telemetry
    auto interceptor_prom = std::promise<Telemetry::Position>();
    auto interceptor_fut = interceptor_prom.get_future();

    // Promise/future for receiver telemetry
    auto receiver_prom = std::promise<Telemetry::Position>();
    auto receiver_fut = receiver_prom.get_future();

    // Subscribe to position updates on interceptor
    auto interceptor_handle =
        telemetry_interceptor.subscribe_position([&interceptor_prom](Telemetry::Position position) {
            LogInfo() << "Interceptor received position: lat=" << position.latitude_deg
                      << ", lon=" << position.longitude_deg;
            interceptor_prom.set_value(position);
        });

    // Subscribe to position updates on receiver
    auto receiver_handle =
        telemetry_receiver.subscribe_position([&receiver_prom](Telemetry::Position position) {
            LogInfo() << "Receiver received position: lat=" << position.latitude_deg
                      << ", lon=" << position.longitude_deg;
            receiver_prom.set_value(position);
        });

    // Send position from sender
    LogInfo() << "Publishing original position from sender...";
    TelemetryServer::Position position{};
    position.latitude_deg = original_lat;
    position.longitude_deg = original_lon;
    position.absolute_altitude_m = original_alt;
    position.relative_altitude_m = original_alt;

    // Empty velocity and heading for this test
    TelemetryServer::VelocityNed velocity{};
    TelemetryServer::Heading heading{};

    ASSERT_EQ(
        telemetry_server.publish_position(position, velocity, heading),
        TelemetryServer::Result::Success);

    // Wait for both telemetry callbacks
    ASSERT_EQ(interceptor_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    ASSERT_EQ(receiver_fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);

    // Give intercept callback time to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Verify intercept was called
    ASSERT_TRUE(intercept_called.load());

    // Get the results
    auto interceptor_position = interceptor_fut.get();
    auto receiver_position = receiver_fut.get();

    // Verify interceptor sees modified coordinates (San Francisco)
    EXPECT_NEAR(interceptor_position.latitude_deg, modified_lat, 1e-6);
    EXPECT_NEAR(interceptor_position.longitude_deg, modified_lon, 1e-6);
    EXPECT_NEAR(interceptor_position.absolute_altitude_m, original_alt, 1.0f);

    // Receiver also gets modified coordinates because intercept happens BEFORE forwarding
    EXPECT_NEAR(
        receiver_position.latitude_deg,
        modified_lat,
        1e-6); // Should be modified San Francisco coordinates
    EXPECT_NEAR(
        receiver_position.longitude_deg,
        modified_lon,
        1e-6); // Should be modified San Francisco coordinates
    EXPECT_NEAR(receiver_position.absolute_altitude_m, original_alt, 1.0f);

    LogInfo() << "Test completed successfully:";
    LogInfo() << "  - Interceptor saw modified coordinates (San Francisco)";
    LogInfo() << "  - Receiver also saw modified coordinates (San Francisco)";
    LogInfo() << "  - This proves intercept affects BOTH local processing AND forwarding";

    // Cleanup
    telemetry_interceptor.unsubscribe_position(interceptor_handle);
    telemetry_receiver.unsubscribe_position(receiver_handle);
    mavsdk_interceptor.intercept_incoming_messages_async(nullptr);
}

TEST(SystemTest, InterceptJsonIncoming)
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

    // Create telemetry instances
    auto telemetry = Telemetry{system};
    auto telemetry_server = TelemetryServer{mavsdk_autopilot.server_component()};

    // Set up incoming JSON message interception on ground station
    auto json_prom = std::promise<Mavsdk::MavlinkMessage>();
    auto json_fut = json_prom.get_future();
    std::atomic<bool> intercept_called{false};

    auto json_handle = mavsdk_groundstation.subscribe_incoming_messages_json(
        [&json_prom, &intercept_called](Mavsdk::MavlinkMessage json_message) {
            LogInfo() << "Intercepted incoming JSON message: " << json_message.message_name
                      << " from system " << json_message.system_id
                      << " with fields: " << json_message.fields_json;

            if (json_message.message_name == "GLOBAL_POSITION_INT") {
                intercept_called = true;
                json_prom.set_value(json_message);
                return true; // Don't drop the message
            }
            return true; // Let other messages through
        });

    // Publish position from autopilot
    LogInfo() << "Publishing position from autopilot...";
    TelemetryServer::Position position{};
    position.latitude_deg = 47.3977421; // Zurich coordinates
    position.longitude_deg = 8.5455938;
    position.absolute_altitude_m = 488.0f;
    position.relative_altitude_m = 488.0f;

    TelemetryServer::VelocityNed velocity{};
    velocity.north_m_s = 1.5f;
    velocity.east_m_s = 2.0f;
    velocity.down_m_s = -0.5f;

    TelemetryServer::Heading heading{};
    heading.heading_deg = 180.0f;

    ASSERT_EQ(
        telemetry_server.publish_position(position, velocity, heading),
        TelemetryServer::Result::Success);

    // Wait for JSON interception to occur
    ASSERT_EQ(json_fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // Verify intercept was called
    ASSERT_TRUE(intercept_called.load());

    auto intercepted_message = json_fut.get();

    // Verify the intercepted JSON message structure
    EXPECT_EQ(intercepted_message.message_name, "GLOBAL_POSITION_INT");
    EXPECT_EQ(intercepted_message.system_id, 1u); // Autopilot system ID
    EXPECT_GT(intercepted_message.fields_json.length(), 0u);

    // Parse JSON to verify field values
    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(intercepted_message.fields_json, json));

    // Verify position fields from the intercepted message
    EXPECT_NEAR(json["lat"].asInt() / 1e7, position.latitude_deg, 1e-6);
    EXPECT_NEAR(json["lon"].asInt() / 1e7, position.longitude_deg, 1e-6);
    EXPECT_NEAR(json["alt"].asInt() / 1e3, position.absolute_altitude_m, 1.0);
    EXPECT_NEAR(json["relative_alt"].asInt() / 1e3, position.relative_altitude_m, 1.0);

    // Verify velocity fields
    EXPECT_NEAR(json["vx"].asInt() / 1e2, velocity.north_m_s, 0.1);
    EXPECT_NEAR(json["vy"].asInt() / 1e2, velocity.east_m_s, 0.1);
    EXPECT_NEAR(json["vz"].asInt() / 1e2, velocity.down_m_s, 0.1);

    // Verify heading field
    EXPECT_NEAR(json["hdg"].asInt() / 1e2, heading.heading_deg, 1.0);

    LogInfo() << "Successfully tested incoming JSON message interception";
    LogInfo() << "  - Message name: " << intercepted_message.message_name;
    LogInfo() << "  - System ID: " << intercepted_message.system_id;
    LogInfo() << "  - Fields JSON length: " << intercepted_message.fields_json.length();

    // Cleanup
    mavsdk_groundstation.unsubscribe_incoming_messages_json(json_handle);
}

TEST(SystemTest, InterceptJsonOutgoing)
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

    // Create telemetry instances
    auto telemetry = Telemetry{system};
    auto telemetry_server = TelemetryServer{mavsdk_autopilot.server_component()};

    // Set up outgoing JSON message interception on autopilot
    auto json_prom = std::promise<Mavsdk::MavlinkMessage>();
    auto json_fut = json_prom.get_future();
    std::atomic<bool> intercept_called{false};

    auto json_handle = mavsdk_autopilot.subscribe_outgoing_messages_json(
        [&json_prom, &intercept_called](Mavsdk::MavlinkMessage json_message) {
            LogInfo() << "Intercepted outgoing JSON message: " << json_message.message_name
                      << " to system " << json_message.target_system_id
                      << " with fields: " << json_message.fields_json;

            if (json_message.message_name == "GPS_RAW_INT") {
                intercept_called = true;
                json_prom.set_value(json_message);
                return true; // Don't drop the message
            }
            return true; // Let other messages through
        });

    // Publish GPS data from autopilot
    LogInfo() << "Publishing GPS data from autopilot...";
    TelemetryServer::RawGps raw_gps{};
    raw_gps.timestamp_us = 1234567890123456789;
    raw_gps.latitude_deg = 47.3977421; // Zurich coordinates
    raw_gps.longitude_deg = 8.5455938;
    raw_gps.absolute_altitude_m = 488.0f;
    raw_gps.hdop = 1.0f;
    raw_gps.vdop = 1.5f;
    raw_gps.velocity_m_s = 15.0f;
    raw_gps.cog_deg = 90.0f;
    raw_gps.altitude_ellipsoid_m = 490.0f;
    raw_gps.horizontal_uncertainty_m = 2.0f;
    raw_gps.vertical_uncertainty_m = 3.0f;
    raw_gps.velocity_uncertainty_m_s = 0.5f;
    raw_gps.heading_uncertainty_deg = 5.0f;
    raw_gps.yaw_deg = 90.0f;

    TelemetryServer::GpsInfo gps_info{};
    gps_info.num_satellites = 12;
    gps_info.fix_type = TelemetryServer::FixType::Fix3D;

    ASSERT_EQ(
        telemetry_server.publish_raw_gps(raw_gps, gps_info), TelemetryServer::Result::Success);

    // Wait for JSON interception to occur
    ASSERT_EQ(json_fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);

    // Verify intercept was called
    ASSERT_TRUE(intercept_called.load());

    auto intercepted_message = json_fut.get();

    // Verify the intercepted JSON message structure
    EXPECT_EQ(intercepted_message.message_name, "GPS_RAW_INT");
    EXPECT_GT(intercepted_message.fields_json.length(), 0u);

    // Parse JSON to verify field values
    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(intercepted_message.fields_json, json));

    // Verify GPS fields from the intercepted message
    EXPECT_EQ(json["time_usec"].asUInt64(), raw_gps.timestamp_us);
    EXPECT_EQ(json["fix_type"].asUInt(), static_cast<uint8_t>(gps_info.fix_type));
    EXPECT_NEAR(json["lat"].asInt() / 1e7, raw_gps.latitude_deg, 1e-6);
    EXPECT_NEAR(json["lon"].asInt() / 1e7, raw_gps.longitude_deg, 1e-6);
    EXPECT_NEAR(json["alt"].asInt() / 1e3, raw_gps.absolute_altitude_m, 1.0);
    EXPECT_NEAR(json["eph"].asInt() / 1e2, raw_gps.hdop, 0.1);
    EXPECT_NEAR(json["epv"].asInt() / 1e2, raw_gps.vdop, 0.1);
    EXPECT_NEAR(json["vel"].asInt() / 1e2, raw_gps.velocity_m_s, 0.1);
    EXPECT_NEAR(json["cog"].asInt() / 1e2, raw_gps.cog_deg, 1.0);
    EXPECT_EQ(json["satellites_visible"].asUInt(), gps_info.num_satellites);

    LogInfo() << "Successfully tested outgoing JSON message interception";
    LogInfo() << "  - Message name: " << intercepted_message.message_name;
    LogInfo() << "  - Target system: " << intercepted_message.target_system_id;
    LogInfo() << "  - Fields JSON length: " << intercepted_message.fields_json.length();

    // Cleanup
    mavsdk_autopilot.unsubscribe_outgoing_messages_json(json_handle);
}
