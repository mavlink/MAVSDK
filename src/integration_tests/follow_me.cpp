#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <array>
#include "integration_test_helper.h"
#include "global_include.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/follow_me/follow_me.h"

using namespace mavsdk;
using namespace std::chrono;
using namespace std::this_thread;

void print(const FollowMe::Config& config);
void send_location_updates(
    std::shared_ptr<FollowMe> follow_me,
    const Telemetry::Position& home,
    size_t count = 25ul,
    float rate = 2.f);

const size_t N_LOCATIONS = 100ul;

TEST_F(SitlTest, FollowMeOneLocation)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ConnectionResult::Success, ret);

    // Wait for system to connect via heartbeat.
    sleep_for(seconds(2));
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto follow_me = std::make_shared<FollowMe>(system);
    auto action = std::make_shared<Action>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << '\n';
        sleep_for(seconds(1));
    }

    // Get the home position so the Follow-Me locations are set with respect
    // to the home position instead of being hardcoded.
    auto home = telemetry->home();

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
    some_location.latitude_deg = home.latitude_deg + 0.00006671;
    some_location.longitude_deg = home.longitude_deg - 0.00003415;
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

    // Unsubscribe to avoid races on destruction.
    telemetry->subscribe_flight_mode(nullptr);
}

TEST_F(SitlTest, FollowMeMultiLocationWithConfig)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
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

    while (!telemetry->health_all_ok()) {
        std::cout << "Waiting for system to be ready" << '\n';
        sleep_for(seconds(1));
    }

    // Get the home position so the Follow-Me locations are set with respect
    // to the home position instead of being hardcoded.
    auto home = telemetry->home();

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
    config.min_height_m = 12.f; // increase min height
    config.follow_distance_m = 20.f; // set distance b/w system and target during FollowMe mode
    config.responsiveness = 0.2f; // set to higher responsiveness
    config.follow_direction =
        FollowMe::Config::FollowDirection::Front; // System follows target from FRONT side

    // Apply configuration
    FollowMe::Result config_result = follow_me->set_config(config);
    ASSERT_EQ(FollowMe::Result::Success, config_result);

    // Start following
    FollowMe::Result follow_me_result = follow_me->start();
    ASSERT_EQ(FollowMe::Result::Success, follow_me_result);

    // send location update every second
    send_location_updates(follow_me, home);

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

    // Unsubscribe to avoid races on destruction.
    telemetry->subscribe_flight_mode(nullptr);
}

