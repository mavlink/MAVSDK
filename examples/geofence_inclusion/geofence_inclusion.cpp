//
// Demonstrates how to Add & Upload geofence using MAVSDK.
// The example is summarised below:
// 1. Adds points to geofence.
// 2. Uploads the geofence mission.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/geofence/geofence.h>

#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

static Geofence::Point add_point(double latitude_deg, double longitude_deg);

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
    mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
        auto system = mavsdk.systems().back();

        if (system->has_autopilot()) {
            std::cout << "Discovered autopilot\n";

            // Unsubscribe again as we only want to find one system.
            mavsdk.subscribe_on_new_system(nullptr);
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

    // Instantiate plugins.
    auto telemetry = Telemetry{system};
    auto geofence = Geofence{system};

    while (!telemetry.health_all_ok()) {
        std::cout << "Waiting for system to be ready\n";
        sleep_for(seconds(1));
    }

    std::cout << "System ready\n";
    std::cout << "Creating and uploading geofence\n";

    std::vector<Geofence::Point> points;
    points.emplace_back(add_point(47.39929240, 8.54296524));
    points.emplace_back(add_point(47.39696482, 8.54161340));
    points.emplace_back(add_point(47.39626761, 8.54527193));
    points.emplace_back(add_point(47.39980072, 8.54736050));

    std::vector<Geofence::Polygon> polygons;
    Geofence::Polygon new_polygon{};
    new_polygon.fence_type = Geofence::Polygon::FenceType::Inclusion;
    new_polygon.points = points;

    polygons.push_back(new_polygon);

    {
        std::cout << "Uploading geofence...\n";

        const Geofence::Result result = geofence.upload_geofence(polygons);

        if (result != Geofence::Result::Success) {
            std::cerr << "Geofence upload failed: " << result << ", exiting.\n";
            return 1;
        }
        std::cout << "Geofence uploaded.\n";
    }

    return 0;
}

Geofence::Point add_point(double latitude_deg, double longitude_deg)
{
    Geofence::Point new_point;
    new_point.latitude_deg = latitude_deg;
    new_point.longitude_deg = longitude_deg;
    return new_point;
}
