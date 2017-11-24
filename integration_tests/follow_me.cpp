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
using namespace std::placeholders;


void print(const FollowMe::Config &config);
void send_location_updates(FollowMe &follow_me_handle, int count, float rate);

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
    std::bind([](Telemetry::FlightMode mode) {
        std::cout << "FlightMode: " << Telemetry::flight_mode_str(mode)
                  << std::endl;

    }, std::placeholders::_1));

    action_ret = device.action().takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    sleep_for(seconds(5)); // let it reach takeoff altitude

    auto curr_config = device.followme().get_config();
    print(curr_config);

    // start following with default configuration
    FollowMe::Result followme_result = device.followme().start();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);

    // send location updates every second
    send_location_updates(device.followme(), 10, 1.f);

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
    config.min_height_m = 12.f; // increase min height
    config.follow_dist_m = 20.f; // set distance b/w device and GCS during FollowMe mode
    config.responsiveness = 0.2f; // set to higher responsiveness
    config.follow_dir = FollowMe::Config::FollowDirection::FRONT; // Device follows you from FRONT side

    // Apply configuration
    bool configured = device.followme().set_config(config);
    ASSERT_EQ(true, configured);

    sleep_for(seconds(2)); // untill config is applied

    // Verify your configuration
    auto curr_config = device.followme().get_config();
    print(curr_config);

    // Start following
    FollowMe::Result followme_result = device.followme().start();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);

    // send location updates once in every 2 seconds
    send_location_updates(device.followme(), 40, 0.5f);

    // Stop following
    followme_result = device.followme().stop();
    ASSERT_EQ(FollowMe::Result::SUCCESS, followme_result);
    sleep_for(seconds(2)); // to watch flight mode change from "FollowMe" to default "HOLD"

    action_ret = device.action().land();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);
    sleep_for(seconds(2)); // let it land
}

void print(const FollowMe::Config &config)
{
    std::cout << "Current FollowMe configuration of the device" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << "Min Height: " << config.min_height_m << "m" << std::endl;
    std::cout << "Distance: " << config.follow_dist_m << "m" << std::endl;
    std::cout << "Responsiveness: " << config.responsiveness << std::endl;
    std::cout << "Following from: " << FollowMe::Config::to_str(config.follow_dir) << std::endl;
    std::cout << "---------------------------" << std::endl;
}

