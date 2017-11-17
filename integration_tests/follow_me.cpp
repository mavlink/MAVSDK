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

void print(const FollowMe::Config &config);
void print(const FollowMe::FollowInfo &info);

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
    std::bind([](Telemetry::FlightMode flight_mode) {
        std::cout << "FlightMode: " << Telemetry::flight_mode_str(flight_mode)
                  << std::endl;

    }, std::placeholders::_1));

    action_ret = device.action().takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    sleep_for(seconds(5));

    auto defult_config = device.followme().get_config();
    print(defult_config);

    auto info = device.followme().get_follow_info();
    print(info);

    // start following with default configuration
    FollowMe::Result followme_result = device.followme().start();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);

    // Let the device follow you (GCS) for half a min, say.
    sleep_for(seconds(30));

    // stop following
    followme_result = device.followme().stop();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);
    sleep_for(seconds(2)); // to watch flight mode change from "FollowMe" to default "HOLD"

    action_ret = device.action().land();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);
    sleep_for(seconds(2)); // let the device land
}

TEST_F(SitlTest, FollowMeWithConfig)
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
    std::bind([](Telemetry::FlightMode flight_mode) {
        std::cout << "FlightMode: " << Telemetry::flight_mode_str(flight_mode)
                  << std::endl;

    }, std::placeholders::_1));

    action_ret = device.action().takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    sleep_for(seconds(5));

    // configure follow me behaviour
    FollowMe::Config config;
    config.min_height_m = 30.f; // increase min height
    config.follow_dist_m = 20.f; // set distance b/w device and GCS during FollowMe mode
    config.responsiveness = 0.2f; // set to higher responsiveness
    config.follow_dir = FollowMe::Config::FollowDirection::FRONT; // Device follows you from FRONT side

    // Apply configuration
    bool configured = device.followme().set_config(config);
    ASSERT_EQ(true, configured);

    // set initial motion report details
    FollowMe::FollowInfo info;
    info.lat += .000003;
    device.followme().set_follow_info(info);
    print(info);

    // start following
    FollowMe::Result followme_result = device.followme().start();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);

    // Let the device follow you (GCS) for half a min, say.
    sleep_for(seconds(30));

    // stops following
    followme_result = device.followme().stop();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);
    sleep_for(seconds(2)); // to watch flight mode change from "FollowMe" to default "HOLD"

    action_ret = device.action().land();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);
}


void print(Telemetry::FlightMode flight_mode)
{
    std::cout << "FlightMode: " << Telemetry::flight_mode_str(flight_mode)
              << std::endl;
}

void print(const FollowMe::Config &config)
{
    std::cout << "Default PX4 FollowMe config" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << "Min Height: " << config.min_height_m << "m" << std::endl;
    std::cout << "Distance: " << config.follow_dist_m << "m" << std::endl;
    std::cout << "Responsiveness: " << config.responsiveness << std::endl;
    std::cout << "Following from: " << FollowMe::Config::to_str(config.follow_dir) << std::endl;
    std::cout << "---------------------------" << std::endl;
}

void print(const FollowMe::FollowInfo &info)
{
    std::cout << "Follow info" << std::endl;
    std::cout << "-----------" << std::endl;
    std::cout << "GCS at Lat: " << info.lat << " Lon: " << info.lon << " Alt: " <<
              info.alt << std::endl;
    std::cout << "-----------" << std::endl;
}
