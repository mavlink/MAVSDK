//
// Connect to and control a gimbal manager, using the "raw" GimbalManager API
// Unless you need specific functionality provided by the GimbalManager,
// use the high-level Gimbal class, exemplifid in examples/gimbal
//
// Author: Kalyan Sriram <coder.kalyan@gmail.com>

#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/gimbal_manager/gimbal_manager.h>
#include <iostream>
#include <thread>
#include <sys/time.h>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console color

void usage(std::string bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev:[baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
              //<< "Id should be from [1-6] indicating gimbal 1-6" << std::endl;
}

//uint8_t parse_component_id(char* raw) {
    //switch (std::stoi(raw)) {
        //case 1:
            //return MAV_COMP_ID_GIMBAL;
        //case 2:
            //return MAV_COMP_ID_GIMBAL2;
        //case 3:
            //return MAV_COMP_ID_GIMBAL3;
        //case 4:
            //return MAV_COMP_ID_GIMBAL4;
        //case 5:
            //return MAV_COMP_ID_GIMBAL5;
        //case 6:
            //return MAV_COMP_ID_GIMBAL6;
        //default:
            //return MAV_COMP_ID_GIMBAL;
    //}
//}

int main(int argc, char** argv)
{
    Mavsdk dc;
    //uint8_t manager_sid = 1, manager_cid = MAV_COMP_ID_USER1;

    Mavsdk::Configuration config(Mavsdk::Configuration::UsageType::GroundStation);
    dc.set_configuration(config);

    std::string connection_url;
    //uint8_t device_id;
    ConnectionResult connection_result;

    bool discovered_system = false;
    if (argc == 2) {
        connection_url = argv[1];
        connection_result = dc.add_any_connection(connection_url);
        //device_id = parse_component_id(argv[2]);
    } else {
        usage(argv[0]);
        return 1;
    }

    //std::cout << "Configured for gimbal component " << device_id
              //<< "(" << std::stoi(argv[2]) << ")" << std::endl;

    if (connection_result != ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT
                  << "Connection failed: " << connection_result_str(connection_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    System& system = dc.system(5283920058631409231);

    std::cout << "Waiting to discover system..." << std::endl;
    dc.register_on_discover([&discovered_system](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        if (uuid == 5283920058631409231)
            discovered_system = true;
    });

    sleep_for(seconds(3));

    if (!discovered_system) {
        std::cout << ERROR_CONSOLE_TEXT << "No system found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    GimbalManager manager(system);

    //sleep_for(seconds(6));
    std::cout << "sending" << std::endl;
    manager.request_information();

    return 0;
}
