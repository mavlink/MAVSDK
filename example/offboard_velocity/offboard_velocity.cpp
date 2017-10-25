/**
* @file offboard_velocity.cpp
* @brief Example that demonstrates offboard velocity control in local NED and body coordinates
*
* @authors Author: Julian Oes <julian@oes.ch>,
*                  Shakthi Prashanth <shakthi.prashanth.m@intel.com>
* @date 2017-10-17
*/

#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <dronecore/dronecore.h>

using namespace dronecore;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::chrono::seconds;

#define ERROR_CONSOLE_TEXT "\033[31m" //Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" //Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m"  //Restore normal console colour

// Handles Action's result
inline void action_error_exit(Action::Result result, const std::string &message)
{
    if (result != Action::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << Action::result_str(
                      result) << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Handles Offboard's result
inline void offboard_error_exit(Offboard::Result result, const std::string &message)
{
    if (result != Offboard::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << Offboard::result_str(
                      result) << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Handles connection result
inline void connection_error_exit(DroneCore::ConnectionResult result, const std::string &message)
{
    if (result != DroneCore::ConnectionResult::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message
                  << DroneCore::connection_result_str(result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Logs during Offboard control
inline void offboard_log(const std::string &offb_mode, const std::string msg)
{
    std::cout << "[" << offb_mode << "]" << msg << std::endl;
}

/**
* @name connected device
* @brief Does Offboard control using NED co-ordinates
* returns true if everything went well in Offboard control, exits with a log otherwise.
**/
bool offb_ctrl_ned(Device &device)
{
    const std::string offb_mode = "NED";
    // Send it once before starting offboard, otherwise it will be rejected.
    device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f});
    offboard_log(offb_mode, " Sent Null velocity command once before starting OFFBOARD");

    Offboard::Result offboard_result = device.offboard().start();
    offboard_error_exit(offboard_result, "Offboard start failed");
    offboard_log(offb_mode, " OFFBOARD started");
    sleep_for(seconds(1));

    // Let yaw settle.
    offboard_log(offb_mode, " Let yaw settle...");
    device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 90.0f});
    sleep_for(seconds(1));
    offboard_log(offb_mode, " Done...");

    {
        const float step_size = 0.01f;
        const float one_cycle = 2.0f * (float)M_PI;
        const unsigned steps = (unsigned)(one_cycle / step_size);

        offboard_log(offb_mode,  " Go right & oscillate");
        for (unsigned i = 0; i < steps; ++i) {
            float vx = 5.0f * sinf(i * step_size);
            device.offboard().set_velocity_ned({vx, 0.0f, 0.0f, 90.0f});
            sleep_for(milliseconds(10));
        }
    }
    offboard_log(offb_mode, " Done...");
    // NOTE: Use sleep_for() after each velocity-ned command to closely monitor their behaviour.

    offboard_log(offb_mode,  " Turn clock-wise 270 deg");
    device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 270.0f});
    sleep_for(seconds(2));

    offboard_log(offb_mode, " Done");

    offboard_log(offb_mode, " Go UP 2 m/s, Turn clock-wise 180 deg");
    device.offboard().set_velocity_ned({0.0f, 0.0f, -2.0f, 180.0f});
    sleep_for(seconds(4));
    offboard_log(offb_mode, " Done...");

    offboard_log(offb_mode,  " Turn clock-wise 90 deg");
    device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 90.0f});
    sleep_for(seconds(4));
    offboard_log(offb_mode, " Done...");

    offboard_log(offb_mode,  " Go DOWN 1.0 m/s");
    device.offboard().set_velocity_ned({0.0f, 0.0f, 1.0f, 0.0f});
    sleep_for(seconds(4));
    offboard_log(offb_mode, " Done...");

    // Now, stop offboard mode.
    offboard_result = device.offboard().stop();
    offboard_error_exit(offboard_result, "Offboard stop failed: ");
    offboard_log(offb_mode, " OFFBOARD stopped");

    return true;
}

/**
* @name connected device
* @brief Does Offboard control using BODY co-ordinates
* returns true if everything went well in Offboard control, exits with a log otherwise.
*/
bool offb_ctrl_body(Device &device)
{
    const std::string offb_mode = "BODY";

    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    offboard_log(offb_mode,  " Sent once before starting OFFBOARD");

    Offboard::Result offboard_result = device.offboard().start();
    offboard_error_exit(offboard_result, "Offboard start failed: ");
    offboard_log(offb_mode, " OFFBOARD started");

    // Turn around yaw and climb
    offboard_log(offb_mode, " Turn around yaw & climb");
    device.offboard().set_velocity_body({0.0f, 0.0f, -1.0f, 60.0f});
    sleep_for(seconds(2));

    // Turn back
    offboard_log(offb_mode, " Turn back");
    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, -60.0f});
    sleep_for(seconds(2));

    // Wait for a bit
    offboard_log(offb_mode, " Wait for a bit");
    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    sleep_for(seconds(2));
    // NOTE: Use sleep_for() after each velocity-ned command to closely monitor their behaviour.

    // Fly a circle
    offboard_log(offb_mode, " Fly a circle");
    device.offboard().set_velocity_body({5.0f, 0.0f, 0.0f, 60.0f});
    sleep_for(seconds(5));

    // Wait for a bit
    offboard_log(offb_mode, " Wait for a bit");
    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    sleep_for(seconds(5));

    // Fly a circle sideways
    offboard_log(offb_mode, " Fly a circle sideways...");
    device.offboard().set_velocity_body({0.0f, -5.0f, 0.0f, 60.0f});
    sleep_for(seconds(5));

    // Wait for a bit
    offboard_log(offb_mode, " Wait for a bit");
    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    sleep_for(seconds(5));

    // Now, stop offboard mode.
    offboard_result = device.offboard().stop();
    offboard_error_exit(offboard_result, "Offboard stop failed: ");
    offboard_log(offb_mode, " OFFBOARD stopped");

    return true;
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

    //  using LOCAL NED co-ordinates
    bool ret = offb_ctrl_ned(device);
    if (ret == false) {
        return EXIT_FAILURE;
    }
    std::cout << "---------------------------" << std::endl;

    //  using BODY NED co-ordinates
    ret = offb_ctrl_body(device);
    if (ret == false) {
        return EXIT_FAILURE;
    }

    const Action::Result land_result = device.action().land();
    action_error_exit(land_result, "Landing failed");

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.
    sleep_for(seconds(10));
    std::cout << "Landed" << std::endl;

    return EXIT_SUCCESS;
}
