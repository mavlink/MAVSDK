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

    auto system = get_system(mavsdk);
    if (!system) {
        return 1;
    }

    auto info = Info{system};

    // Wait until version/firmware information has been populated from the vehicle
    while (info.get_identification().first == Info::Result::InformationNotReceivedYet) {
        std::cout << "Waiting for Version information to populate from system." << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Get the system Version struct
    const Info::Version& systemVersion = info.get_version().second;

    // Print out the vehicle version information.
    std::cout << "  flight_sw_major: " << systemVersion.flight_sw_major << '\n'
              << "  flight_sw_minor: " << systemVersion.flight_sw_minor << '\n'
              << "  flight_sw_patch: " << systemVersion.flight_sw_patch << '\n'
              << "  flight_sw_version_type: " << systemVersion.flight_sw_version_type << '\n'
              << "  flight_sw_vendor_major: " << systemVersion.flight_sw_vendor_major << '\n'
              << "  flight_sw_vendor_minor: " << systemVersion.flight_sw_vendor_minor << '\n'
              << "  flight_sw_vendor_patch: " << systemVersion.flight_sw_vendor_patch << '\n'
              << "  flight_sw_git_hash: " << systemVersion.flight_sw_git_hash << '\n'
              << "  os_sw_major: " << systemVersion.os_sw_major << '\n'
              << "  os_sw_minor: " << systemVersion.os_sw_minor << '\n'
              << "  os_sw_patch: " << systemVersion.os_sw_patch << '\n'
              << "  os_sw_git_hash: " << systemVersion.os_sw_git_hash << '\n';

    // Get the system Product struct
    const Info::Product& systemProduct = info.get_product().second;

    // Print out the vehicle product information.
    std::cout << "  vendor_id: " << systemProduct.vendor_id << '\n'
              << "  vendor_name: " << systemProduct.vendor_name << '\n'
              << "  product_id: " << systemProduct.product_id << '\n'
              << "  product_name: " << systemProduct.product_id << '\n';

    return 0;
}