void send_location_updates(FollowMe &follow_me, int count, float rate)
{
    // TODO: Generate these co-ordinates from algorithm
    FollowMe::TargetLocation spiral_path[] = {
        { 47.3997373, 8.5426978, 490.0, 0.f, 0.f, 0.f },
        { 47.39973730, 8.54269780, 510.0, 0.f, 0.f, 0.f },
        { 47.39780291, 8.54557048, 490.0, 0.f, 0.f, 0.f },
        { 47.39779838, 8.54555174, 492.0, 0.f, 0.f, 0.f },
        { 47.39778748, 8.54554499, 493.0, 0.f, 0.f, 0.f },
        { 47.39777659, 8.54553561, 494.0, 0.f, 0.f, 0.f },
        { 47.39776569, 8.54553292, 494.0, 0.f, 0.f, 0.f },
        { 47.39774663, 8.54552622, 501.0, 0.f, 0.f, 0.f },
        { 47.39772938, 8.54552488, 501.0, 0.f, 0.f, 0.f },
        { 47.39771304, 8.54554231, 502.0, 0.f, 0.f, 0.f },
        { 47.39770578, 8.54557445, 504.0, 0.f, 0.f, 0.f },
        { 47.39770487, 8.54559596, 502.0, 0.f, 0.f, 0.f },
        { 47.39770578, 8.54561741, 502.0, 0.f, 0.f, 0.f },
        { 47.39770669, 8.54563887, 502.0, 0.f, 0.f, 0.f },
        { 47.39771486, 8.54565765, 493.0, 0.f, 0.f, 0.f },
        { 47.39773029, 8.54567642, 496.0, 0.f, 0.f, 0.f },
        { 47.39775026, 8.54568447, 502.0, 0.f, 0.f, 0.f },
        { 47.39776751, 8.54569118, 502.0, 0.f, 0.f, 0.f },
        { 47.39778203, 8.54569118, 502.0, 0.f, 0.f, 0.f },
        { 47.39779838, 8.54568447, 502.0, 0.f, 0.f, 0.f },
        { 47.39781835, 8.54566972, 502.0, 0.f, 0.f, 0.f },
        { 47.39782107, 8.54564692, 502.0, 0.f, 0.f, 0.f },
        { 47.39782474, 8.54561876, 502.0, 0.f, 0.f, 0.f },
        { 47.39782475, 8.54559193, 501.0, 0.f, 0.f, 0.f },
        { 47.39782474, 8.54556511, 501.0, 0.f, 0.f, 0.f },
        { 47.39782107, 8.54553427, 503.0, 0.f, 0.f, 0.f },
        { 47.39780836, 8.54552756, 502.0, 0.f, 0.f, 0.f },
        { 47.39779656, 8.54551549, 501.0, 0.f, 0.f, 0.f },
        { 47.39777568, 8.54550342, 501.0, 0.f, 0.f, 0.f },
        { 47.39775482, 8.54549671, 504.0, 0.f, 0.f, 0.f },
        { 47.39773755, 8.54549671, 501.0, 0.f, 0.f, 0.f },
        { 47.39771849, 8.54550208, 501.0, 0.f, 0.f, 0.f },
        { 47.39770396, 8.54551415, 502.0, 0.f, 0.f, 0.f },
        { 47.39769398, 8.54554097, 501.0, 0.f, 0.f, 0.f },
        { 47.39768762, 8.54556243, 501.0, 0.f, 0.f, 0.f },
        { 47.39768672, 8.54557852, 501.0, 0.f, 0.f, 0.f },
        { 47.39768493, 8.54559998, 502.0, 0.f, 0.f, 0.f },
        { 47.39768399, 8.54562278, 501.0, 0.f, 0.f, 0.f },
        { 47.39768399, 8.54564155, 501.0, 0.f, 0.f, 0.f },
        { 47.39769035, 8.54566569, 501.0, 0.f, 0.f, 0.f },
        { 47.39770759, 8.54568983, 502.0, 0.f, 0.f, 0.f },
        { 47.39772757, 8.54569922, 502.0, 0.f, 0.f, 0.f },
        { 47.39774481, 8.54570727, 507.0, 0.f, 0.f, 0.f },
        { 47.39776025, 8.54572202, 502.0, 0.f, 0.f, 0.f },
        { 47.39778567, 8.54572336, 505.0, 0.f, 0.f, 0.f },
        { 47.39780291, 8.54572202, 507.0, 0.f, 0.f, 0.f },
        { 47.39782107, 8.54571263, 502.0, 0.f, 0.f, 0.f },
        { 47.39783469, 8.54569788, 501.0, 0.f, 0.f, 0.f },
        { 47.39783832, 8.54568179, 501.0, 0.f, 0.f, 0.f },
        { 47.39784104, 8.54566569, 503.0, 0.f, 0.f, 0.f },
        { 47.39784376, 8.54564424, 502.0, 0.f, 0.f, 0.f },
        { 47.39784386, 8.54564435, 503.0, 0.f, 0.f, 0.f },
        { 47.39784396, 8.54564444, 505.0, 0.f, 0.f, 0.f },
        { 47.39784386, 8.54564454, 503.0, 0.f, 0.f, 0.f },
        { 47.39784346, 8.54564464, 504.0, 0.f, 0.f, 0.f },
        { 47.39784336, 8.54564424, 501.0, 0.f, 0.f, 0.f },
        { 47.39772757, 8.54569922, 501.0, 0.f, 0.f, 0.f },
        { 47.39774481, 8.54570727, 503.0, 0.f, 0.f, 0.f },
        { 47.39776025, 8.54572202, 501.0, 0.f, 0.f, 0.f },
        { 47.39778567, 8.54572336, 503.0, 0.f, 0.f, 0.f },
        { 47.39770396, 8.54551415, 501.0, 0.f, 0.f, 0.f },
        { 47.39769398, 8.54554097, 502.0, 0.f, 0.f, 0.f },
        { 47.39768762, 8.54556243, 501.0, 0.f, 0.f, 0.f },
        { 47.39768672, 8.54557852, 501.0, 0.f, 0.f, 0.f },
        { 47.39768494, 8.54559998, 501.0, 0.f, 0.f, 0.f },
        { 47.39779454, 8.54559464, 502.0, 0.f, 0.f, 0.f },
        { 47.39780291, 8.54557048, 489.0, 0.f, 0.f, 0.f },
        { 47.39779838, 8.54555173, 502.0, 0.f, 0.f, 0.f },
        { 47.39778748, 8.54554499, 508.0, 0.f, 0.f, 0.f },
        { 47.39777659, 8.54553561, 510.0, 0.f, 0.f, 0.f },
        { 47.39776569, 8.54553292, 501.0, 0.f, 0.f, 0.f },
        { 47.39774663, 8.54552622, 502.0, 0.f, 0.f, 0.f },
        { 47.39772938, 8.54552488, 505.0, 0.f, 0.f, 0.f },
        { 47.39771304, 8.54554231, 505.0, 0.f, 0.f, 0.f }
    };

    for (auto pos : spiral_path) {
        if (count-- == 0) {
            return;
        }
        follow_me.set_curr_target_location(pos);
        auto sleep_duration_ms = static_cast<int>(1 / rate * 1000);
        sleep_for(milliseconds(sleep_duration_ms));
    }
}
