//
// Example to demonstrate how to implement at camera trigger server.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/camera_trigger_server/camera_trigger_server.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(std::string bin_name)
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

    Mavsdk mavsdk;
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    std::cout << "Waiting to discover system...\n";
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    // We wait for new systems to be discovered, once we find one that has a
    // camera, we decide to use it.
    mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
        auto system = mavsdk.systems().back();

        std::cout << "Discovered\n";

        // Unsubscribe again as we only want to find one system.
        mavsdk.subscribe_on_new_system(nullptr);
        prom.set_value(system);
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds max, surely.
    if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
        std::cerr << "No system found, exiting.\n";
        return 1;
    }

    // Get discovered system now.
    auto system = fut.get();

    // Instantiate plugins.
    auto telemetry = Telemetry{system};
    auto camera_trigger_server = CameraTriggerServer{system};

    camera_trigger_server.subscribe_capture([](CameraTriggerServer::Result result, uint32_t sequence_num) {
        std::cout << "Requesting capture: " << sequence_num << '\n';

        // TODO: actually capture image here
    });

    // Wait a bit to make sure we see capture information.
    sleep_for(seconds(60));

    return 0;
}
