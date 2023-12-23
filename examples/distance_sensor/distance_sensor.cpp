//
// Simple example to demonstrate how to imitate that all distance sensor
// readings are read from all orientations.
//
// Author: Julian Oes <julian@oes.ch>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;

static void subscribe_distance_sensor(Telemetry& telemetry);

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

    // subscribe to distance sensor readings
    subscribe_distance_sensor(telemetry);

    // endless loop so that the sensor readings will keep comming
    // will close only with kill or Ctrl+C
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

void subscribe_distance_sensor(Telemetry& telemetry)
{
    telemetry.subscribe_distance_sensor([](Telemetry::DistanceSensor dist_sensor) {
        std::cout << "Distance sensor dist: " << dist_sensor.current_distance_m
                  << " - orient: " << dist_sensor.orientation << '\n';
    });
}
