/**
* @file follow_target.cpp
* @brief Example that switches Vehicle mode to FollowMe.
* @author Author: Shakthi Prashanth M <shakthi.prashanth.m@intel.com>
* @date 2017-10-27
*/

#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <dronecore/dronecore.h>

using namespace dronecore;

using std::this_thread::sleep_for;
using std::chrono::seconds;

#define ERROR_CONSOLE_TEXT "\033[31m" //Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" //Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m"  //Restore normal console colour

// Handles Action's result
inline void action_error_exit(Action::Result result, const std::string &message)
{
    if (result != Action::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << ": " << Action::result_str(
                      result) << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Handles FollowMe's result
inline void followme_error_exit(FollowMe::Result result, const std::string &message)
{
    if (result != FollowMe::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << ": " << FollowMe::result_str(
                      result) << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Handles connection result
inline void connection_error_exit(DroneCore::ConnectionResult result, const std::string &message)
{
    if (result != DroneCore::ConnectionResult::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << ": "
                  << DroneCore::connection_result_str(result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

int main(int, char **)
{
    DroneCore dc;

    // add udp connection
    DroneCore::ConnectionResult conn_result = dc.add_udp_connection();
    connection_error_exit(conn_result, "Connection failed");

    // Wait for the device to connect via heartbeat
    while (!dc.is_connected()) {
        std::cout << "Wait for device to connect via heartbeat" << std::endl;
        sleep_for(seconds(1));
    }

    // Device got connected...
    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "Waiting for device to be ready" << std::endl;
        sleep_for(seconds(1));
    }
    std::cout << "Device is ready" << std::endl;

    // Arm
    Action::Result arm_result = device.action().arm();
    action_error_exit(arm_result, "Arming failed");
    std::cout << "Armed" << std::endl;

    // Takeoff
    Action::Result takeoff_result = device.action().takeoff();
    action_error_exit(takeoff_result, "Takeoff failed");
    std::cout << "In Air..." << std::endl;
    sleep_for(seconds(5));

    // Start FollowMe mode (by passing optional position)
    FollowMe::Result followme_result = device.followme().start();
    followme_error_exit(followme_result, "Failed to start FollowMe mode");
    std::cout << "[FollowMe] Waiting for 20 secs :-) ..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(20));

    // Stop FollowMe mode
    followme_result = device.followme().stop();
    followme_error_exit(followme_result, "Failed to stop FollowMe mode");
    std::cout << "Exiting." << std::endl;

    return 0;
}
