//
// Example how to listen to a transponder using MAVSDK.
//

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

    Mavsdk mavsdk;
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
    auto transponder = Transponder{system.value()};

    // We want to listen to the transponder of the drone at 1 Hz.
    std::cout << "Setting transponder update rate\n";
    const Transponder::Result set_rate_result = transponder.set_rate_transponder(1.0);
    if (set_rate_result != Transponder::Result::Success) {
        std::cerr << "Setting rate failed:" << set_rate_result << '\n';
        return 1;
    }

    // Set up callback to monitor transponder activity
    std::cout << "Setting transponder subscription\n";
    transponder.subscribe_transponder([](Transponder::AdsbVehicle adsbVehicle) {
        std::cout << "ICAO Address: " << adsbVehicle.icao_address << '\n'
                  << "Latitude: " << adsbVehicle.latitude_deg << " deg\n"
                  << "Longitude: " << adsbVehicle.longitude_deg << " deg\n"
                  << "Absolute Altitude: " << adsbVehicle.absolute_altitude_m << " m\n"
                  << "Heading: " << adsbVehicle.heading_deg << " deg\n"
                  << "Horizontal Velocity: " << adsbVehicle.horizontal_velocity_m_s << " m/s\n"
                  << "Vertical Velocity: " << adsbVehicle.vertical_velocity_m_s << " m/s\n"
                  << "Call Sign: " << adsbVehicle.callsign << '\n'
                  << "Emitter Type: " << adsbVehicle.emitter_type << '\n'
                  << "Squawk: " << adsbVehicle.squawk << '\n';
    });

    // Search for aircraft transponders
    sleep_for(seconds(60));
    std::cout << "Finished...\n";

    return 0;
}
