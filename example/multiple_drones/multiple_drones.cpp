//
// Example to connect multiple vehicles and make them take off and land in parallel.
//./multiple_drones udp://:14540 udp://:14541
//
// Author: Julian Oes <julian@oes.ch>
// Author: Shayaan Haider (via Slack)

#include <dronecode_sdk/dronecode_sdk.h>
#include <dronecode_sdk/plugins/action/action.h>
#include <dronecode_sdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

using namespace dronecode_sdk;
using namespace std::this_thread;
using namespace std::chrono;

static void takeoff_land(System &system);

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

int main(int argc,
         char **argv) // The input looks like this ./multiple_drones udp://:14540 udp://:14541
{
    if (argc == 1) {
        std::cout << ERROR_CONSOLE_TEXT << "Please specify connection" << NORMAL_CONSOLE_TEXT
                  << std::endl;
        ;
        return 1;
    }

    DronecodeSDK dc;

    // the loop below adds the number of ports the sdk monitors.
    for (int i = 1; i < argc; ++i) {
        ConnectionResult connection_result = dc.add_any_connection(argv[i]);
        if (connection_result != ConnectionResult::SUCCESS) {
            std::cout << ERROR_CONSOLE_TEXT
                      << "Connection error: " << connection_result_str(connection_result)
                      << NORMAL_CONSOLE_TEXT << std::endl;
            ;
            return 1;
        }
    }

    bool discovered_system = false;

    std::cout << "Waiting to discover system..." << std::endl;
    dc.register_on_discover([&discovered_system](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        discovered_system = true;
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a system after around 2
    // seconds.
    sleep_for(seconds(2));

    if (!discovered_system) {
        std::cout << ERROR_CONSOLE_TEXT << "No system found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    std::vector<std::thread> threads;

    for (auto uuid : dc.system_uuids()) {
        System &system = dc.system(uuid);
        std::thread t(&takeoff_land, std::ref(system));
        threads.push_back(std::move(t));
    }

    for (auto &t : threads) {
        t.join();
    }
    return 0;
}

void takeoff_land(System &system)
{
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);

    // We want to listen to the altitude of the drone at 1 Hz.
    const Telemetry::Result set_rate_result = telemetry->set_rate_position(1.0);

    if (set_rate_result != Telemetry::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT
                  << "Setting rate failed:" << Telemetry::result_str(set_rate_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return;
    }

    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry->position_async([](Telemetry::Position position) {
        std::cout << TELEMETRY_CONSOLE_TEXT // set to blue
                  << "Altitude: " << position.relative_altitude_m << " m"
                  << NORMAL_CONSOLE_TEXT // set to default color again
                  << std::endl;
    });

    // Check if vehicle is ready to arm
    while (telemetry->health_all_ok() != true) {
        std::cout << "Vehicle is getting ready to arm" << std::endl;
        sleep_for(seconds(1));
    }

    // Arm vehicle
    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = action->arm();

    if (arm_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Arming failed:" << Action::result_str(arm_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        // return ;
    }

    // Take off
    std::cout << "Taking off..." << std::endl;
    const Action::Result takeoff_result = action->takeoff();
    if (takeoff_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Takeoff failed:" << Action::result_str(takeoff_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        // return ;
    }

    // Let it hover for a bit before landing again.
    sleep_for(seconds(20));

    std::cout << "Landing..." << std::endl;
    const Action::Result land_result = action->land();
    if (land_result != Action::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Land failed:" << Action::result_str(land_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        // return ;
    }

    // Check if vehicle is still in air
    while (telemetry->in_air()) {
        std::cout << "Vehicle is landing..." << std::endl;
        sleep_for(seconds(1));
    }
    std::cout << "Landed!" << std::endl;

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.

    sleep_for(seconds(5));
    std::cout << "Finished..." << std::endl;
    return;
}
