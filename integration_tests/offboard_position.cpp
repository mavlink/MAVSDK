#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"

using namespace dronecore;

TEST_F(SitlTest, OffboardSetAltitude)
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
    float altitude_m = -20.0f; // [m]
    const float dev = 0.50f; // [m]
    Offboard::PositionLocalNED position = {NAN, NAN, altitude_m};
    offboard->set_position_local_ned(position);

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    offboard->set_position_local_ned(position);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    EXPECT_GT(telemetry->position().relative_altitude_m, -altitude_m - dev);
    EXPECT_LT(telemetry->position().relative_altitude_m, -altitude_m + dev);

    altitude_m = -3.0f;
    position = {NAN, NAN, altitude_m};
    offboard->set_position_local_ned(position);
    std::this_thread::sleep_for(std::chrono::seconds(20));
    EXPECT_GT(telemetry->position().relative_altitude_m, -altitude_m - dev);
    EXPECT_LT(telemetry->position().relative_altitude_m, -altitude_m + dev);

    altitude_m = -5.0f;
    position = {NAN, NAN, altitude_m};
    offboard->set_position_local_ned(position);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    EXPECT_GT(telemetry->position().relative_altitude_m, -altitude_m - dev);
    EXPECT_LT(telemetry->position().relative_altitude_m, -altitude_m + dev);

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    action_ret = action->land();
    EXPECT_EQ(action_ret, ActionResult::SUCCESS);
}

TEST_F(SitlTest, OffboardSetPosition)
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
    float x = 0.0f; // [m]
    float y = 0.0f; // [m]
    float z = -20.0f; // [m]
    const float dev = 0.50f; // [m]
    Offboard::PositionLocalNED position = {x, y, z};
    offboard->set_position_local_ned(position);

    Offboard::Result offboard_result = offboard->start();

    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    offboard->set_position_local_ned(position);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    EXPECT_GT(telemetry->position().relative_altitude_m, -z - dev);
    EXPECT_LT(telemetry->position().relative_altitude_m, -z + dev);

    x = 3.0f; // [m]
    y = 3.0f; // [m]
    z = -3.0f; // [m]
    position = {x, y, z};
    offboard->set_position_local_ned(position);
    std::this_thread::sleep_for(std::chrono::seconds(20));
    EXPECT_GT(telemetry->position().relative_altitude_m, -z - dev);
    EXPECT_LT(telemetry->position().relative_altitude_m, -z + dev);

    x = 3.0f; // [m]
    y = -3.0f; // [m]
    z = -5.0f; // [m]
    position = {x, y, z};
    offboard->set_position_local_ned(position);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    EXPECT_GT(telemetry->position().relative_altitude_m, -z - dev);
    EXPECT_LT(telemetry->position().relative_altitude_m, -z + dev);

    x = 0.0f; // [m]
    y = 0.0f; // [m]
    z = -5.0f; // [m]
    position = {x, y, z};
    offboard->set_position_local_ned(position);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    EXPECT_GT(telemetry->position().relative_altitude_m, -z - dev);
    EXPECT_LT(telemetry->position().relative_altitude_m, -z + dev);

    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    action_ret = action->land();
    EXPECT_EQ(action_ret, ActionResult::SUCCESS);
}