void print(const FollowMe::Config& config)
{
    std::cout << "Current FollowMe configuration of the system" << '\n';
    std::cout << "---------------------------" << '\n';
    std::cout << "Min Height: " << config.min_height_m << "m" << '\n';
    std::cout << "Distance: " << config.follow_distance_m << "m" << '\n';
    std::cout << "Responsiveness: " << config.responsiveness << '\n';
    std::cout << "Following from: " << config.follow_direction << '\n';
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

void send_location_updates(
    std::shared_ptr<FollowMe> follow_me, const Telemetry::Position& home, size_t count, float rate)
{
    // TODO: Generate these co-ordinates from an algorithm
    // Altitude here is ignored by PX4, as we've set min altitude in configuration.
    std::array<FollowMe::TargetLocation, N_LOCATIONS> spiral_path = {
        create_target_location(home.latitude_deg - 0.00006671, home.longitude_deg + 0.00003415),
        create_target_location(home.latitude_deg - 0.00006035, home.longitude_deg + 0.00005829),
        create_target_location(home.latitude_deg - 0.00004311, home.longitude_deg + 0.00008243),
        create_target_location(home.latitude_deg - 0.00002313, home.longitude_deg + 0.00009182),
        create_target_location(home.latitude_deg - 0.00000589, home.longitude_deg + 0.00009987),
        create_target_location(home.latitude_deg + 0.00000955, home.longitude_deg + 0.00011462),
        create_target_location(home.latitude_deg + 0.00003497, home.longitude_deg + 0.00011596),
        create_target_location(home.latitude_deg + 0.00005221, home.longitude_deg + 0.00011462),
        create_target_location(home.latitude_deg + 0.00007037, home.longitude_deg + 0.00010523),
        create_target_location(home.latitude_deg + 0.00008399, home.longitude_deg + 0.00009048),
        create_target_location(home.latitude_deg + 0.00008762, home.longitude_deg + 0.00007439),
        create_target_location(home.latitude_deg + 0.00009034, home.longitude_deg + 0.00005829),
        create_target_location(home.latitude_deg + 0.00009306, home.longitude_deg + 0.00003684),
        create_target_location(home.latitude_deg - 0.00002132, home.longitude_deg - 0.00008252),
        create_target_location(home.latitude_deg + 0.00007405, home.longitude_deg - 0.00001547),
        create_target_location(home.latitude_deg + 0.00005221, home.longitude_deg - 0.00003692),
        create_target_location(home.latitude_deg - 0.00003766, home.longitude_deg - 0.00006509),
        create_target_location(home.latitude_deg + 0.00005766, home.longitude_deg - 0.00007984),
        create_target_location(home.latitude_deg + 0.00198667, home.longitude_deg - 0.00290895),
        create_target_location(home.latitude_deg + 0.00198660, home.longitude_deg - 0.00290960),
        create_target_location(home.latitude_deg + 0.00004768, home.longitude_deg - 0.00005566),
        create_target_location(home.latitude_deg + 0.00003678, home.longitude_deg - 0.00006241),
        create_target_location(home.latitude_deg + 0.00002589, home.longitude_deg - 0.00007179),
        create_target_location(home.latitude_deg + 0.00001499, home.longitude_deg - 0.00007448),
        create_target_location(home.latitude_deg - 0.00000407, home.longitude_deg - 0.00008118),
        create_target_location(home.latitude_deg - 0.00002132, home.longitude_deg - 0.00008252),
        create_target_location(home.latitude_deg - 0.00003766, home.longitude_deg - 0.00006509),
        create_target_location(home.latitude_deg - 0.00004492, home.longitude_deg - 0.00003295),
        create_target_location(home.latitude_deg - 0.00004583, home.longitude_deg - 0.00001144),
        create_target_location(home.latitude_deg - 0.00004492, home.longitude_deg + 0.00001001),
        create_target_location(home.latitude_deg - 0.00004401, home.longitude_deg + 0.00003147),
        create_target_location(home.latitude_deg - 0.00003584, home.longitude_deg + 0.00005025),
        create_target_location(home.latitude_deg - 0.00002041, home.longitude_deg + 0.00006902),
        create_target_location(home.latitude_deg - 0.00000044, home.longitude_deg + 0.00007707),
        create_target_location(home.latitude_deg + 0.00001681, home.longitude_deg + 0.00008378),
        create_target_location(home.latitude_deg + 0.00003133, home.longitude_deg + 0.00008378),
        create_target_location(home.latitude_deg + 0.00004768, home.longitude_deg + 0.00007707),
        create_target_location(home.latitude_deg + 0.00006765, home.longitude_deg + 0.00006232),
        create_target_location(home.latitude_deg + 0.00007037, home.longitude_deg + 0.00003952),
        create_target_location(home.latitude_deg + 0.00007404, home.longitude_deg + 0.00001136),
        create_target_location(home.latitude_deg + 0.00007404, home.longitude_deg - 0.00004229),
        create_target_location(home.latitude_deg + 0.00007037, home.longitude_deg - 0.00007313),
        create_target_location(home.latitude_deg + 0.00004586, home.longitude_deg - 0.00009191),
        create_target_location(home.latitude_deg + 0.00002498, home.longitude_deg - 0.00010398),
        create_target_location(home.latitude_deg + 0.00000412, home.longitude_deg - 0.00011069),
        create_target_location(home.latitude_deg - 0.00001315, home.longitude_deg - 0.00011069),
        create_target_location(home.latitude_deg - 0.00003221, home.longitude_deg - 0.00010532),
        create_target_location(home.latitude_deg - 0.00004674, home.longitude_deg - 0.00009325),
        create_target_location(home.latitude_deg - 0.00005672, home.longitude_deg - 0.00006643),
        create_target_location(home.latitude_deg - 0.00006308, home.longitude_deg - 0.00004497),
        create_target_location(home.latitude_deg - 0.00006398, home.longitude_deg - 0.00002888),
        create_target_location(home.latitude_deg - 0.00006577, home.longitude_deg - 0.00000742),
        create_target_location(home.latitude_deg - 0.00006671, home.longitude_deg + 0.00001538),
        create_target_location(home.latitude_deg - 0.00006671, home.longitude_deg + 0.00003415),
        create_target_location(home.latitude_deg - 0.00006035, home.longitude_deg + 0.00005829),
        create_target_location(home.latitude_deg - 0.00004311, home.longitude_deg + 0.00008243),
        create_target_location(home.latitude_deg - 0.00002313, home.longitude_deg + 0.00009182),
        create_target_location(home.latitude_deg - 0.00000589, home.longitude_deg + 0.00009987),
        create_target_location(home.latitude_deg + 0.00000955, home.longitude_deg + 0.00011462),
        create_target_location(home.latitude_deg + 0.00003497, home.longitude_deg + 0.00011596),
        create_target_location(home.latitude_deg + 0.00005221, home.longitude_deg + 0.00011462),
        create_target_location(home.latitude_deg + 0.00007037, home.longitude_deg + 0.00010523),
        create_target_location(home.latitude_deg + 0.00008399, home.longitude_deg + 0.00009048),
        create_target_location(home.latitude_deg + 0.00008762, home.longitude_deg + 0.00007439),
        create_target_location(home.latitude_deg + 0.00009034, home.longitude_deg + 0.00005829),
        create_target_location(home.latitude_deg + 0.00009306, home.longitude_deg + 0.00003684),
        create_target_location(home.latitude_deg + 0.00009316, home.longitude_deg + 0.00003695),
        create_target_location(home.latitude_deg + 0.00009326, home.longitude_deg + 0.00003704),
        create_target_location(home.latitude_deg + 0.00009316, home.longitude_deg + 0.00003714),
        create_target_location(home.latitude_deg + 0.00009276, home.longitude_deg + 0.00003724),
        create_target_location(home.latitude_deg + 0.00009266, home.longitude_deg + 0.00003684),
        create_target_location(home.latitude_deg - 0.00002313, home.longitude_deg + 0.00009182),
        create_target_location(home.latitude_deg - 0.00000589, home.longitude_deg + 0.00009987),
        create_target_location(home.latitude_deg + 0.00000955, home.longitude_deg + 0.00011462),
        create_target_location(home.latitude_deg + 0.00003497, home.longitude_deg + 0.00011596),
        create_target_location(home.latitude_deg - 0.00004674, home.longitude_deg - 0.00009325),
        create_target_location(home.latitude_deg - 0.00005672, home.longitude_deg - 0.00006643),
        create_target_location(home.latitude_deg - 0.00006308, home.longitude_deg - 0.00004497),
        create_target_location(home.latitude_deg - 0.00006398, home.longitude_deg - 0.00002888),
        create_target_location(home.latitude_deg - 0.00006576, home.longitude_deg - 0.00000742),
        create_target_location(home.latitude_deg + 0.00004384, home.longitude_deg - 0.00001276),
        create_target_location(home.latitude_deg + 0.00005221, home.longitude_deg - 0.00003692),
        create_target_location(home.latitude_deg + 0.00004768, home.longitude_deg - 0.00005567),
        create_target_location(home.latitude_deg + 0.00003678, home.longitude_deg - 0.00006241),
        create_target_location(home.latitude_deg + 0.00002589, home.longitude_deg - 0.00007179),
        create_target_location(home.latitude_deg + 0.00001499, home.longitude_deg - 0.00007448),
        create_target_location(home.latitude_deg - 0.00000407, home.longitude_deg - 0.00008118),
        create_target_location(home.latitude_deg - 0.00003766, home.longitude_deg - 0.00006509),
        create_target_location(home.latitude_deg - 0.00002132, home.longitude_deg - 0.00008252),
        create_target_location(home.latitude_deg - 0.00003766, home.longitude_deg - 0.00006509),
        create_target_location(home.latitude_deg - 0.00004492, home.longitude_deg - 0.00003295),
        create_target_location(home.latitude_deg - 0.00004583, home.longitude_deg - 0.00001144),
        create_target_location(home.latitude_deg - 0.00004492, home.longitude_deg + 0.00001001),
        create_target_location(home.latitude_deg - 0.00004401, home.longitude_deg + 0.00003147),
        create_target_location(home.latitude_deg - 0.00003584, home.longitude_deg + 0.00005025),
        create_target_location(home.latitude_deg - 0.00002041, home.longitude_deg + 0.00006902),
        create_target_location(home.latitude_deg - 0.00000044, home.longitude_deg + 0.00007707),
        create_target_location(home.latitude_deg + 0.00001681, home.longitude_deg + 0.00008378),
        create_target_location(home.latitude_deg + 0.00009276, home.longitude_deg + 0.00003724),
        create_target_location(home.latitude_deg + 0.00001499, home.longitude_deg - 0.00007448)};

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
