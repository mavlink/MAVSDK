#include <dronecore/dronecore.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdint>

using namespace dronecore;
using namespace std::placeholders;

#define ERROR_CONSOLE_TEXT "\033[31m" //Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" //Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m"  //Restore normal console colour

int main(int /*argc*/, char ** /*argv*/)
{
    DroneCore dc;

    bool discovered_device = false;

    DroneCore::ConnectionResult connection_result = dc.add_udp_connection();

    if (connection_result != DroneCore::ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Connection failed: "
                  << DroneCore::connection_result_str(connection_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    std::cout << "Waiting to discover device..." << std::endl;
    dc.register_on_discover([&discovered_device](uint64_t uuid) {
        std::cout << "Discovered device with UUID: " << uuid << std::endl;
        discovered_device = true;
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a device after around 2 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (!discovered_device) {
        std::cout << ERROR_CONSOLE_TEXT << "No device found, exiting." << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // We don't need to specify the UUID if it's only one device anyway.
    // If there were multiple, we could specify it with:
    // dc.device(uint64_t uuid);
    Device &device = dc.device();

    // Check if vehicle is ready to arm
    if (device.telemetry().health_all_ok() != true) {
        std::cout << ERROR_CONSOLE_TEXT << "Vehicle not ready to arm" << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Arm vehicle
    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = device.action().arm();

    if (arm_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Arming failed:" << Action::result_str(
                      arm_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    device.telemetry().flight_mode_async(
    std::bind([&](Telemetry::FlightMode flight_mode) {
        auto last_location = device.follow_me().get_last_location();

        std::cout << "[FlightMode: " << Telemetry::flight_mode_str(flight_mode)
                  << "] Vehicle is at Lat: " << last_location.latitude_deg << " deg, "  <<
                  "Lon: " << last_location.longitude_deg << " deg." << std::endl;
    }, _1));


    // Take off
    std::cout << "Taking off..." << std::endl;
    const Action::Result takeoff_result = device.action().takeoff();
    if (takeoff_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Takeoff failed:" << Action::result_str(
                      takeoff_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Start Follow Me
    device.follow_me().set_target_location({47.39768399, 8.54564155, 0.0, 0.f, 0.f, 0.f});
    FollowMe::Result follow_me_result = device.follow_me().start();
    if (follow_me_result != FollowMe::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "failed to initiate follow me mode" << std::endl;

    }
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Landing..." << std::endl;
    const Action::Result land_result = device.action().land();
    if (land_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Land failed:" << Action::result_str(
                      land_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Finished..." << std::endl;
    return 0;
}
