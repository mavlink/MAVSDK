//
// This is a simple example how you can tell an autopilot to do a flight
// termination. Note that there might be additional protections like
// parameters (for PX4 called circuit breakers) against such a command,
// so before using this, make sure to test it on the bench.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;

static void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    const std::string connection_url = argv[1];

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    std::cout << "Waiting to discover system...\n";
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    // We wait for new systems to be discovered, once we find one that has an
    // autopilot, we decide to use it.
    Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
        auto system = mavsdk.systems().back();

        std::cout << "Found " << mavsdk.systems().size() << " systems\n";

        if (system->has_autopilot()) {
            std::cout << "Discovered autopilot\n";

            // Unsubscribe again as we only want to find one system.
            mavsdk.unsubscribe_on_new_system(handle);
            prom.set_value(system);
        }
    });

    const ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds max, surely.
    if (fut.wait_for(std::chrono::seconds(3)) == std::future_status::timeout) {
        std::cerr << "No autopilot found, exiting.\n";
        return 1;
    }

    // Get discovered system now.
    auto system = fut.get();

    // Instantiate plugins.
    auto action = Action{system};

    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "WARNING: termination will:\n"
                 "- stop all motors in air, \n"
                 "- set servos to failsafe position,\n"
                 "- and deploy the parachute if available\n"
                 "\n";

    std::cout << "Are you sure to terminate? [y/N]\n";
    char type;
    std::cin >> type;

    if (type != 'y' && type != 'Y') {
        std::cout << "Not terminating, exciting.\n";
        return 0;
    }

    // Sending terminate command.
    std::cout << "Sending termination\n";
    action.terminate();

    return 0;
}
