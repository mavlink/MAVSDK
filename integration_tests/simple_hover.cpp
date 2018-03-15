#include <iostream>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

using namespace dronecore;


static void takeoff_and_hover_at_altitude(float altitude_m = NAN);


TEST_F(SitlTest, ActionSimpleHoverDefault)
{
    takeoff_and_hover_at_altitude();
}

TEST_F(SitlTest, ActionSimpleHoverHigher)
{
    takeoff_and_hover_at_altitude(5.0f);
}

TEST_F(SitlTest, ActionSimpleHoverLower)
{
    takeoff_and_hover_at_altitude(1.0f);
}


void takeoff_and_hover_at_altitude(float altitude_m)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_TRUE(dc.is_connected());

    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);

    int iteration = 0;
    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        ASSERT_LT(++iteration, 10);
    }

    ActionResult action_ret = action->arm();
    EXPECT_EQ(action_ret, ActionResult::SUCCESS);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (std::isfinite(altitude_m)) {
        action->set_takeoff_altitude(altitude_m);
    } else {
        // The default should be 2.5 m, so we check against that.
        altitude_m = 2.5f;
    }

    action_ret = action->takeoff();
    EXPECT_EQ(action_ret, ActionResult::SUCCESS);
    // We wait 1.5s / m plus a margin of 3s.
    const int wait_time_s = static_cast<int>(altitude_m * 1.5f + 3.0f);
    std::this_thread::sleep_for(std::chrono::seconds(wait_time_s));


    EXPECT_GT(telemetry->position().relative_altitude_m, altitude_m - 0.25f);
    EXPECT_LT(telemetry->position().relative_altitude_m, altitude_m + 0.25f);

    action_ret = action->land();
    EXPECT_EQ(action_ret, ActionResult::SUCCESS);

    iteration = 0;
    while (telemetry->in_air()) {
        std::cout << "waiting for system to be landed" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // TODO: currently we need to wait a long time until landed is detected.
        ASSERT_LT(++iteration, 2 * wait_time_s);
    }

    action_ret = action->disarm();
    EXPECT_EQ(action_ret, ActionResult::SUCCESS);
}
