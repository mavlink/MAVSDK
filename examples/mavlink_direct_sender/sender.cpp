//
// Example to use MavlinkDirect to send OBSTACLE_DISTANCE message
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.h>
#include <iostream>
#include <chrono>
#include <thread>

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

    // Set up as companion computer
    auto config = Mavsdk::Configuration{ComponentType::CompanionComputer};

    // Create MAVSDK instance
    Mavsdk mavsdk{config};

    // Connect using the provided connection URL
    auto connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    // Discover and connect to system
    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "No autopilot found" << std::endl;
        return 1;
    }

    // Create MavlinkDirect plugin instance
    auto mavlink_direct = MavlinkDirect{system.value()};

    // Create a HEARTBEAT message
    MavlinkDirect::MavlinkMessage obstacle_distance{};
    obstacle_distance.message_name = "OBSTACLE_DISTANCE";
    obstacle_distance.system_id = config.get_system_id(); // Your component's system ID
    obstacle_distance.component_id = config.get_component_id(); // Your component's component ID
    obstacle_distance.target_system_id = 0; // Does not apply for this message
    obstacle_distance.target_component_id = 0; // Does not apply for this message

    // For now we just hard-code values for demonstration purposes.
    // We assume the sensor has is looking forward and has 8 segments with 10 degrees per segment,
    // so 80 degrees field of view.
    // On 3 segments we see an obstacle, the closest is at 7m.
    //
    // In a real application this JSON message content would of course be
    // assembled manually, or using a json library like jsoncpp or nlohmann/json.
    obstacle_distance.fields_json = R"({
        "time_usec": 12345678,
        "sensor_type": 3,
        "distances": [
            2000,   2000,  2000,  1000,   800,   700,  2000,  2000,
            65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
            65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
            65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
            65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
            65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
            65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
            65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536,
            65536, 65536, 65536, 65536, 65536, 65536, 65536, 65536
        ],
        "increment": 0,
        "min_distance": 000,
        "max_distance": 2000,
        "increment_f": 10.0,
        "angle_offset": -40.0,
        "frame": 12
    })";

    while (true) {
        // Send the message
        auto result = mavlink_direct.send_message(obstacle_distance);
        if (result == MavlinkDirect::Result::Success) {
            std::cout << "OBSTACLE_DISTANCE message sent successfully" << std::endl;
        } else {
            std::cerr << "OBSTACLE_DISTANCE message could not be sent: " << result << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}
