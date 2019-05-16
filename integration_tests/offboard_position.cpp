#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"
#include "plugins/mission/mission.h"

using namespace dronecode_sdk;

TEST_F(SitlTest, OffboardPositionNED)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ConnectionResult::SUCCESS, ret);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_TRUE(dc.system().has_autopilot());

    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);
    auto mission = std::make_shared<Mission>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    action_ret = action->takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Send it once before starting offboard, otherwise it will be rejected.
    offboard->set_position_ned({0.0f, 0.0f, -10.0f, 0.0f});

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    offboard->set_position_ned({0.0f, 0.0f, -10.0f, 90.0f});
    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        const float radius = 10.0f;
        const float step = 0.01f;
        for (float angle = 0.0f; angle <= 2.0f * M_PI_F; angle += step) {
            float x = radius * cosf(angle);
            float y = radius * sinf(angle);
            offboard->set_position_ned({x, y, -10.0f, 90.0f});
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        offboard->set_position_ned({0.0f, 0.0f, -10.0f, 90.0f});
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
    EXPECT_EQ(offboard_result, Offboard::Result::NO_SETPOINT_SET);

    // Alright, set one then.
    offboard->set_position_ned({0.0f, 0.0f, 0.0f, 270.0f});
    // And start again.
    offboard_result = offboard->start();
    // Now it should work.
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);
    EXPECT_TRUE(offboard->is_active());

    // Ok let's carry on.
    offboard->set_position_ned({10.0f, 0.0f, -10.0f, 270.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    offboard->set_position_ned({0.0f, -10.0f, -20.0f, 180.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    offboard->set_position_ned({-10.0f, 0.0f, -30.0f, 90.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    offboard->set_position_ned({0.0f, 10.0f, -20.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    offboard->set_position_ned({0.0f, 00.0f, -10.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    action_ret = action->land();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
}
