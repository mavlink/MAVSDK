#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"
#include "plugins/mission/mission.h"

using namespace dronecore;


TEST_F(SitlTest, OffboardVelocityNED)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ConnectionResult::SUCCESS, ret);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);
    auto mission = std::make_shared<Mission>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ActionResult action_ret = action->arm();
    ASSERT_EQ(ActionResult::SUCCESS, action_ret);

    action_ret = action->takeoff();
    ASSERT_EQ(ActionResult::SUCCESS, action_ret);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Send it once before starting offboard, otherwise it will be rejected.
    offboard->set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f});

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    offboard->set_velocity_ned({0.0f, 0.0f, 0.0f, 90.0f});
    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        const float step_size = 0.01f;
        const float one_cycle = 2.0f * M_PI_F;
        const unsigned steps = 2 * unsigned(one_cycle / step_size);

        for (unsigned i = 0; i < steps; ++i) {
            float vx = 5.0f * sinf(i * step_size);
            //std::cout << "vx: " << vx << std::endl;
            offboard->set_velocity_ned({vx, 0.0f, 0.0f, 90.0f});
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
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
    offboard->set_velocity_ned({0.0f, 0.0f, 0.0f, 270.0f});
    // And start again.
    offboard_result = offboard->start();
    // Now it should work.
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);
    EXPECT_TRUE(offboard->is_active());

    // Ok let's carry on.
    offboard->set_velocity_ned({0.0f, 0.0f, 0.0f, 270.0f});
    std::this_thread::sleep_for(std::chrono::seconds(2));

    offboard->set_velocity_ned({0.0f, 0.0f, -2.0f, 180.0f});
    std::this_thread::sleep_for(std::chrono::seconds(4));

    offboard->set_velocity_ned({0.0f, 0.0f, 0.0f, 90.0f});
    std::this_thread::sleep_for(std::chrono::seconds(4));

    offboard->set_velocity_ned({0.0f, 0.0f, 1.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(4));

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    action_ret = action->land();
    EXPECT_EQ(action_ret, ActionResult::SUCCESS);
}


TEST_F(SitlTest, OffboardVelocityBody)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ConnectionResult::SUCCESS, ret);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    System &system = dc.system();

    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    ActionResult action_ret = action->arm();
    ASSERT_EQ(ActionResult::SUCCESS, action_ret);

    action_ret = action->takeoff();
    ASSERT_EQ(ActionResult::SUCCESS, action_ret);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Send it once before starting offboard, otherwise it will be rejected.
    offboard->set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    // Yaw clockwise and climb
    offboard->set_velocity_body({0.0f, 0.0f, -1.0f, 60.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Yaw anti-clockwise
    offboard->set_velocity_body({0.0f, 0.0f, 0.0f, -60.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Wait for a bit
    offboard->set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Fly a circle
    offboard->set_velocity_body({5.0f, 0.0f, 0.0f, 60.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Wait for a bit
    offboard->set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Fly a circle sideways
    offboard->set_velocity_body({0.0f, -5.0f, 0.0f, 60.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Wait for a bit
    offboard->set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    action_ret = action->land();
    EXPECT_EQ(action_ret, ActionResult::SUCCESS);
}
