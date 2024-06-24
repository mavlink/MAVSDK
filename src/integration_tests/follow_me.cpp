#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <array>

#include "integration_test_helper.h"

#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/follow_me/follow_me.h"
#include "plugins/info/info.h"
#include "plugins/param/param.h"

using namespace mavsdk;
using namespace std::chrono;
using namespace std::this_thread;

/* Check if the autopilot supports improved follow me */
bool autopilot_has_improved_followme(const std::shared_ptr<Param> param);

/* Auxilary Functions */
void print(const FollowMe::Config& config);

void send_location_updates(
    std::shared_ptr<FollowMe> follow_me, size_t count = 25ul, float rate = 2.f);

const size_t N_LOCATIONS = 100ul;

/* Test FollowMe with a stationary target at one location */
TEST(SitlTest, PX4FollowMeOneLocation)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ConnectionResult::Success, ret);

    // Wait for system to connect via heartbeat.
    sleep_for(seconds(2));
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto follow_me = std::make_shared<FollowMe>(system);
    auto action = std::make_shared<Action>(system);
    auto param = std::make_shared<Param>(system);

    // Skip the test if the Autopilot doesn't support the improved Follow Me (in MAVSDK v2)
    if (!autopilot_has_improved_followme(param)) {
        GTEST_SKIP();
    }

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::Success, action_ret);

    telemetry->subscribe_flight_mode([&follow_me](Telemetry::FlightMode flight_mode) {
        const FollowMe::TargetLocation last_location = follow_me->get_last_location();

        std::cout << "[FlightMode: " << flight_mode
                  << "] Vehicle is at Lat: " << last_location.latitude_deg << " deg, "
                  << "Lon: " << last_location.longitude_deg << " deg." << '\n';
    });

    action_ret = action->takeoff();
    ASSERT_EQ(Action::Result::Success, action_ret);

    sleep_for(seconds(5)); // let it reach takeoff altitude

    auto curr_config = follow_me->get_config();
    print(curr_config);

    // Set just a single location before starting FollowMe (optional)
    FollowMe::TargetLocation some_location;
    some_location.latitude_deg = 47.39768399;
    some_location.longitude_deg = 8.54564155;
    some_location.absolute_altitude_m = 0.0;
    some_location.velocity_x_m_s = 0.f;
    some_location.velocity_y_m_s = 0.f;
    some_location.velocity_z_m_s = 0.f;
    follow_me->set_target_location(some_location);

    // Start following with default configuration
    FollowMe::Result follow_me_result = follow_me->start();
    ASSERT_EQ(FollowMe::Result::Success, follow_me_result);
    sleep_for(seconds(1));

    std::cout << "We're waiting (for 5s) to see the drone moving target location set." << '\n';
    sleep_for(seconds(5));

    // stop following
    follow_me_result = follow_me->stop();
    ASSERT_EQ(FollowMe::Result::Success, follow_me_result);
    sleep_for(seconds(2)); // to watch flight mode change from "FollowMe" to default "HOLD"

    action_ret = action->land();
    ASSERT_EQ(Action::Result::Success, action_ret);
    sleep_for(seconds(2)); // let the system land

    while (telemetry->armed()) {
        std::cout << "waiting for system to disarm" << '\n';
        sleep_for(seconds(1));
    }
}

