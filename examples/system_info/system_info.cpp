//
// Simple example to demonstrate how to query system info using MAVSDK.
//

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/info/info.h>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    auto info = Info{system.value()};

    // Wait until version/firmware information has been populated from the vehicle
    while (info.get_identification().first == Info::Result::InformationNotReceivedYet) {
        std::cout << "Waiting for Version information to populate from system." << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Get the system Version struct
    const Info::Version& system_version = info.get_version().second;

    // Print out the vehicle version information.
    std::cout << system_version << std::endl;

    // Get the system Product struct
    const Info::Product& system_product = info.get_product().second;

    // Print out the vehicle product information.
    std::cout << system_product << std::endl;

    return 0;
}
