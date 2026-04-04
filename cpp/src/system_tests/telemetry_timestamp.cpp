#include "log.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/mavlink_direct/mavlink_direct.h"
#include <atomic>
#include <cmath>
#include <chrono>
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

// Unit-level: verify struct default initialization and equality operators
TEST(Telemetry, TimestampStructDefaults)
{
    // Default-constructed structs should have timestamp_us == 0
    Telemetry::Altitude alt{};
    EXPECT_EQ(alt.timestamp_us, 0u);
    EXPECT_TRUE(std::isnan(alt.altitude_monotonic_m));

    Telemetry::GroundTruth gt{};
    EXPECT_EQ(gt.timestamp_us, 0u);
    EXPECT_TRUE(std::isnan(gt.latitude_deg));

    // Equality: same timestamps
    Telemetry::Altitude a1{};
    a1.timestamp_us = 100;
    a1.altitude_monotonic_m = 1.0f;
    a1.altitude_amsl_m = 2.0f;
    a1.altitude_local_m = 3.0f;
    a1.altitude_relative_m = 4.0f;
    a1.altitude_terrain_m = 5.0f;
    a1.bottom_clearance_m = 6.0f;

    Telemetry::Altitude a2 = a1;
    EXPECT_TRUE(a1 == a2);

    // Inequality: different timestamps
    a2.timestamp_us = 200;
    EXPECT_FALSE(a1 == a2);

    // GroundTruth equality
    Telemetry::GroundTruth g1{};
    g1.latitude_deg = 47.0;
    g1.longitude_deg = 8.0;
    g1.absolute_altitude_m = 500.0f;
    g1.timestamp_us = 300;

    Telemetry::GroundTruth g2 = g1;
    EXPECT_TRUE(g1 == g2);

    g2.timestamp_us = 400;
    EXPECT_FALSE(g1 == g2);
}

