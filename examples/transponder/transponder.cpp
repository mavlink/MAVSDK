//
// Simple example to demonstrate how to use the MAVSDK.
//
// Author: Richard Beasley <richard.beasley@fractureddynamics.com>

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/transponder/transponder.h>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TRANSPONDER_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console color

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

    std::cout << "Waiting to discover system..." << std::endl;
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
        auto system = mavsdk.systems().back();

        if (system->has_autopilot()) {
            std::cout << "Discovered autopilot" << std::endl;

            // Unsubscribe again as we only want to find one system.
            mavsdk.subscribe_on_new_system(nullptr);
            prom.set_value(system);
        }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds.
    if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
        std::cout << ERROR_CONSOLE_TEXT << "No autopilot found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    auto system = fut.get();

    auto transponder = Transponder{system};

    // We want to listen to the transponder of the drone at 1 Hz.
    std::cout << "Setting transponder update rate" << std::endl;
    const Transponder::Result set_rate_result = transponder.set_rate_transponder(1.0);
    if (set_rate_result != Transponder::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Setting rate failed:" << set_rate_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Set up callback to monitor transponder activity
    std::cout << "Setting transponder subscription" << std::endl;
    transponder.subscribe_transponder([](Transponder::AdsbVehicle adsbVehicle) {
        std::cout << TRANSPONDER_CONSOLE_TEXT // set to blue
                  << "ICAO Address: " << adsbVehicle.icao_address << std::endl
                  << "Latitude: " << adsbVehicle.latitude_deg << " deg" << std::endl
                  << "Longitude: " << adsbVehicle.longitude_deg << " deg" << std::endl
                  << "Absolute Altitude: " << adsbVehicle.absolute_altitude_m << "m" << std::endl
                  << "Heading: " << adsbVehicle.heading_deg << " deg" << std::endl
                  << "Horizontal Velocity: " << adsbVehicle.horizontal_velocity_m_s << "m/s" << std::endl
                  << "Vertical Velocity: " << adsbVehicle.vertical_velocity_m_s << "m/s" << std::endl
                  << "Call Sign: " << adsbVehicle.callsign << std::endl
                  << "Emitter Type: " << adsbVehicle.emitter_type << std::endl
                  << "Squawk: " << adsbVehicle.squawk << std::endl
                  << NORMAL_CONSOLE_TEXT // set to default color again
                  << std::endl;
    });

    // Search for aircraft transponders
    sleep_for(seconds(60));
    std::cout << "Finished..." << std::endl;

    return 0;
}
