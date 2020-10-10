/**
 * @file geofence_inclusion.cpp
 *
 * @brief Demonstrates how to Add & Upload geofence using the Dronecode SDK.
 * The example is summarised below:
 * 1. Adds points to geofence.
 * 2. Uploads the geofence mission.
 *
 * @author Jonathan Zaturensky <jonathan@airmap.com>,
 * @date 2019-05-09
 */

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/geofence/geofence.h>

#include <functional>
#include <future>
#include <iostream>

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

using namespace mavsdk;
using namespace std::placeholders; // for `_1`
using namespace std::chrono; // for seconds(), milliseconds()
using namespace std::this_thread; // for sleep_for()

static Geofence::Point add_point(double latitude_deg, double longitude_deg);

void usage(std::string bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}

int main(int argc, char** argv)
{
    Mavsdk mavsdk;

    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        std::cout << "Waiting to discover system..." << std::endl;
        mavsdk.subscribe_on_change([&mavsdk, prom]() {
            const auto system = mavsdk.systems().at(0);

            if (system->is_connected()) {
                std::cout << "Discovered system" << std::endl;
                prom->set_value();
            } else {
                std::cout << "System timed out" << std::endl;
                std::cout << "Exiting." << std::endl;
                exit(0);
            }
        });

        std::string connection_url;
        ConnectionResult connection_result;

        if (argc == 2) {
            connection_url = argv[1];
            connection_result = mavsdk.add_any_connection(connection_url);
        } else {
            usage(argv[0]);
            return 1;
        }

        if (connection_result != ConnectionResult::Success) {
            std::cout << ERROR_CONSOLE_TEXT << "Connection failed: " << connection_result
                      << NORMAL_CONSOLE_TEXT << std::endl;
            return 1;
        }

        future_result.get();
    }

    auto system = mavsdk.systems().at(0);
    auto action = std::make_shared<Action>(system);
    auto mission = std::make_shared<Mission>(system);
    auto telemetry = std::make_shared<Telemetry>(system);
    auto geofence = std::make_shared<Geofence>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "Waiting for system to be ready" << std::endl;
        sleep_for(seconds(1));
    }

    std::cout << "System ready" << std::endl;
    std::cout << "Creating and uploading geofence" << std::endl;

    std::vector<Geofence::Point> points;
    points.push_back(add_point(47.39929240, 8.54296524));
    points.push_back(add_point(47.39696482, 8.54161340));
    points.push_back(add_point(47.39626761, 8.54527193));
    points.push_back(add_point(47.39980072, 8.54736050));

    std::vector<Geofence::Polygon> polygons;
    Geofence::Polygon new_polygon{};
    new_polygon.fence_type = Geofence::Polygon::FenceType::Inclusion;
    new_polygon.points = points;

    polygons.push_back(new_polygon);

    {
        std::cout << "Uploading geofence..." << std::endl;

        const Geofence::Result result = geofence->upload_geofence(polygons);

        if (result != Geofence::Result::Success) {
            std::cout << "Geofence upload failed (" << result << "), exiting." << std::endl;
            return 1;
        }
        std::cout << "Geofence uploaded." << std::endl;
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
