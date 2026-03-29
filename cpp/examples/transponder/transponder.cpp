//
// Example how to listen to a transponder using MAVSDK.
//

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/transponder/transponder.h>
#include <format>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << std::format(
        "Usage : {} <connection_url>\nConnection URL format should be :\n For TCP server: tcpin://<our_ip>:<port>\n For TCP client: tcpout://<remote_ip>:<port>\n For UDP server: udpin://<our_ip>:<port>\n For UDP client: udpout://<remote_ip>:<port>\n For Serial : serial://</path/to/serial/dev>:<baudrate>]\nFor example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n",
        bin_name);
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << std::format("Connection failed: {}\n", connection_result);
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
        std::cerr << std::format("Setting rate failed:{}\n", set_rate_result);
        return 1;
    }

    // Set up callback to monitor transponder activity
    std::cout << "Setting transponder subscription\n";
    transponder.subscribe_transponder([](Transponder::AdsbVehicle adsbVehicle) {
        std::cout << std::format(
            "ICAO Address: {}\nLatitude: {} deg\nLongitude: {} deg\nAbsolute Altitude: {} m\nHeading: {} deg\nHorizontal Velocity: {} m/s\nVertical Velocity: {} m/s\nCall Sign: {}\nEmitter Type: {}\nSquawk: {}\n",
            adsbVehicle.icao_address,
            adsbVehicle.latitude_deg,
            adsbVehicle.longitude_deg,
            adsbVehicle.absolute_altitude_m,
            adsbVehicle.heading_deg,
            adsbVehicle.horizontal_velocity_m_s,
            adsbVehicle.vertical_velocity_m_s,
            adsbVehicle.callsign,
            adsbVehicle.emitter_type,
            adsbVehicle.squawk);
    });

    // Search for aircraft transponders
    sleep_for(seconds(60));
    std::cout << "Finished...\n";

    return 0;
}