/* Test FollowMe with a dynamically moving target */
TEST(SitlTest, PX4FollowMeMultiLocationWithConfig)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ConnectionResult::Success, ret);

    // Wait for system to connect via heartbeat.
    ASSERT_TRUE(poll_condition_with_timeout(
        [&mavsdk]() {
            const auto systems = mavsdk.systems();

            if (systems.size() == 0) {
                return false;
            }

            const auto system = mavsdk.systems().at(0);
            return system->is_connected();
        },
        std::chrono::seconds(10)));
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto follow_me = std::make_shared<FollowMe>(system);
    auto action = std::make_shared<Action>(system);
    auto param = std::make_shared<Param>(system);

    // Skip the test if the Autopilot doesn't support the improved Follow Me (in MAVSDK v2)
    if (!autopilot_has_improved_followme(param)) {
        GTEST_SKIP();
    }

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::Success, action_ret);

    telemetry->subscribe_flight_mode([&follow_me](Telemetry::FlightMode flight_mode) {
        const FollowMe::TargetLocation last_location = follow_me->get_last_location();

        std::cout << "[FlightMode: " << flight_mode
                  << "] Vehicle is at Lat: " << last_location.latitude_deg << " deg, "
                  << "Lon: " << last_location.longitude_deg << " deg." << '\n';
    });

    action_ret = action->takeoff();
    ASSERT_EQ(Action::Result::Success, action_ret);

    sleep_for(seconds(5));

    // configure follow me behaviour
    FollowMe::Config config;
    config.follow_height_m = 12.f; // increase min height
    config.follow_distance_m = 20.f; // set distance b/w system and target during FollowMe mode
    config.responsiveness = 0.2f; // Make it less responsive (higher value for the setting)
    config.follow_angle_deg = 0.0; // System follows target from FRONT side

    // Apply configuration
    FollowMe::Result config_result = follow_me->set_config(config);
    ASSERT_EQ(FollowMe::Result::Success, config_result);

    // Start following
    FollowMe::Result follow_me_result = follow_me->start();
    ASSERT_EQ(FollowMe::Result::Success, follow_me_result);

    // send location update every second
    send_location_updates(follow_me);

    // Stop following
    follow_me_result = follow_me->stop();
    ASSERT_EQ(FollowMe::Result::Success, follow_me_result);
    sleep_for(seconds(2)); // to watch flight mode change from "FollowMe" to default "HOLD"

    action_ret = action->land();
    ASSERT_EQ(Action::Result::Success, action_ret);
    sleep_for(seconds(2)); // let it land

    while (telemetry->armed()) {
        std::cout << "waiting for system to disarm" << '\n';
        sleep_for(seconds(1));
    }
}

void print(const FollowMe::Config& config)
{
    std::cout << "Current FollowMe configuration of the system" << '\n';
    std::cout << "---------------------------" << '\n';
    std::cout << "Height: " << config.follow_height_m << "m" << '\n';
    std::cout << "Distance: " << config.follow_distance_m << "m" << '\n';
    std::cout << "Following angle: " << config.follow_angle_deg << "[deg]" << '\n';
    std::cout << "Responsiveness: " << config.responsiveness << '\n';
    std::cout << "---------------------------" << '\n';
}

FollowMe::TargetLocation create_target_location(double latitude_deg, double longitude_deg)
{
    FollowMe::TargetLocation location;
    location.latitude_deg = latitude_deg;
    location.longitude_deg = longitude_deg;
    location.absolute_altitude_m = 0.0;
    location.velocity_x_m_s = 0.f;
    location.velocity_y_m_s = 0.f;
    location.velocity_z_m_s = 0.f;
    return location;
}

bool autopilot_has_improved_followme(const std::shared_ptr<Param> param)
{
    // Check if the newly added parameter in the improved follow-me exists
    // Improved Follow-Me PR: https://github.com/PX4/PX4-Autopilot/pull/18026
    const std::pair<Param::Result, float> get_result = param->get_param_float("FLW_TGT_MAX_VEL");

    if (get_result.first == Param::Result::Success) {
        return true;
    } else {
        return false;
    }
}

