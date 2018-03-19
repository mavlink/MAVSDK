/**
* @file follow_me.cpp
*
* @brief Example that demonstrates the usage of Follow Me plugin.
* The example registers with FakeLocationProvider for location updates
* and sends them to the Follow Me plugin which are sent to drone. You can observe
* drone following you. We print last location of the drone in flight mode callback.
*
* @author Shakthi Prashanth <shakthi.prashanth.m@intel.com>
* @date 2018-01-03
*/

#include <chrono>
#include <dronecore/action.h>
#include <dronecore/dronecore.h>
#include <dronecore/follow_me.h>
#include <dronecore/telemetry.h>
#include <iostream>
#include <memory>
#include <thread>

#include "fake_location_provider.h"

using namespace dronecore;
using namespace std::placeholders; // for `_1`
using namespace std::chrono; // for seconds(), milliseconds(), etc
using namespace std::this_thread;  // for sleep_for()

// For coloring output
#define ERROR_CONSOLE_TEXT "\033[31m" //Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" //Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m"  //Restore normal console colour

inline void action_error_exit(ActionResult result, const std::string &message);
inline void follow_me_error_exit(FollowMe::Result result, const std::string &message);
inline void connection_error_exit(ConnectionResult result, const std::string &message);

int main(int, char **)
{
    DroneCore dc;

    ConnectionResult conn_result = dc.add_udp_connection();
    connection_error_exit(conn_result, "Connection failed");

    // Wait for the system to connect via heartbeat
    while (!dc.is_connected()) {
        std::cout << "Wait for system to connect via heartbeat" << std::endl;
        sleep_for(seconds(1));
    }

    // System got discovered.
    System &system = dc.system();
    auto action = std::make_shared<Action>(system);
    auto follow_me = std::make_shared<FollowMe>(system);
    auto telemetry = std::make_shared<Telemetry>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "Waiting for system to be ready" << std::endl;
        sleep_for(seconds(1));
    }
    std::cout << "System is ready" << std::endl;

    // Arm
    ActionResult arm_result = action->arm();
    action_error_exit(arm_result, "Arming failed");
    std::cout << "Armed" << std::endl;

    // Subscribe to receive updates on flight mode. You can find out whether FollowMe is active.
    telemetry->flight_mode_async(
    std::bind([&](Telemetry::FlightMode flight_mode) {
        const FollowMe::TargetLocation last_location = follow_me->get_last_location();
        std::cout << "[FlightMode: " << Telemetry::flight_mode_str(flight_mode)
                  << "] Vehicle is at: " << last_location.latitude_deg << ", "
                  << last_location.longitude_deg << " degrees." << std::endl;
    }, std::placeholders::_1));

    // Takeoff
    ActionResult takeoff_result = action->takeoff();
    action_error_exit(takeoff_result, "Takeoff failed");
    std::cout << "In Air..." << std::endl;
    sleep_for(seconds(5)); // Wait for drone to reach takeoff altitude

    // Configure Min height of the drone to be "20 meters" above home & Follow direction as "Front right".
    FollowMe::Config config;
    config.min_height_m = 20.0;
    config.follow_direction = FollowMe::Config::FollowDirection::FRONT_RIGHT;
    FollowMe::Result follow_me_result = follow_me->set_config(config);

    // Start Follow Me
    follow_me_result = follow_me->start();
    follow_me_error_exit(follow_me_result, "Failed to start FollowMe mode");

    boost::asio::io_service io; // for event loop
    std::unique_ptr<FakeLocationProvider> location_provider(new FakeLocationProvider(io));
    // Register for platform-specific Location provider. We're using FakeLocationProvider for the example.
    location_provider->request_location_updates([&system, &follow_me](double lat, double lon) {
        follow_me->set_target_location({lat, lon, 0.0, 0.f, 0.f, 0.f});
    });
    io.run(); // will run as long as location updates continue to happen.

    // Stop Follow Me
    follow_me_result = follow_me->stop();
    follow_me_error_exit(follow_me_result, "Failed to stop FollowMe mode");

    // Stop flight mode updates.
    telemetry->flight_mode_async(nullptr);

    // Land
    const ActionResult land_result = action->land();
    action_error_exit(land_result, "Landing failed");
    while (telemetry->in_air()) {
        std::cout << "waiting until landed" << std::endl;
        sleep_for(seconds(1));
    }
    std::cout << "Landed..." << std::endl;
    return 0;
}

// Handles Action's result
inline void action_error_exit(ActionResult result, const std::string &message)
{
    if (result != ActionResult::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << action_result_str(
                      result) << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}
// Handles FollowMe's result
inline void follow_me_error_exit(FollowMe::Result result, const std::string &message)
{
    if (result != FollowMe::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << FollowMe::result_str(
                      result) << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}
// Handles connection result
inline void connection_error_exit(ConnectionResult result, const std::string &message)
{
    if (result != ConnectionResult::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message
                  << connection_result_str(result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}
