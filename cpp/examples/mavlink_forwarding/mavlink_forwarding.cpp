//
// Simple example to demonstrate how to forward MAVLink traffic
// sent to UDP port 12550 to UDP 14550 on localhost.
//

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <format>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;

int main(int, char**)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    ConnectionResult connection_result =
        mavsdk.add_any_connection("udpin://0.0.0.0:12550", ForwardingOption::ForwardingOn);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << std::format("First connection failed: {}\n", connection_result);
        return 1;
    }

    connection_result =
        mavsdk.add_any_connection("udpout://127.0.0.1:14550", ForwardingOption::ForwardingOn);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << std::format("Second connection failed: {}\n", connection_result);
        return 1;
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
