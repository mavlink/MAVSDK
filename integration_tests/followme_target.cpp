#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecore.h"

using namespace dronecore;

TEST_F(SitlTest, FollowMeTargetTest)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(DroneCore::ConnectionResult::SUCCESS, ret);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = device.action().arm();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    action_ret = device.action().takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    FollowMe::Result followme_result = device.followme().start();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);

    std::cout << "[FollowMe] Waiting for 20 secs :-) ...";
    std::this_thread::sleep_for(std::chrono::seconds(20));
    std::cout << "Done." << std::endl;
}
