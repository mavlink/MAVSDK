#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <array>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecode_sdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/follow_me/follow_me.h"

using namespace dronecode_sdk;
using namespace std::chrono;
using namespace std::this_thread;
using namespace std::placeholders;

void print(const FollowMe::Config &config);
void send_location_updates(std::shared_ptr<FollowMe> follow_me,
                           size_t count = 25ul,
                           float rate = 1.f);

const size_t N_LOCATIONS = 100ul;

TEST_F(SitlTest, FollowMeOneLocation)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ConnectionResult::SUCCESS, ret);

    // Wait for system to connect via heartbeat.
    sleep_for(seconds(2));
    System &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto follow_me = std::make_shared<FollowMe>(system);
    auto action = std::make_shared<Action>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        sleep_for(seconds(1));
    }

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    telemetry->flight_mode_async(std::bind(
        [&](Telemetry::FlightMode flight_mode) {
            const FollowMe::TargetLocation last_location = follow_me->get_last_location();

            std::cout << "[FlightMode: " << Telemetry::flight_mode_str(flight_mode)
                      << "] Vehicle is at Lat: " << last_location.latitude_deg << " deg, "
                      << "Lon: " << last_location.longitude_deg << " deg." << std::endl;
        },
        std::placeholders::_1));

    action_ret = action->takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    sleep_for(seconds(5)); // let it reach takeoff altitude

    auto curr_config = follow_me->get_config();
    print(curr_config);

    // Set just a single location before starting FollowMe (optional)
    follow_me->set_target_location({47.39768399, 8.54564155, 0.0, 0.f, 0.f, 0.f});

    // Start following with default configuration
    FollowMe::Result follow_me_result = follow_me->start();
    ASSERT_EQ(FollowMe::Result::SUCCESS, follow_me_result);
    sleep_for(seconds(1));

    std::cout << "We're waiting (for 5s) to see the drone moving target location set." << std::endl;
    sleep_for(seconds(5));

    // stop following
    follow_me_result = follow_me->stop();
    ASSERT_EQ(FollowMe::Result::SUCCESS, follow_me_result);
    sleep_for(seconds(2)); // to watch flight mode change from "FollowMe" to default "HOLD"

    action_ret = action->land();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);
    sleep_for(seconds(2)); // let the system land

    while (telemetry->armed()) {
        std::cout << "waiting for system to disarm" << std::endl;
        sleep_for(seconds(1));
    }
}

TEST_F(SitlTest, FollowMeMultiLocationWithConfig)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ConnectionResult::SUCCESS, ret);

    // Wait for system to connect via heartbeat.
    ASSERT_TRUE(poll_condition_with_timeout([&dc]() { return dc.is_connected(); },
                                            std::chrono::seconds(10)));
    System &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto follow_me = std::make_shared<FollowMe>(system);
    auto action = std::make_shared<Action>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "Waiting for system to be ready" << std::endl;
        sleep_for(seconds(1));
    }

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    telemetry->flight_mode_async(std::bind(
        [&](Telemetry::FlightMode flight_mode) {
            const FollowMe::TargetLocation last_location = follow_me->get_last_location();

            std::cout << "[FlightMode: " << Telemetry::flight_mode_str(flight_mode)
                      << "] Vehicle is at Lat: " << last_location.latitude_deg << " deg, "
                      << "Lon: " << last_location.longitude_deg << " deg." << std::endl;
        },
        std::placeholders::_1));

    action_ret = action->takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    sleep_for(seconds(5));

    // configure follow me behaviour
    FollowMe::Config config;
    config.min_height_m = 12.f; // increase min height
    config.follow_distance_m = 20.f; // set distance b/w system and target during FollowMe mode
    config.responsiveness = 0.2f; // set to higher responsiveness
    config.follow_direction =
        FollowMe::Config::FollowDirection::FRONT; // System follows target from FRONT side

    // Apply configuration
    FollowMe::Result config_result = follow_me->set_config(config);
    ASSERT_EQ(FollowMe::Result::SUCCESS, config_result);

    // Start following
    FollowMe::Result follow_me_result = follow_me->start();
    ASSERT_EQ(FollowMe::Result::SUCCESS, follow_me_result);

    // send location update every second
    send_location_updates(follow_me);

    // Stop following
    follow_me_result = follow_me->stop();
    ASSERT_EQ(FollowMe::Result::SUCCESS, follow_me_result);
    sleep_for(seconds(2)); // to watch flight mode change from "FollowMe" to default "HOLD"

    action_ret = action->land();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);
    sleep_for(seconds(2)); // let it land

    while (telemetry->armed()) {
        std::cout << "waiting for system to disarm" << std::endl;
        sleep_for(seconds(1));
    }
}

void print(const FollowMe::Config &config)
{
    std::cout << "Current FollowMe configuration of the system" << std::endl;
    std::cout << "---------------------------" << std::endl;
    std::cout << "Min Height: " << config.min_height_m << "m" << std::endl;
    std::cout << "Distance: " << config.follow_distance_m << "m" << std::endl;
    std::cout << "Responsiveness: " << config.responsiveness << std::endl;
    std::cout << "Following from: " << FollowMe::Config::to_str(config.follow_direction)
              << std::endl;
    std::cout << "---------------------------" << std::endl;
}

