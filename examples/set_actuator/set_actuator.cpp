//
// Simple example to demonstrate how to control actuators (i.e. servos) using MAVSDK.
//
// Author: Kalyan Sriram <kalyan@coderkalyan.com>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>

using namespace mavsdk;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console color

void usage(std::string bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage :" << bin_name << " <connection_url>"
              << " <actuator_index> <actuator_value>" << std::endl
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
    int index;
    float value;

    if (argc == 4) {
        connection_url = argv[1];
        connection_result = mavsdk.add_any_connection(connection_url);
        index = std::stod(argv[2]);
        value = std::stof(argv[3]);
    } else {
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::Success) {
        std::cout << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    std::promise<void> prom;
    std::future<void> fut = prom.get_future();
    std::cout << "Waiting to discover system..." << std::endl;
    mavsdk.register_on_discover([&prom](uint64_t /* uuid*/) { prom.set_value(); });

    if (fut.wait_for(std::chrono::seconds(2)) != std::future_status::ready) {
        std::cout << "No device found, exiting." << std::endl;
        return 1;
    }

    System& system = mavsdk.system();

    auto action = std::make_shared<Action>(system);

    std::cout << "Setting actuator..." << std::endl;
    const Action::Result set_actuator_result = action->set_actuator(index, value);

    if (set_actuator_result != Action::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Setting actuator failed:" << set_actuator_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    return 0;
}