TEST(Telemetry, AltitudeTimestamp)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15200"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15200"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    // Wait for autopilot to discover the ground station connection
    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    auto sender = MavlinkDirect{gs_system};

    const uint64_t expected_timestamp_us = 123456789ULL;
    auto prom = std::promise<Telemetry::Altitude>{};
    auto fut = prom.get_future();
    std::atomic<bool> received{false};

    auto handle = telemetry.subscribe_altitude([&](const Telemetry::Altitude& altitude) {
        if (!received.exchange(true)) {
            prom.set_value(altitude);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send ALTITUDE message via MavlinkDirect
    MavlinkDirect::MavlinkMessage msg;
    msg.message_name = "ALTITUDE";
    msg.system_id = 1;
    msg.component_id = 1;
    msg.target_system_id = 0;
    msg.target_component_id = 0;
    msg.fields_json =
        R"({"time_usec":123456789,"altitude_monotonic":100.0,"altitude_amsl":200.0,)"
        R"("altitude_local":300.0,"altitude_relative":400.0,"altitude_terrain":500.0,)"
        R"("bottom_clearance":600.0})";

    auto result = sender.send_message(msg);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_altitude = fut.get();
    EXPECT_EQ(received_altitude.timestamp_us, expected_timestamp_us);
    EXPECT_FLOAT_EQ(received_altitude.altitude_monotonic_m, 100.0f);
    EXPECT_FLOAT_EQ(received_altitude.altitude_amsl_m, 200.0f);
    EXPECT_FLOAT_EQ(received_altitude.altitude_local_m, 300.0f);
    EXPECT_FLOAT_EQ(received_altitude.altitude_relative_m, 400.0f);
    EXPECT_FLOAT_EQ(received_altitude.altitude_terrain_m, 500.0f);
    EXPECT_FLOAT_EQ(received_altitude.bottom_clearance_m, 600.0f);

    telemetry.unsubscribe_altitude(handle);
}

TEST(Telemetry, GroundTruthTimestamp)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15201"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15201"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    auto sender = MavlinkDirect{gs_system};

    const uint64_t expected_timestamp_us = 987654321ULL;
    auto prom = std::promise<Telemetry::GroundTruth>{};
    auto fut = prom.get_future();
    std::atomic<bool> received{false};

    auto handle = telemetry.subscribe_ground_truth([&](const Telemetry::GroundTruth& ground_truth) {
        if (!received.exchange(true)) {
            prom.set_value(ground_truth);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send HIL_STATE_QUATERNION message via MavlinkDirect
    MavlinkDirect::MavlinkMessage msg;
    msg.message_name = "HIL_STATE_QUATERNION";
    msg.system_id = 1;
    msg.component_id = 1;
    msg.target_system_id = 0;
    msg.target_component_id = 0;
    msg.fields_json = R"({"time_usec":987654321,"attitude_quaternion":[1.0,0.0,0.0,0.0],)"
                      R"("rollspeed":0.0,"pitchspeed":0.0,"yawspeed":0.0,)"
                      R"("lat":473977000,"lon":85456000,"alt":488000,)"
                      R"("vx":0,"vy":0,"vz":0,)"
                      R"("ind_airspeed":0,"true_airspeed":0,"xacc":0,"yacc":0,"zacc":0})";

    auto result = sender.send_message(msg);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto received_ground_truth = fut.get();
    EXPECT_EQ(received_ground_truth.timestamp_us, expected_timestamp_us);
    EXPECT_NEAR(received_ground_truth.latitude_deg, 47.3977, 1e-4);
    EXPECT_NEAR(received_ground_truth.longitude_deg, 8.5456, 1e-4);
    EXPECT_NEAR(received_ground_truth.absolute_altitude_m, 488.0f, 0.1f);

    telemetry.unsubscribe_ground_truth(handle);
}

TEST(Telemetry, AltitudeTimestampUpdates)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15202"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15202"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    auto sender = MavlinkDirect{gs_system};

    // Track all received timestamps
    std::mutex mtx;
    std::vector<uint64_t> received_timestamps;
    auto handle = telemetry.subscribe_altitude([&](const Telemetry::Altitude& altitude) {
        std::lock_guard<std::mutex> lock(mtx);
        received_timestamps.push_back(altitude.timestamp_us);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send 3 altitude messages with increasing timestamps
    const uint64_t timestamps[] = {1000000ULL, 2000000ULL, 3000000ULL};
    for (auto ts : timestamps) {
        MavlinkDirect::MavlinkMessage msg;
        msg.message_name = "ALTITUDE";
        msg.system_id = 1;
        msg.component_id = 1;
        msg.target_system_id = 0;
        msg.target_component_id = 0;
        msg.fields_json = "{\"time_usec\":" + std::to_string(ts) +
                          ",\"altitude_monotonic\":1.0,\"altitude_amsl\":2.0,"
                          "\"altitude_local\":3.0,\"altitude_relative\":4.0,"
                          "\"altitude_terrain\":5.0,\"bottom_clearance\":6.0}";

        auto result = sender.send_message(msg);
        EXPECT_EQ(result, MavlinkDirect::Result::Success);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Wait for all messages to arrive
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    telemetry.unsubscribe_altitude(handle);

    // Verify we received at least 3 messages with the expected timestamps
    std::lock_guard<std::mutex> lock(mtx);
    ASSERT_GE(received_timestamps.size(), 3u);
    // Check that all 3 timestamps appeared in order
    bool found_1 = false, found_2 = false, found_3 = false;
    for (auto ts : received_timestamps) {
        if (ts == 1000000ULL)
            found_1 = true;
        if (ts == 2000000ULL)
            found_2 = true;
        if (ts == 3000000ULL)
            found_3 = true;
    }
    EXPECT_TRUE(found_1) << "Missing timestamp 1000000";
    EXPECT_TRUE(found_2) << "Missing timestamp 2000000";
    EXPECT_TRUE(found_3) << "Missing timestamp 3000000";

    // Verify poll also returns the latest timestamp
    auto polled = telemetry.altitude();
    EXPECT_EQ(polled.timestamp_us, 3000000ULL);
}
