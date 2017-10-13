//
// Example that demonstrates Offboard velocity control using LOCAL_NED & BODY_NED
//
// Author: Julian Oes <julian@oes.ch>, Shakthi Prashanth <shakthi.prashanth.m@intel.com>
#include <iostream>
#include <cmath>
#include <thread>
#include <chrono>
#include <dronecore/dronecore.h>

using namespace dronecore;

#define ERROR_CONSOLE_TEXT "\033[31m" //Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" //Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m"  //Restore normal console colour

void wait_msec(int msec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

void wait_sec(int sec)
{
    std::this_thread::sleep_for(std::chrono::seconds(sec));
}

int main(int argc, char **argv)
{
    DroneCore dc;
    std::string offb_mode = (argc > 1) ? argv[1] : "";

    if (argc == 1 || (offb_mode != "local-ned" && offb_mode != "body-ned")) {
        std::cerr << "Usage: offboard <coordinate-frame>\n" <<
                  "coordinate-frame: \"local-ned\", or \"body-ned\""
                  << std::endl;
        return EXIT_FAILURE;
    }

    // add udp connection
    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    if (ret != DroneCore::ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Connection failed: "
                  << DroneCore::connection_result_str(ret)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return EXIT_FAILURE;
    }

    // Wait for device to connect via heartbeat.
    wait_sec(2);
    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        wait_sec(1);
    }

    Action::Result arm_result = device.action().arm();
    if (arm_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Arming failed:" << Action::result_str(
                      arm_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return EXIT_FAILURE;
    }

    Action::Result takeoff_result = device.action().takeoff();
    if (takeoff_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Takeoff failed:" << Action::result_str(
                      takeoff_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "In Air" << std::endl;
    wait_sec(5);

    //////////////////////////////////////////////////////////////////////////////
    if (offb_mode == "local-ned") {
        // Send it once before starting offboard, otherwise it will be rejected.
        device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f});
        std::cout << "[" << offb_mode << "]" << " sent Null velocity command once before starting OFFBOARD"
                  << std::endl;

        Offboard::Result offboard_result = device.offboard().start();
        if (offboard_result != Offboard::Result::SUCCESS) {
            std::cout << ERROR_CONSOLE_TEXT << "Offboard start failed:" << Offboard::result_str(
                          offboard_result) << NORMAL_CONSOLE_TEXT << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "[" << offb_mode << "] OFFBOARD started" << std::endl;

        // Let yaw settle.
        std::cout << "[" << offb_mode << "] Let yaw settle..." << std::endl;
        for (unsigned i = 0; i < 100; ++i) {
            device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 90.0f});
            wait_msec(10);
        }
        std::cout << "[" << offb_mode << "] Done..." << std::endl;
        wait_sec(5);

        {
            const float step_size = 0.01f;
            const float one_cycle = 2.0f * (float)M_PI;
            const unsigned steps = (unsigned)(one_cycle / step_size);

            std::cout << "[" << offb_mode << "]" << " Go right & oscilate" << std::endl;
            for (unsigned i = 0; i < steps; ++i) {
                float vx = 5.0f * sinf(i * step_size);
                device.offboard().set_velocity_ned({vx, 0.0f, 0.0f, 90.0f});
                wait_msec(10);
            }
        }
        std::cout << "[" << offb_mode << "] Done..." << std::endl;
        wait_sec(2);

        std::cout << "[" << offb_mode << "]" << " Turn clock-wise 270 deg" << std::endl;
        for (unsigned i = 0; i < 400; ++i) {
            device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 270.0f});
            wait_msec(10);
        }
        std::cout << "[" << offb_mode << "] Done..." << std::endl;
        wait_sec(2);

        std::cout << "[" << offb_mode << "]" << " Go UP 2 m/s, Turn clock-wise 180 deg" << std::endl;
        for (unsigned i = 0; i < 400; ++i) {
            device.offboard().set_velocity_ned({0.0f, 0.0f, -2.0f, 180.0f});
            wait_msec(10);
        }
        std::cout << "[" << offb_mode << "] Done..." << std::endl;
        wait_sec(2);

        std::cout << "[" << offb_mode << "]" << " Turn clock-wise 90 deg" << std::endl;
        for (unsigned i = 0; i < 400; ++i) {
            device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 90.0f});
            wait_msec(10);
        }
        std::cout << "[" << offb_mode << "] Done..." << std::endl;
        wait_sec(2);

        std::cout << "[" << offb_mode << "]" << " Go DOWN 1.0 m/s" << std::endl;
        for (unsigned i = 0; i < 400; ++i) {
            device.offboard().set_velocity_ned({0.0f, 0.0f, 1.0f, 0.0f});
            wait_msec(10);
        }
        std::cout << "[" << offb_mode << "] Done..." << std::endl;
        wait_sec(2);

    } else if (offb_mode == "body-ned") {

        device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
        std::cout << "[" << offb_mode << "]" << " sent once before starting OFFBOARD" << std::endl;

        Offboard::Result offboard_result = device.offboard().start();
        if (offboard_result != Offboard::Result::SUCCESS) {
            std::cout << ERROR_CONSOLE_TEXT << "Offboard start failed:" << Offboard::result_str(
                          offboard_result) << NORMAL_CONSOLE_TEXT << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "[" << offb_mode << "] OFFBOARD started" << std::endl;

        // Turn around yaw and climb
        std::cout << "[" << offb_mode << "] Turn around yaw & climb" << std::endl;
        for (unsigned i = 0; i < 200; ++i) {
            device.offboard().set_velocity_body({0.0f, 0.0f, -1.0f, 60.0f});
            wait_msec(10);
        }

        // Turn back
        std::cout << "[" << offb_mode << "] Turn back" << std::endl;
        for (unsigned i = 0; i < 200; ++i) {
            device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, -60.0f});
            wait_msec(10);
        }

        // Wait for a bit
        std::cout << "[" << offb_mode << "] Wait for a bit" << std::endl;
        for (unsigned i = 0; i < 200; ++i) {
            device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
            wait_msec(10);
        }

        // Fly a circle
        std::cout << "[" << offb_mode << "] Fly a circle" << std::endl;
        for (unsigned i = 0; i < 500; ++i) {
            device.offboard().set_velocity_body({5.0f, 0.0f, 0.0f, 60.0f});
            wait_msec(10);
        }

        // Wait for a bit
        std::cout << "[" << offb_mode << "] Wait for a bit" << std::endl;
        for (unsigned i = 0; i < 500; ++i) {
            device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
            wait_msec(10);
        }

        // Fly a circle sideways
        std::cout << "[" << offb_mode << "] Fly a circle sideways..." << std::endl;
        for (unsigned i = 0; i < 500; ++i) {
            device.offboard().set_velocity_body({0.0f, -5.0f, 0.0f, 60.0f});
            wait_msec(10);
        }

        // Wait for a bit
        std::cout << "[" << offb_mode << "] Wait for a bit" << std::endl;
        for (unsigned i = 0; i < 500; ++i) {
            device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
            wait_msec(10);
        }
    }
    //////////////////////////////////////////////////////////////////////////////

    Offboard::Result offboard_result = device.offboard().stop();
    if (offboard_result != Offboard::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Offboard stop failed:" << Offboard::result_str(
                      offboard_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[" << offb_mode << "] OFFBOARD stopped" << std::endl;

    const Action::Result land_result = device.action().land();
    if (land_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Land failed:" << Action::result_str(
                      land_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "[" << offb_mode << "] Landed" << std::endl;

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.
    wait_sec(10);

    return EXIT_SUCCESS;
}
