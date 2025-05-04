//
// How to set up a SiK radio by sending a MAVLink command.
//

#include <cstdint>
#include <future>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <iostream>
#include <string>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url> <radio_id>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

std::shared_ptr<System> get_system(Mavsdk& mavsdk)
{
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
    if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
        std::cerr << "No autopilot found.\n";
        return {};
    }

    // Get discovered system now.
    return fut.get();
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    const int new_radio_id = std::stoi(argv[2]);

    Mavsdk mavsdk;
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = get_system(mavsdk);
    if (!system) {
        return 1;
    }

    // Instantiate plugin.
    MavlinkPassthrough mavlink_passthrough(system);

    std::cout << "Setting radio ID to " << new_radio_id << '\n';

    const auto result = mavlink_passthrough.send_command_long(MavlinkPassthrough::CommandLong{
        mavlink_passthrough.get_target_sysid(),
        mavlink_passthrough.get_target_compid(),
        550,
        0.f, // all IDs
        3.f,
        static_cast<float>(new_radio_id),
        NAN,
        NAN,
        NAN,
        NAN});

    if (result != MavlinkPassthrough::Result::Success) {
        std::cerr << "Command failed: " << result << '\n';
        return 1;
    }

    std::cerr << "New radio ID " << new_radio_id << " set\n";

    return 0;
}
