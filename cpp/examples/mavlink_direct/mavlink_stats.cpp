//
// Example to display stats about all messages arriving from the
// common.xml MAVLink dialect
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.h>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <thread>
#include <map>

using namespace mavsdk;

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

    // Message statistics tracking
    std::map<std::string, unsigned> message_counts;
    auto start_time = std::chrono::high_resolution_clock::now();

    // Subscribe to any message name
    auto handle = mavlink_direct.subscribe_message(
        "", [&message_counts](const mavsdk::MavlinkDirect::MavlinkMessage& message) {
            message_counts[message.message_name]++;
        });

    std::cout << "Listening MAVLink messages. Press Ctrl+C to exit..." << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed =
            std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();

        // Clear screen for better readability
        std::cout << "\033[2J\033[H";

        std::cout << "┌──────────────────────────────────────────────────────┐\n";
        std::cout << "│ MAVLink Message Statistics                           │\n";
        std::cout << "│ Runtime: " << std::setw(3) << elapsed
                  << " seconds                                 │\n";
        std::cout << "├──────────────────────────────────┬───────┬───────────┤\n";
        std::cout << "│ Message name                     │ Total │ Rate (Hz) │\n";
        std::cout << "├──────────────────────────────────┼───────┼───────────┤\n";

        for (const auto& [message_name, count] : message_counts) {
            double messages_per_second = static_cast<double>(count) / elapsed;
            std::cout << "│ " << std::left << std::setw(32) << message_name << " │ " << std::right
                      << std::setw(5) << count << " │ " << std::right << std::setw(9) << std::fixed
                      << std::setprecision(2) << messages_per_second << " │\n";
        }

        std::cout << "└──────────────────────────────────┴───────┴───────────┘\n";
        std::cout << std::flush;
    }

    // Unsubscribe from all messages
    mavlink_direct.unsubscribe_message(handle);
    std::cout << "Unsubscribed from MAVLink messages, exiting." << std::endl;

    return 0;
}
