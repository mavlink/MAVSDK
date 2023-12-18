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

    // Instantiate plugins.
    auto telemetry = Telemetry{system.value()};
    auto geofence = Geofence{system.value()};

    while (!telemetry.health_all_ok()) {
        std::cout << "Waiting for system to be ready\n";
        sleep_for(seconds(1));
    }

    std::cout << "System ready\n";
    std::cout << "Creating and uploading geofences\n";

    std::vector<Geofence::Polygon> polygons;
    std::vector<Geofence::Circle> circles;

    // Polygon inclusion
    {
        std::vector<Geofence::Point> points;
        points.emplace_back(add_point(47.40029181357694, 8.540208324993841));
        points.emplace_back(add_point(47.400173434578114, 8.547689686153717));
        points.emplace_back(add_point(47.39677781722575, 8.547444838745832));
        points.emplace_back(add_point(47.39709351430107, 8.539753608416305));
        Geofence::Polygon new_polygon{};
        new_polygon.fence_type = Geofence::FenceType::Inclusion;
        new_polygon.points = points;
        polygons.push_back(new_polygon);
    }

    // Polygon exclusion
    {
        std::vector<Geofence::Point> points;
        points.emplace_back(add_point(47.39845295869903, 8.543682820794851));
        points.emplace_back(add_point(47.39837403681116, 8.545066315854541));
        points.emplace_back(add_point(47.39759073790796, 8.544926413920791));
        points.emplace_back(add_point(47.39762230688655, 8.543531259263398));
        Geofence::Polygon new_polygon{};
        new_polygon.fence_type = Geofence::FenceType::Exclusion;
        new_polygon.points = points;
        polygons.push_back(new_polygon);
    }

    // Circle inclusion
    {
        Geofence::Point center = add_point(47.39867310982157, 8.54379160368353);
        Geofence::Circle new_circle{};
        new_circle.point = center;
        new_circle.fence_type = Geofence::FenceType::Inclusion;
        new_circle.radius = 319.54F;
        circles.push_back(new_circle);
    }

    // Circle exclusion
    {
        Geofence::Point center = add_point(47.39928080314044, 8.54540060087578);
        Geofence::Circle new_circle{};
        new_circle.point = center;
        new_circle.fence_type = Geofence::FenceType::Exclusion;
        new_circle.radius = 49.52F;
        circles.push_back(new_circle);
    }

    Geofence::GeofenceData geofence_data{};
    geofence_data.polygons = polygons;
    geofence_data.circles = circles;

    std::cout << "Uploading geofence...\n";

    const Geofence::Result result = geofence.upload_geofence(geofence_data);

    if (result != Geofence::Result::Success) {
        std::cerr << "Geofence upload failed: " << result << ", exiting.\n";
        return 1;
    }
    std::cout << "Geofence uploaded.\n";

    return 0;
}

Geofence::Point add_point(double latitude_deg, double longitude_deg)
{
    Geofence::Point new_point;
    new_point.latitude_deg = latitude_deg;
    new_point.longitude_deg = longitude_deg;
    return new_point;
}
