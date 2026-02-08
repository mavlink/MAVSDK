//
// Example to use MavlinkDirect to subscribe to MAVLink message
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.h>
#include <iostream>
#include <chrono>
#include <thread>

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    // Initialize MAVSDK with GroundStation component type
    mavsdk::Mavsdk mavsdk{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};

    // Add connection
    mavsdk::ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != mavsdk::ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    // Wait for the system to connect
    std::cout << "Waiting for system to connect..." << std::endl;
    while (mavsdk.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Get the connected system
    auto system = mavsdk.systems().at(0);
    if (!system->is_connected()) {
        std::cerr << "System not connected" << std::endl;
        return 1;
    }

    // Instantiate the plugin
    auto mavlink_direct = mavsdk::MavlinkDirect{system};

    // Subscribe to GPS updates
    auto gps_handle = mavlink_direct.subscribe_message(
        "GPS_RAW_INT", [](const mavsdk::MavlinkDirect::MavlinkMessage& message) {
            std::cout << "** " << message.message_name << " **\n";
            std::cout << message.fields_json << '\n';
        });

    std::cout << "Listening for GPS messages. Press Ctrl+C to exit..." << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Unsubscribe from GPS updates
    mavlink_direct.unsubscribe_message(gps_handle);
    std::cout << "Unsubscribed from GPS updates, exiting." << std::endl;

    return 0;
}
