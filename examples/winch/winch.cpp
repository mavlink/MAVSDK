//
// Example how to winch MAVSDK.
//

#include <cstdint>
#include <future>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/winch/winch.h>
#include <iostream>
#include <thread>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

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

    // Instantiate plugin.
    Winch winch(system.value());

    // This seems to fail against normal PX4 SITL with payload_deliverer started.
    // Presumably that's because commands are sent to component 169 and not 1.
    const auto result = winch.lock(1);
    if (result != Winch::Result::Success) {
        std::cerr << "Winch result: " << result << '\n';
        return 1;
    }

    return 0;
}
