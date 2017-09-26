#include <iostream>
#include "integration_test_helper.h"
#include "dronecore.h"

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

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, DroneCore::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_TRUE(dc.is_connected());

    Device &device = dc.device();

    for (int i = 0; ; ++i) {
        if (device.telemetry().health_all_ok()) {
            break;
        }
        std::cout << "waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        ASSERT_LT(i, 10);
    }

    Action::Result action_ret = device.action().arm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (std::isfinite(altitude_m)) {
        device.action().set_takeoff_altitude(altitude_m);
    } else {
        // The default should be 2.5 m, so we check against that.
        altitude_m = 2.5f;
    }

    action_ret = device.action().takeoff();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    // We wait 1.5s / m plus a margin of 3s.
    const int wait_time_s = (int)(altitude_m * 1.5f + 3.0f);
    std::this_thread::sleep_for(std::chrono::seconds(wait_time_s));


    EXPECT_GT(device.telemetry().position().relative_altitude_m, altitude_m - 0.25f);
    EXPECT_LT(device.telemetry().position().relative_altitude_m, altitude_m + 0.25f);

    action_ret = device.action().land();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);

    for (int i = 0; ; ++i) {
        if (!device.telemetry().in_air()) {
            break;
        }
        std::cout << "waiting for device to be landed" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // TODO: currently we need to wait a long time until landed is detected.
        ASSERT_LT(i, wait_time_s + 5);
    }

    action_ret = device.action().disarm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
}
