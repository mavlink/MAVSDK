#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/telemetry/telemetry.hpp"
#include "plugins/telemetry_server/telemetry_server.hpp"
#include <chrono>
#include <thread>
#include <future>
#include <gtest/gtest.h>
#include <json/json.h>

using namespace mavsdk;

TEST(Intercept, IncomingDrop)
{
    // Create 3 MAVSDK instances: Sender -> Interceptor/Forwarder -> Receiver
    Mavsdk mavsdk_sender{Mavsdk::Configuration{ComponentType::Autopilot}};
    Mavsdk mavsdk_interceptor{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_receiver{Mavsdk::Configuration{ComponentType::GroundStation}};

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

    LogInfo("Waiting for connections to establish...");

    auto sender_system_opt = mavsdk_interceptor.first_autopilot(10.0);
    ASSERT_TRUE(sender_system_opt);
    auto sender_system = sender_system_opt.value();

    while (mavsdk_sender.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto telemetry_interceptor = Telemetry{sender_system};
    auto telemetry_server = TelemetryServer{mavsdk_sender.server_component()};

    // Drop all GLOBAL_POSITION_INT messages at the interceptor using the JSON API
    std::atomic<bool> intercept_called{false};
    auto drop_handle = mavsdk_interceptor.subscribe_incoming_messages_json(
        [&intercept_called](Mavsdk::MavlinkMessage message) {
            if (message.message_name == "GLOBAL_POSITION_INT") {
                intercept_called = true;
                return false; // drop
            }
            return true;
        });

    // Interceptor should NOT receive a position update
    std::atomic<bool> interceptor_received{false};
    auto interceptor_handle = telemetry_interceptor.subscribe_position(
        [&interceptor_received](Telemetry::Position) { interceptor_received = true; });

    TelemetryServer::Position position{};
    position.latitude_deg = 47.3977421;
    position.longitude_deg = 8.5455938;
    position.absolute_altitude_m = 488.0f;
    position.relative_altitude_m = 488.0f;
    TelemetryServer::VelocityNed velocity{};
    TelemetryServer::Heading heading{};

    ASSERT_EQ(
        telemetry_server.publish_position(position, velocity, heading),
        TelemetryServer::Result::Success);

    // Give time for messages to propagate (or not)
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    EXPECT_TRUE(intercept_called.load());
    EXPECT_FALSE(interceptor_received.load());

    LogInfo("Test completed: GLOBAL_POSITION_INT was dropped by interceptor");
    LogInfo("  - Interceptor JSON callback was called: {}", intercept_called.load());
    LogInfo("  - Interceptor telemetry NOT received: {}", !interceptor_received.load());

    telemetry_interceptor.unsubscribe_position(interceptor_handle);
    mavsdk_interceptor.unsubscribe_incoming_messages_json(drop_handle);
}

TEST(Intercept, JsonIncoming)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    LogInfo("Waiting for autopilot system to connect...");
    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto telemetry = Telemetry{system};
    auto telemetry_server = TelemetryServer{mavsdk_autopilot.server_component()};

    auto json_prom = std::promise<Mavsdk::MavlinkMessage>();
    auto json_fut = json_prom.get_future();
    std::atomic<bool> intercept_called{false};

    auto json_handle = mavsdk_groundstation.subscribe_incoming_messages_json(
        [&json_prom, &intercept_called](Mavsdk::MavlinkMessage json_message) {
            LogInfo(
                "Intercepted incoming JSON message: {} from system {} with fields: {}",
                json_message.message_name,
                json_message.system_id,
                json_message.fields_json);

            if (json_message.message_name == "GLOBAL_POSITION_INT") {
                intercept_called = true;
                json_prom.set_value(json_message);
                return true;
            }
            return true;
        });

    TelemetryServer::Position position{};
    position.latitude_deg = 47.3977421;
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

    ASSERT_EQ(json_fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    ASSERT_TRUE(intercept_called.load());

    auto intercepted_message = json_fut.get();
    EXPECT_EQ(intercepted_message.message_name, "GLOBAL_POSITION_INT");
    EXPECT_EQ(intercepted_message.system_id, 1u);
    EXPECT_GT(intercepted_message.fields_json.length(), 0u);

    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(intercepted_message.fields_json, json));

    EXPECT_NEAR(json["lat"].asInt() / 1e7, position.latitude_deg, 1e-6);
    EXPECT_NEAR(json["lon"].asInt() / 1e7, position.longitude_deg, 1e-6);
    EXPECT_NEAR(json["alt"].asInt() / 1e3, position.absolute_altitude_m, 1.0);
    EXPECT_NEAR(json["relative_alt"].asInt() / 1e3, position.relative_altitude_m, 1.0);
    EXPECT_NEAR(json["vx"].asInt() / 1e2, velocity.north_m_s, 0.1);
    EXPECT_NEAR(json["vy"].asInt() / 1e2, velocity.east_m_s, 0.1);
    EXPECT_NEAR(json["vz"].asInt() / 1e2, velocity.down_m_s, 0.1);
    EXPECT_NEAR(json["hdg"].asInt() / 1e2, heading.heading_deg, 1.0);

    LogInfo("Successfully tested incoming JSON message interception");
    mavsdk_groundstation.unsubscribe_incoming_messages_json(json_handle);
}

TEST(Intercept, JsonOutgoing)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    LogInfo("Waiting for autopilot system to connect...");
    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auto telemetry = Telemetry{system};
    auto telemetry_server = TelemetryServer{mavsdk_autopilot.server_component()};

    auto json_prom = std::promise<Mavsdk::MavlinkMessage>();
    auto json_fut = json_prom.get_future();
    std::atomic<bool> intercept_called{false};

    auto json_handle = mavsdk_autopilot.subscribe_outgoing_messages_json(
        [&json_prom, &intercept_called](Mavsdk::MavlinkMessage json_message) {
            LogInfo(
                "Intercepted outgoing JSON message: {} to system {} with fields: {}",
                json_message.message_name,
                json_message.target_system_id,
                json_message.fields_json);

            if (json_message.message_name == "GPS_RAW_INT") {
                intercept_called = true;
                json_prom.set_value(json_message);
                return true;
            }
            return true;
        });

    TelemetryServer::RawGps raw_gps{};
    raw_gps.timestamp_us = 1234567890123456789;
    raw_gps.latitude_deg = 47.3977421;
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

    ASSERT_EQ(json_fut.wait_for(std::chrono::seconds(1)), std::future_status::ready);
    ASSERT_TRUE(intercept_called.load());

    auto intercepted_message = json_fut.get();
    EXPECT_EQ(intercepted_message.message_name, "GPS_RAW_INT");
    EXPECT_GT(intercepted_message.fields_json.length(), 0u);

    Json::Value json;
    Json::Reader reader;
    ASSERT_TRUE(reader.parse(intercepted_message.fields_json, json));

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

    LogInfo("Successfully tested outgoing JSON message interception");
    mavsdk_autopilot.unsubscribe_outgoing_messages_json(json_handle);
}
