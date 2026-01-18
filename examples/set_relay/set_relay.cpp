//
// Simple example to demonstrate how to control relays using MAVSDK.
//
// For ArduPilot, set
//   RELAY1_FUNCTION 1
// Then reboot
//   param set RELAY1_PIN 101 // MainOut1
//   param set SERVO1_FUNCTION -1
// Equivalent to in mavproxy "relay set 0 0" and "relay set 0 1"

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cerr
        << "Usage : " << bin_name << " <connection_url> <index> <setting>\n"
        << "Connection URL format should be :\n"
        << " For TCP server: tcpin://<our_ip>:<port>\n"
        << " For TCP client: tcpout://<remote_ip>:<port>\n"
        << " For UDP server: udp://<our_ip>:<port>\n"
        << " For UDP client: udp://<remote_ip>:<port>\n"
        << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
        << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n"
        << "Index is the relay instance number, 0 or great.\n"
        << "Setting is the relay value. 1=on, 0=off, others possible depending on system hardware.\n";
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        usage(argv[0]);
        return 1;
    }

    const std::string connection_url = argv[1];
    const int index = std::stod(argv[2]);
    const int setting = std::stod(argv[3]);

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    const ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    std::cout << "Waiting to discover system...\n";
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    // We wait for new systems to be discovered, once we find one that has an
    // autopilot, we decide to use it.
    Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
        auto system = mavsdk.systems().back();

        if (system->has_autopilot()) {
            std::cout << "Discovered autopilot\n";

            // Unsubscribe again as we only want to find one system.
            mavsdk.unsubscribe_on_new_system(handle);
            prom.set_value(system);
        }
    });

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

    std::cout << "Setting relay " << index << " to " << setting << "...\n";
    const Action::Result set_relay_result = action.set_relay(index, setting);

    if (set_relay_result != Action::Result::Success) {
        std::cerr << "Setting relay failed:" << set_relay_result << '\n';
        return 1;
    }

    return 0;
}
