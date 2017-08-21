//
// Simple example to demonstrate how to use DroneCore.
//
// Author: Julian Oes <julian@oes.ch>

#include <dronecore/dronecore.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdint>

using namespace dronecore;

int main(int /*argc*/, char ** /*argv*/)
{
    DroneCore dc;

    bool discovered_device = false;

    DroneCore::ConnectionResult connection_result = dc.add_udp_connection();

    if (connection_result != DroneCore::ConnectionResult::SUCCESS) {
        std::cout << "Connection failed: " << DroneCore::connection_result_str(
                      connection_result) << std::endl;
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
        std::cout << "No device found, exiting." << std::endl;
        return 1;
    }

    // We don't need to specify the UUID if it's only one device anyway.
    // If there were multiple, we could specify it with:
    // dc.device(uint64_t uuid);
    Device &device = dc.device();

    // We want to listen to the altitude of the drone at 1 Hz.
    const Telemetry::Result set_rate_result = dc.device().telemetry().set_rate_position(1.0);
    if (set_rate_result != Telemetry::Result::SUCCESS) {
        std::cout << "Setting rate failed:" << Telemetry::result_str(set_rate_result) << std::endl;
        return 1;
    }

    device.telemetry().position_async([](Telemetry::Position position) {
        std::cout << "\033[34m" // set to blue
                  << "Altitude: " << position.relative_altitude_m << " m"
                  << "\033[0m" // set to default color again
                  << std::endl;
    });

    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = device.action().arm();

    if (arm_result != Action::Result::SUCCESS) {
        std::cout << "Arming failed:" << Action::result_str(arm_result) << std::endl;
        return 1;
    }

    std::cout << "Taking off..." << std::endl;
    const Action::Result takeoff_result = device.action().takeoff();
    if (takeoff_result != Action::Result::SUCCESS) {
        std::cout << "Takeoff failed:" << Action::result_str(takeoff_result) << std::endl;
        return 1;
    }

    // Let it hover for a bit before landing again.
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Landing..." << std::endl;
    const Action::Result land_result = device.action().land();
    if (land_result != Action::Result::SUCCESS) {
        std::cout << "Land failed:" << Action::result_str(land_result) << std::endl;
        return 1;
    }

    // We are relying on auto-disarming but let's keep watching the telemetry infos a bit longer.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}
