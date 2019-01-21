#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

using namespace dronecode_sdk;

static void takeoff_and_hover_at_altitude(float altitude_m);

TEST_F(SitlTest, ActionHoverSyncDefault)
{
    takeoff_and_hover_at_altitude(2.5);
}

TEST_F(SitlTest, ActionHoverSyncHigher)
{
    takeoff_and_hover_at_altitude(5.0f);
}

TEST_F(SitlTest, ActionHoverSyncLower)
{
    takeoff_and_hover_at_altitude(1.0f);
}

void takeoff_and_hover_at_altitude(float altitude_m)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    LogInfo() << "Waiting for system connect";
    ASSERT_TRUE(wait_for_cond_sync([&dc]() { return dc.is_connected(); }, std::chrono::seconds(1)));

    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(wait_for_cond_sync([&telemetry]() { return telemetry->health_all_ok(); },
                                   std::chrono::seconds(10)));

    auto action = std::make_shared<Action>(system);
    Action::Result action_ret = action->arm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);

    EXPECT_EQ(Action::Result::SUCCESS, action->set_takeoff_altitude(altitude_m));
    auto takeoff_altitude_result = action->get_takeoff_altitude();
    EXPECT_EQ(takeoff_altitude_result.first, Action::Result::SUCCESS);
    EXPECT_FLOAT_EQ(takeoff_altitude_result.second, altitude_m);

    action_ret = action->takeoff();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);

    // TODO: The wait time should not be hard-coded because the
    //       simulation might run faster.

    // We wait 2s / m plus a margin of 5s.
    const int wait_time_s = static_cast<int>(altitude_m * 2.0f + 5.0f);
    std::this_thread::sleep_for(std::chrono::seconds(wait_time_s));

    EXPECT_GT(telemetry->position().relative_altitude_m, altitude_m - 0.25f);
    EXPECT_LT(telemetry->position().relative_altitude_m, altitude_m + 0.25f);

    action_ret = action->land();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);

    EXPECT_TRUE(wait_for_cond_sync([&telemetry]() { return !telemetry->in_air(); },
                                   std::chrono::seconds(wait_time_s)));

    action_ret = action->disarm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
}