void send_location_updates(std::shared_ptr<FollowMe> follow_me, size_t count, float rate)
{
    // TODO: Generate these co-ordinates from an algorithm
    // Altitude here is ignored by PX4, as we've set min altitude in configuration.
    std::array<FollowMe::TargetLocation, N_LOCATIONS> spiral_path = {
        {{47.39768399, 8.54564155, 0.0, 0.f, 0.f, 0.f},
         {47.39769035, 8.54566569, 0.0, 0.f, 0.f, 0.f},
         {47.39770759, 8.54568983, 0.0, 0.f, 0.f, 0.f},
         {47.39772757, 8.54569922, 0.0, 0.f, 0.f, 0.f},
         {47.39774481, 8.54570727, 0.0, 0.f, 0.f, 0.f},
         {47.39776025, 8.54572202, 0.0, 0.f, 0.f, 0.f},
         {47.39778567, 8.54572336, 0.0, 0.f, 0.f, 0.f},
         {47.39780291, 8.54572202, 0.0, 0.f, 0.f, 0.f},
         {47.39782107, 8.54571263, 0.0, 0.f, 0.f, 0.f},
         {47.39783469, 8.54569788, 0.0, 0.f, 0.f, 0.f},
         {47.39783832, 8.54568179, 0.0, 0.f, 0.f, 0.f},
         {47.39784104, 8.54566569, 0.0, 0.f, 0.f, 0.f},
         {47.39784376, 8.54564424, 0.0, 0.f, 0.f, 0.f},
         {47.39772938, 8.54552488, 0.0, 0.f, 0.f, 0.f},
         {47.39782475, 8.54559193, 0.0, 0.f, 0.f, 0.f},
         {47.39780291, 8.54557048, 0.0, 0.f, 0.f, 0.f},
         {47.39771304, 8.54554231, 0.0, 0.f, 0.f, 0.f},
         {47.39780836, 8.54552756, 0.0, 0.f, 0.f, 0.f},
         {47.39973737, 8.54269845, 0.0, 0.f, 0.f, 0.f},
         {47.39973730, 8.54269780, 0.0, 0.f, 0.f, 0.f},
         {47.39779838, 8.54555174, 0.0, 0.f, 0.f, 0.f},
         {47.39778748, 8.54554499, 0.0, 0.f, 0.f, 0.f},
         {47.39777659, 8.54553561, 0.0, 0.f, 0.f, 0.f},
         {47.39776569, 8.54553292, 0.0, 0.f, 0.f, 0.f},
         {47.39774663, 8.54552622, 0.0, 0.f, 0.f, 0.f},
         {47.39772938, 8.54552488, 0.0, 0.f, 0.f, 0.f},
         {47.39771304, 8.54554231, 0.0, 0.f, 0.f, 0.f},
         {47.39770578, 8.54557445, 0.0, 0.f, 0.f, 0.f},
         {47.39770487, 8.54559596, 0.0, 0.f, 0.f, 0.f},
         {47.39770578, 8.54561741, 0.0, 0.f, 0.f, 0.f},
         {47.39770669, 8.54563887, 0.0, 0.f, 0.f, 0.f},
         {47.39771486, 8.54565765, 0.0, 0.f, 0.f, 0.f},
         {47.39773029, 8.54567642, 0.0, 0.f, 0.f, 0.f},
         {47.39775026, 8.54568447, 0.0, 0.f, 0.f, 0.f},
         {47.39776751, 8.54569118, 0.0, 0.f, 0.f, 0.f},
         {47.39778203, 8.54569118, 0.0, 0.f, 0.f, 0.f},
         {47.39779838, 8.54568447, 0.0, 0.f, 0.f, 0.f},
         {47.39781835, 8.54566972, 0.0, 0.f, 0.f, 0.f},
         {47.39782107, 8.54564692, 0.0, 0.f, 0.f, 0.f},
         {47.39782474, 8.54561876, 0.0, 0.f, 0.f, 0.f},
         {47.39782474, 8.54556511, 0.0, 0.f, 0.f, 0.f},
         {47.39782107, 8.54553427, 0.0, 0.f, 0.f, 0.f},
         {47.39779656, 8.54551549, 0.0, 0.f, 0.f, 0.f},
         {47.39777568, 8.54550342, 0.0, 0.f, 0.f, 0.f},
         {47.39775482, 8.54549671, 0.0, 0.f, 0.f, 0.f},
         {47.39773755, 8.54549671, 0.0, 0.f, 0.f, 0.f},
         {47.39771849, 8.54550208, 0.0, 0.f, 0.f, 0.f},
         {47.39770396, 8.54551415, 0.0, 0.f, 0.f, 0.f},
         {47.39769398, 8.54554097, 0.0, 0.f, 0.f, 0.f},
         {47.39768762, 8.54556243, 0.0, 0.f, 0.f, 0.f},
         {47.39768672, 8.54557852, 0.0, 0.f, 0.f, 0.f},
         {47.39768493, 8.54559998, 0.0, 0.f, 0.f, 0.f},
         {47.39768399, 8.54562278, 0.0, 0.f, 0.f, 0.f},
         {47.39768399, 8.54564155, 0.0, 0.f, 0.f, 0.f},
         {47.39769035, 8.54566569, 0.0, 0.f, 0.f, 0.f},
         {47.39770759, 8.54568983, 0.0, 0.f, 0.f, 0.f},
         {47.39772757, 8.54569922, 0.0, 0.f, 0.f, 0.f},
         {47.39774481, 8.54570727, 0.0, 0.f, 0.f, 0.f},
         {47.39776025, 8.54572202, 0.0, 0.f, 0.f, 0.f},
         {47.39778567, 8.54572336, 0.0, 0.f, 0.f, 0.f},
         {47.39780291, 8.54572202, 0.0, 0.f, 0.f, 0.f},
         {47.39782107, 8.54571263, 0.0, 0.f, 0.f, 0.f},
         {47.39783469, 8.54569788, 0.0, 0.f, 0.f, 0.f},
         {47.39783832, 8.54568179, 0.0, 0.f, 0.f, 0.f},
         {47.39784104, 8.54566569, 0.0, 0.f, 0.f, 0.f},
         {47.39784376, 8.54564424, 0.0, 0.f, 0.f, 0.f},
         {47.39784386, 8.54564435, 0.0, 0.f, 0.f, 0.f},
         {47.39784396, 8.54564444, 0.0, 0.f, 0.f, 0.f},
         {47.39784386, 8.54564454, 0.0, 0.f, 0.f, 0.f},
         {47.39784346, 8.54564464, 0.0, 0.f, 0.f, 0.f},
         {47.39784336, 8.54564424, 0.0, 0.f, 0.f, 0.f},
         {47.39772757, 8.54569922, 0.0, 0.f, 0.f, 0.f},
         {47.39774481, 8.54570727, 0.0, 0.f, 0.f, 0.f},
         {47.39776025, 8.54572202, 0.0, 0.f, 0.f, 0.f},
         {47.39778567, 8.54572336, 0.0, 0.f, 0.f, 0.f},
         {47.39770396, 8.54551415, 0.0, 0.f, 0.f, 0.f},
         {47.39769398, 8.54554097, 0.0, 0.f, 0.f, 0.f},
         {47.39768762, 8.54556243, 0.0, 0.f, 0.f, 0.f},
         {47.39768672, 8.54557852, 0.0, 0.f, 0.f, 0.f},
         {47.39768494, 8.54559998, 0.0, 0.f, 0.f, 0.f},
         {47.39779454, 8.54559464, 0.0, 0.f, 0.f, 0.f},
         {47.39780291, 8.54557048, 0.0, 0.f, 0.f, 0.f},
         {47.39779838, 8.54555173, 0.0, 0.f, 0.f, 0.f},
         {47.39778748, 8.54554499, 0.0, 0.f, 0.f, 0.f},
         {47.39777659, 8.54553561, 0.0, 0.f, 0.f, 0.f},
         {47.39776569, 8.54553292, 0.0, 0.f, 0.f, 0.f},
         {47.39774663, 8.54552622, 0.0, 0.f, 0.f, 0.f},
         {47.39771304, 8.54554231, 0.0, 0.f, 0.f, 0.f},
         {47.39772938, 8.54552488, 0.0, 0.f, 0.f, 0.f},
         {47.39771304, 8.54554231, 0.0, 0.f, 0.f, 0.f},
         {47.39770578, 8.54557445, 0.0, 0.f, 0.f, 0.f},
         {47.39770487, 8.54559596, 0.0, 0.f, 0.f, 0.f},
         {47.39770578, 8.54561741, 0.0, 0.f, 0.f, 0.f},
         {47.39770669, 8.54563887, 0.0, 0.f, 0.f, 0.f},
         {47.39771486, 8.54565765, 0.0, 0.f, 0.f, 0.f},
         {47.39773029, 8.54567642, 0.0, 0.f, 0.f, 0.f},
         {47.39775026, 8.54568447, 0.0, 0.f, 0.f, 0.f},
         {47.39776751, 8.54569118, 0.0, 0.f, 0.f, 0.f},
         {47.39784346, 8.54564464, 0.0, 0.f, 0.f, 0.f},
         {47.39776569, 8.54553292, 0.0, 0.f, 0.f, 0.f}}};

    // We're limiting to N_LOCATIONS for testing.
    count = (count > N_LOCATIONS) ? N_LOCATIONS : count;
    std::cout << "# of Target locations: " << count << " @ Rate: " << rate << "Hz." << std::endl;

    for (auto pos : spiral_path) {
        if (count-- == 0) {
            return;
        }
        follow_me->set_target_location(pos);
        auto sleep_duration_ms = static_cast<int>(1 / rate * 1000);
        sleep_for(milliseconds(sleep_duration_ms));
    }
}
