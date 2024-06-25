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

TEST(SitlTest, OffboardVelocityNED)
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
        const float one_cycle = 2.0f * static_cast<float>(PI);
        const unsigned steps = 2 * unsigned(one_cycle / step_size);

        for (unsigned i = 0; i < steps; ++i) {
            float vx = 5.0f * sinf(i * step_size);
            // std::cout << "vx: " << vx << '\n';
            Offboard::VelocityNedYaw north_and_yaw_right{};
            north_and_yaw_right.north_m_s = vx;
            north_and_yaw_right.yaw_deg = 90.0f;
            offboard->set_velocity_ned(north_and_yaw_right);
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
    EXPECT_EQ(offboard_result, Offboard::Result::NoSetpointSet);

    // Alright, set one then.
    Offboard::VelocityNedYaw yaw_left{};
    yaw_left.yaw_deg = 270.0f;
    offboard->set_velocity_ned(yaw_left);
    // And start again.
    offboard_result = offboard->start();
    // Now it should work.
    EXPECT_EQ(offboard_result, Offboard::Result::Success);
    EXPECT_TRUE(offboard->is_active());

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

    while (telemetry->armed()) {
        // Wait until we're done.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

TEST(SitlTest, OffboardVelocityBody)
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

    while (telemetry->armed()) {
        // Wait until we're done.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
