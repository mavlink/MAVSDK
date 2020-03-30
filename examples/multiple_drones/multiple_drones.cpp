//
// Example to connect multiple vehicles and make them take off and land in parallel.
//./multiple_drones udp://:14540 udp://:14541
//
// Author: Julian Oes <julian@oes.ch>
// Author: Shayaan Haider (via Slack)

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <cstdint>
#include <atomic>
#include <iostream>
#include <thread>
#include <chrono>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

static void takeoff_and_land(System& system);

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

int main(int argc, char* argv[])
{
    if (argc == 1) {
        std::cerr << ERROR_CONSOLE_TEXT << "Please specify connection" << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    Mavsdk dc;

    int total_udp_ports = argc - 1;

    // the loop below adds the number of ports the sdk monitors.
    for (int i = 1; i < argc; ++i) {
        ConnectionResult connection_result = dc.add_any_connection(argv[i]);
        if (connection_result != ConnectionResult::SUCCESS) {
            std::cerr << ERROR_CONSOLE_TEXT
                      << "Connection error: " << connection_result_str(connection_result)
                      << NORMAL_CONSOLE_TEXT << std::endl;
            return 1;
        }
    }

    std::atomic<signed> num_systems_discovered{0};

    std::cout << "Waiting to discover system..." << std::endl;
    dc.register_on_discover([&num_systems_discovered](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        ++num_systems_discovered;
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a system after around 2
    // seconds.
    sleep_for(seconds(2));

    if (num_systems_discovered != total_udp_ports) {
        std::cerr << ERROR_CONSOLE_TEXT << "Not all systems found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    std::vector<std::thread> threads;

    for (auto uuid : dc.system_uuids()) {
        System& system = dc.system(uuid);
        std::thread t(&takeoff_and_land, std::ref(system));
        threads.push_back(std::move(t));
    }

    for (auto& t : threads) {
        t.join();
    }
    return 0;
}

void takeoff_and_land(System& system)
{
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);

    // We want to listen to the altitude of the drone at 1 Hz.
    const Telemetry::Result set_rate_result = telemetry->set_rate_position(1.0);

    if (set_rate_result != Telemetry::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT
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

    if (arm_result != Action::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << "Arming failed:" << Action::result_str(arm_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
    }

    // Take off
    std::cout << "Taking off..." << std::endl;
    const Action::Result takeoff_result = action->takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << "Takeoff failed:" << Action::result_str(takeoff_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
    }

    // Let it hover for a bit before landing again.
    sleep_for(seconds(20));

    std::cout << "Landing..." << std::endl;
    const Action::Result land_result = action->land();
    if (land_result != Action::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << "Land failed:" << Action::result_str(land_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
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
