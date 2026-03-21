#include "log.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/mavlink_direct/mavlink_direct.h"
#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, AltitudeTimestamp)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15200"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15200"),
        ConnectionResult::Success);

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

TEST(SystemTest, GroundTruthTimestamp)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15201"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15201"),
        ConnectionResult::Success);

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

    auto handle =
        telemetry.subscribe_ground_truth([&](const Telemetry::GroundTruth& ground_truth) {
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
    msg.fields_json =
        R"({"time_usec":987654321,"attitude_quaternion":[1.0,0.0,0.0,0.0],)"
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