void send_location_updates(std::shared_ptr<FollowMe> follow_me, size_t count, float rate)
{
    // TODO: Generate these co-ordinates from an algorithm
    // Altitude here is ignored by PX4, as we've set min altitude in configuration.
    std::array<FollowMe::TargetLocation, N_LOCATIONS> spiral_path = {
        create_target_location(47.39768399, 8.54564155),
        create_target_location(47.39769035, 8.54566569),
        create_target_location(47.39770759, 8.54568983),
        create_target_location(47.39772757, 8.54569922),
        create_target_location(47.39774481, 8.54570727),
        create_target_location(47.39776025, 8.54572202),
        create_target_location(47.39778567, 8.54572336),
        create_target_location(47.39780291, 8.54572202),
        create_target_location(47.39782107, 8.54571263),
        create_target_location(47.39783469, 8.54569788),
        create_target_location(47.39783832, 8.54568179),
        create_target_location(47.39784104, 8.54566569),
        create_target_location(47.39784376, 8.54564424),
        create_target_location(47.39772938, 8.54552488),
        create_target_location(47.39782475, 8.54559193),
        create_target_location(47.39780291, 8.54557048),
        create_target_location(47.39771304, 8.54554231),
        create_target_location(47.39780836, 8.54552756),
        create_target_location(47.39973737, 8.54269845),
        create_target_location(47.39973730, 8.54269780),
        create_target_location(47.39779838, 8.54555174),
        create_target_location(47.39778748, 8.54554499),
        create_target_location(47.39777659, 8.54553561),
        create_target_location(47.39776569, 8.54553292),
        create_target_location(47.39774663, 8.54552622),
        create_target_location(47.39772938, 8.54552488),
        create_target_location(47.39771304, 8.54554231),
        create_target_location(47.39770578, 8.54557445),
        create_target_location(47.39770487, 8.54559596),
        create_target_location(47.39770578, 8.54561741),
        create_target_location(47.39770669, 8.54563887),
        create_target_location(47.39771486, 8.54565765),
        create_target_location(47.39773029, 8.54567642),
        create_target_location(47.39775026, 8.54568447),
        create_target_location(47.39776751, 8.54569118),
        create_target_location(47.39778203, 8.54569118),
        create_target_location(47.39779838, 8.54568447),
        create_target_location(47.39781835, 8.54566972),
        create_target_location(47.39782107, 8.54564692),
        create_target_location(47.39782474, 8.54561876),
        create_target_location(47.39782474, 8.54556511),
        create_target_location(47.39782107, 8.54553427),
        create_target_location(47.39779656, 8.54551549),
        create_target_location(47.39777568, 8.54550342),
        create_target_location(47.39775482, 8.54549671),
        create_target_location(47.39773755, 8.54549671),
        create_target_location(47.39771849, 8.54550208),
        create_target_location(47.39770396, 8.54551415),
        create_target_location(47.39769398, 8.54554097),
        create_target_location(47.39768762, 8.54556243),
        create_target_location(47.39768672, 8.54557852),
        create_target_location(47.39768493, 8.54559998),
        create_target_location(47.39768399, 8.54562278),
        create_target_location(47.39768399, 8.54564155),
        create_target_location(47.39769035, 8.54566569),
        create_target_location(47.39770759, 8.54568983),
        create_target_location(47.39772757, 8.54569922),
        create_target_location(47.39774481, 8.54570727),
        create_target_location(47.39776025, 8.54572202),
        create_target_location(47.39778567, 8.54572336),
        create_target_location(47.39780291, 8.54572202),
        create_target_location(47.39782107, 8.54571263),
        create_target_location(47.39783469, 8.54569788),
        create_target_location(47.39783832, 8.54568179),
        create_target_location(47.39784104, 8.54566569),
        create_target_location(47.39784376, 8.54564424),
        create_target_location(47.39784386, 8.54564435),
        create_target_location(47.39784396, 8.54564444),
        create_target_location(47.39784386, 8.54564454),
        create_target_location(47.39784346, 8.54564464),
        create_target_location(47.39784336, 8.54564424),
        create_target_location(47.39772757, 8.54569922),
        create_target_location(47.39774481, 8.54570727),
        create_target_location(47.39776025, 8.54572202),
        create_target_location(47.39778567, 8.54572336),
        create_target_location(47.39770396, 8.54551415),
        create_target_location(47.39769398, 8.54554097),
        create_target_location(47.39768762, 8.54556243),
        create_target_location(47.39768672, 8.54557852),
        create_target_location(47.39768494, 8.54559998),
        create_target_location(47.39779454, 8.54559464),
        create_target_location(47.39780291, 8.54557048),
        create_target_location(47.39779838, 8.54555173),
        create_target_location(47.39778748, 8.54554499),
        create_target_location(47.39777659, 8.54553561),
        create_target_location(47.39776569, 8.54553292),
        create_target_location(47.39774663, 8.54552622),
        create_target_location(47.39771304, 8.54554231),
        create_target_location(47.39772938, 8.54552488),
        create_target_location(47.39771304, 8.54554231),
        create_target_location(47.39770578, 8.54557445),
        create_target_location(47.39770487, 8.54559596),
        create_target_location(47.39770578, 8.54561741),
        create_target_location(47.39770669, 8.54563887),
        create_target_location(47.39771486, 8.54565765),
        create_target_location(47.39773029, 8.54567642),
        create_target_location(47.39775026, 8.54568447),
        create_target_location(47.39776751, 8.54569118),
        create_target_location(47.39784346, 8.54564464),
        create_target_location(47.39776569, 8.54553292)};

    // We're limiting to N_LOCATIONS for testing.
    count = (count > N_LOCATIONS) ? N_LOCATIONS : count;
    std::cout << "# of Target locations: " << count << " @ Rate: " << rate << "Hz." << '\n';

    for (auto pos : spiral_path) {
        if (count-- == 0) {
            return;
        }
        follow_me->set_target_location(pos);
        auto sleep_duration_ms = static_cast<int>(1 / rate * 1000);
        sleep_for(milliseconds(sleep_duration_ms));
    }
}
