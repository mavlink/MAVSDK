#include <iostream>
#include "integration_test_helper.h"
#include "dronecore.h"

using namespace dronecore;

TEST_F(SitlTest, ActionSimpleHover)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, DroneCore::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = device.action().arm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    action_ret = device.action().takeoff();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    std::this_thread::sleep_for(std::chrono::seconds(5));

    action_ret = device.action().land();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);

    while (device.telemetry().in_air()) {
        std::cout << "waiting for device to be landed" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    action_ret = device.action().disarm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
}

