#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"
#include "plugins/mission/mission.h"

using namespace mavsdk;

TEST(SitlTest, PX4OffboardAccelerationNED)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ConnectionResult::Success, ret);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_TRUE(mavsdk.systems().at(0)->has_autopilot());
    auto system = mavsdk.systems().at(0);
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::Success, action_ret);

    action_ret = action->takeoff();
    ASSERT_EQ(Action::Result::Success, action_ret);

    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Send it once before starting offboard, otherwise it will be rejected.
    Offboard::AccelerationNed still{};
    still.north_m_s2 = 0.0f;
    still.east_m_s2 = 0.0f;
    still.down_m_s2 = 0.0f;
    offboard->set_acceleration_ned(still);

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::Success);

    // Go north for a few seconds
    {
        Offboard::AccelerationNed go_north{};
        go_north.north_m_s2 = 1.0f;
        go_north.east_m_s2 = 0.0f;
        go_north.down_m_s2 = 0.0f;
        offboard->set_acceleration_ned(go_north);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // Let's make sure that offboard knows it is active.
    EXPECT_TRUE(offboard->is_active());

    // Stop to stabilize
    {
        offboard->set_acceleration_ned(still);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // Go east for a few seconds
    {
        Offboard::AccelerationNed go_east{};
        go_east.north_m_s2 = 0.0f;
        go_east.east_m_s2 = 1.0f;
        go_east.down_m_s2 = 0.0f;
        offboard->set_acceleration_ned(go_east);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // Stop to stabilize
    {
        offboard->set_acceleration_ned(still);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // Go south for a few seconds
    {
        Offboard::AccelerationNed go_south{};
        go_south.north_m_s2 = -1.0f;
        go_south.east_m_s2 = 0.0f;
        go_south.down_m_s2 = 0.0f;
        offboard->set_acceleration_ned(go_south);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // Stop to stabilize
    {
        offboard->set_acceleration_ned(still);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // Go west for a few seconds
    {
        Offboard::AccelerationNed go_west{};
        go_west.north_m_s2 = 0.0f;
        go_west.east_m_s2 = -1.0f;
        go_west.down_m_s2 = 0.0f;
        offboard->set_acceleration_ned(go_west);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // Stop to stabilize
    {
        offboard->set_acceleration_ned(still);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::Success);

    action_ret = action->land();
    EXPECT_EQ(action_ret, Action::Result::Success);

    while (telemetry->armed()) {
        // Wait until we're done.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
