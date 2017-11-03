#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecore.h"

using namespace dronecore;
using namespace std::chrono;
using namespace std::this_thread;

#ifndef FOLLOW_ME_TESTING
#define FOLLOW_ME_TESTING
#endif

void print_flight_mode(Telemetry::FlightMode flight_mode);

TEST_F(SitlTest, FollowMe)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(DroneCore::ConnectionResult::SUCCESS, ret);

    // Wait for device to connect via heartbeat.
    sleep_for(seconds(2));
    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        sleep_for(seconds(1));
    }

    Action::Result action_ret = device.action().arm();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    device.telemetry().flight_mode_async(
        std::bind(&print_flight_mode, std::placeholders::_1));

    action_ret = device.action().takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    sleep_for(seconds(5));

    FollowMe::Result followme_result = device.followme().start();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);

    // Let the device follow GCS for 20 sec, say.
    sleep_for(seconds(20));

    followme_result = device.followme().stop();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);
    sleep_for(seconds(2)); // to watch flight mode change from "FollowMe" to default "HOLD"
}

void print_flight_mode(Telemetry::FlightMode flight_mode)
{
    std::cout << "FlightMode: " << Telemetry::flight_mode_str(flight_mode)
              << std::endl;
}
