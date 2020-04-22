#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"
#include "plugins/mission/mission.h"

using namespace mavsdk;

TEST_F(SitlTest, OffboardVelocityNED)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ConnectionResult::SUCCESS, ret);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_TRUE(dc.system().has_autopilot());
    System& system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);
    auto mission = std::make_shared<Mission>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::Success, action_ret);

    action_ret = action->takeoff();
    ASSERT_EQ(Action::Result::Success, action_ret);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Send it once before starting offboard, otherwise it will be rejected.
    Offboard::VelocityNedYaw still{};
    offboard->set_velocity_ned(still);

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::Success);

    {
        Offboard::VelocityNedYaw yaw_right{};
        yaw_right.yaw_deg = 90.0f;
        offboard->set_velocity_ned(yaw_right);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    {
        const float step_size = 0.01f;
        const float one_cycle = 2.0f * M_PI_F;
        const unsigned steps = 2 * unsigned(one_cycle / step_size);

        for (unsigned i = 0; i < steps; ++i) {
            float vx = 5.0f * sinf(i * step_size);
            // std::cout << "vx: " << vx << std::endl;
            Offboard::VelocityNedYaw north_and_yaw_right{};
            north_and_yaw_right.north_m_s = vx;
            north_and_yaw_right.yaw_deg = 90.0f;
            offboard->set_velocity_ned(north_and_yaw_right);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    // Let's make sure that offboard knows it is active.
    EXPECT_TRUE(offboard->is_active().second);

    // Then randomly, we just interfere with a mission command to pause it.
    mission->pause_mission_async(nullptr);
    // This needs some time to propagate.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    // Now it should be inactive.
    EXPECT_FALSE(offboard->is_active().second);

    // So we start it yet again.
    offboard_result = offboard->start();

    // It should complain because no setpoint is set.
    EXPECT_EQ(offboard_result, Offboard::Result::NoSetpointSet);

    // Alright, set one then.
    Offboard::VelocityNedYaw yaw_left{};
    yaw_left.yaw_deg = 270.0f;
    offboard->set_velocity_ned(yaw_left);
    // And start again.
    offboard_result = offboard->start();
    // Now it should work.
    EXPECT_EQ(offboard_result, Offboard::Result::Success);
    EXPECT_TRUE(offboard->is_active().second);

    // Ok let's carry on.
    offboard->set_velocity_ned(yaw_left);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Offboard::VelocityNedYaw up{};
    up.down_m_s = -2.0f;
    up.yaw_deg = 180.0f;
    offboard->set_velocity_ned(up);
    std::this_thread::sleep_for(std::chrono::seconds(4));

    Offboard::VelocityNedYaw yaw_right{};
    yaw_right.yaw_deg = 90.0f;
    offboard->set_velocity_ned(yaw_right);
    std::this_thread::sleep_for(std::chrono::seconds(4));

    Offboard::VelocityNedYaw down{};
    down.down_m_s = 1.0f;
    offboard->set_velocity_ned(down);
    std::this_thread::sleep_for(std::chrono::seconds(4));

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::Success);

    action_ret = action->land();
    EXPECT_EQ(action_ret, Action::Result::Success);
}

TEST_F(SitlTest, OffboardVelocityBody)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ConnectionResult::SUCCESS, ret);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_TRUE(dc.system().has_autopilot());
    System& system = dc.system();

    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::Success, action_ret);

    action_ret = action->takeoff();
    ASSERT_EQ(Action::Result::Success, action_ret);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Send it once before starting offboard, otherwise it will be rejected.
    Offboard::VelocityBodyYawspeed still{};
    offboard->set_velocity_body(still);

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::Success);

    // Yaw clockwise and climb
    Offboard::VelocityBodyYawspeed yaw_clockwise_and_climb{};
    yaw_clockwise_and_climb.down_m_s = -1.0f;
    yaw_clockwise_and_climb.yawspeed_deg_s = 60.0f;
    offboard->set_velocity_body(yaw_clockwise_and_climb);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Yaw anti-clockwise
    Offboard::VelocityBodyYawspeed yaw_anticlockwise{};
    yaw_anticlockwise.yawspeed_deg_s = -60.0f;
    offboard->set_velocity_body(yaw_anticlockwise);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Wait for a bit
    offboard->set_velocity_body(still);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Fly a circle
    Offboard::VelocityBodyYawspeed circle{};
    circle.forward_m_s = 5.0f;
    circle.yawspeed_deg_s = 60.0f;
    offboard->set_velocity_body(circle);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Wait for a bit
    offboard->set_velocity_body(still);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Fly a circle sideways
    Offboard::VelocityBodyYawspeed circle_sideways{};
    circle_sideways.right_m_s = -5.0f;
    circle_sideways.yawspeed_deg_s = 60.0f;
    offboard->set_velocity_body(circle_sideways);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Wait for a bit
    offboard->set_velocity_body(still);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::Success);

    action_ret = action->land();
    EXPECT_EQ(action_ret, Action::Result::Success);
}
