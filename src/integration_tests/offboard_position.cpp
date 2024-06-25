#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "mavsdk_math.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"
#include "plugins/mission/mission.h"

using namespace mavsdk;

TEST(SitlTest, OffboardPositionNED)
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
    auto mission = std::make_shared<Mission>(system);

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

    std::this_thread::sleep_for(std::chrono::seconds(5));

    Offboard::PositionNedYaw up{};
    up.down_m = -10.0f;
    // Send it once before starting offboard, otherwise it will be rejected.
    offboard->set_position_ned(up);

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::Success);

    Offboard::PositionNedYaw up_and_yaw{};
    up_and_yaw.down_m = -10.0f;
    up_and_yaw.yaw_deg = 90.0f;
    offboard->set_position_ned(up_and_yaw);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        const float radius = 10.0f;
        const float step = 0.01f;
        float angle = 0.0f;
        while (angle <= 2.0f * static_cast<float>(PI)) {
            float x = radius * cosf(angle);
            float y = radius * sinf(angle);

            Offboard::PositionNedYaw setpoint{};
            setpoint.east_m = x;
            setpoint.north_m = y;
            setpoint.down_m = -10.0f;
            setpoint.yaw_deg = 90.0f;
            offboard->set_position_ned(setpoint);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            angle += step;
        }
        offboard->set_position_ned(up_and_yaw);
    }

    // Let's make sure that offboard knows it is active.
    EXPECT_TRUE(offboard->is_active());

    // Then randomly, we just interfere with a mission command to pause it.
    mission->pause_mission_async(nullptr);
    // This needs some time to propagate.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Now it should be inactive.
    EXPECT_FALSE(offboard->is_active());

    // So we start it yet again.
    offboard_result = offboard->start();

    // It should complain because no setpoint is set.
    EXPECT_EQ(offboard_result, Offboard::Result::NoSetpointSet);

    // Alright, set one then.
    Offboard::PositionNedYaw yaw_west{};
    yaw_west.north_m = 270.0f;
    offboard->set_position_ned(yaw_west);
    // And start again.
    offboard_result = offboard->start();
    // Now it should work.
    EXPECT_EQ(offboard_result, Offboard::Result::Success);
    EXPECT_TRUE(offboard->is_active());

    // Ok let's carry on.
    Offboard::PositionNedYaw setpoint{};
    setpoint.north_m = 10.0f;
    setpoint.down_m = -10.0f;
    setpoint.yaw_deg = 270.0f;
    offboard->set_position_ned(setpoint);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    setpoint.north_m = 0.0f;
    setpoint.east_m = -10.0f;
    setpoint.down_m = -20.0f;
    setpoint.yaw_deg = 180.0f;
    offboard->set_position_ned(setpoint);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    setpoint.north_m = -10.0f;
    setpoint.east_m = 0.0f;
    setpoint.down_m = -30.0f;
    setpoint.yaw_deg = 90.0f;
    offboard->set_position_ned(setpoint);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    setpoint.north_m = 0.0f;
    setpoint.east_m = 10.0f;
    setpoint.down_m = -20.0f;
    setpoint.yaw_deg = 0.0f;
    offboard->set_position_ned(setpoint);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    setpoint.north_m = 0.0f;
    setpoint.east_m = 0.0f;
    setpoint.down_m = -10.0f;
    setpoint.yaw_deg = 0.0f;
    offboard->set_position_ned(setpoint);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::Success);

    action_ret = action->land();
    EXPECT_EQ(action_ret, Action::Result::Success);
}
