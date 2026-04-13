#include "log.hpp"
#include "mavsdk.hpp"
#include "plugins/telemetry/telemetry.hpp"
#include "plugins/mavlink_direct/mavlink_direct.hpp"
#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(SystemTest, HomePositionFullFields)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:15210"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:15210"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();

    while (mavsdk_autopilot.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    auto gs_system = mavsdk_autopilot.systems().at(0);

    auto telemetry = Telemetry{system};
    auto sender = MavlinkDirect{gs_system};

    auto prom = std::promise<Telemetry::HomePosition>{};
    auto fut = prom.get_future();
    std::atomic<bool> received{false};

    auto handle = telemetry.subscribe_home([&](const Telemetry::HomePosition& home) {
        if (!received.exchange(true)) {
            prom.set_value(home);
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Send HOME_POSITION message with all fields populated
    MavlinkDirect::MavlinkMessage msg;
    msg.message_name = "HOME_POSITION";
    msg.system_id = 1;
    msg.component_id = 1;
    msg.target_system_id = 0;
    msg.target_component_id = 0;
    msg.fields_json = R"({"latitude":473977000,"longitude":85456000,"altitude":488000,)"
                      R"("x":1.5,"y":2.5,"z":-3.5,)"
                      R"("q":[0.707,0.0,0.707,0.0],)"
                      R"("approach_x":10.0,"approach_y":20.0,"approach_z":-5.0,)"
                      R"("time_usec":555444333})";

    auto result = sender.send_message(msg);
    EXPECT_EQ(result, MavlinkDirect::Result::Success);

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(5)), std::future_status::ready);

    auto home = fut.get();

    // Verify GPS position (converted from 1e7/1e3 encoding)
    EXPECT_NEAR(home.latitude_deg, 47.3977, 1e-4);
    EXPECT_NEAR(home.longitude_deg, 8.5456, 1e-4);
    EXPECT_NEAR(home.absolute_altitude_m, 488.0f, 0.1f);
    EXPECT_FLOAT_EQ(home.relative_altitude_m, 0.0f);

    // Verify local NED position
    EXPECT_FLOAT_EQ(home.local_north_m, 1.5f);
    EXPECT_FLOAT_EQ(home.local_east_m, 2.5f);
    EXPECT_FLOAT_EQ(home.local_down_m, -3.5f);

    // Verify quaternion
    EXPECT_NEAR(home.q.w, 0.707f, 0.01f);
    EXPECT_NEAR(home.q.x, 0.0f, 0.01f);
    EXPECT_NEAR(home.q.y, 0.707f, 0.01f);
    EXPECT_NEAR(home.q.z, 0.0f, 0.01f);

    // Verify approach vector
    EXPECT_FLOAT_EQ(home.approach_north_m, 10.0f);
    EXPECT_FLOAT_EQ(home.approach_east_m, 20.0f);
    EXPECT_FLOAT_EQ(home.approach_down_m, -5.0f);

    // Verify timestamp
    EXPECT_EQ(home.timestamp_us, 555444333ULL);

    telemetry.unsubscribe_home(handle);
}
