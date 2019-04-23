#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

using namespace dronecode_sdk;

TEST_F(SitlTest, ActionGoto)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_TRUE(dc.is_connected());

    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);

    int iteration = 0;
    while (!telemetry->health_all_ok()) {
        LogInfo() << "waiting for system to be ready";
        std::this_thread::sleep_for(std::chrono::seconds(1));

        ASSERT_LT(++iteration, 20);
    }

    auto action = std::make_shared<Action>(system);
    Action::Result action_ret = action->arm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    action_ret = action->takeoff();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Go somewhere
    action->goto_location(47.398000, 8.545592, NAN, NAN);
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // And back
    action->goto_location(47.3977233, 8.545592, NAN, NAN);
    std::this_thread::sleep_for(std::chrono::seconds(10));

    action_ret = action->land();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);

    iteration = 0;
    while (telemetry->in_air()) {
        LogInfo() << "waiting for system to be landed";
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // TODO: currently we need to wait a long time until landed is detected.
        ASSERT_LT(++iteration, 20);
    }

    action_ret = action->disarm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
}
