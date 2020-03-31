/*
 * Dummy gimbal device implementing MAVLink Gimbal Protocol v2.
 */
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <iostream>
#include <thread>
#include <mavlink/v2.0/common/mavlink.h>
#include <mavlink/v2.0/common/mavlink_msg_gimbal_device_information.h>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console color

void usage(std::string bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <connection_url> <id>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev:[baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl
              << "Id should be from [1-6] indicating gimbal 1-6" << std::endl;
}

int main(int argc, char** argv)
{
    Mavsdk dc;
    std::string connection_url;
    int gimbal_id;
    ConnectionResult connection_result;

    bool discovered_system = false;
    if (argc == 3) {
        // Connect to the system
        connection_url = argv[1];
        connection_result = dc.add_any_connection(connection_url);
        gimbal_id = std::stoi(argv[2]);
    } else {
        // Print the usage info if we're missing specified args
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT
                  << "Connection failed: " << connection_result_str(connection_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    System& system = dc.system();

    // Register system
    std::cout << "Waiting to discover system..." << std::endl;
    dc.register_on_discover([&discovered_system](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        discovered_system = true;
    });

    sleep_for(seconds(2));

    if (!discovered_system) {
        std::cout << ERROR_CONSOLE_TEXT << "No system found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    // Register callback for when we receive a gimbal device info request
    bool received_information = false;
    MavlinkPassthrough pass(system);
    pass.subscribe_message_async(MAVLINK_MSG_ID_GIMBAL_DEVICE_INFORMATION,
            [&received_information, &gimbal_id](const mavlink_message_t message) {
        std::cout << "Received" << sizeof(message) << gimbal_id << std::endl;
    });

    while (true) {
        sleep_for(seconds(1));
    }

    return 0;
}
