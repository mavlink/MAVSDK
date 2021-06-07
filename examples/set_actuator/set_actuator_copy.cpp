//
// Simple example to demonstrate how to control actuators (i.e. servos) using MAVSDK.
//
// Author: Kalyan Sriram <kalyan@coderkalyan.com>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>

using namespace mavsdk;
using namespace std;
using namespace std::chrono; // for seconds(), milliseconds()
using namespace std::this_thread; // for sleep_for()

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour
/**************************************************************/

void usage(std::string bin_name)
{
    std::cerr << "Usage :" << bin_name << " <connection_url> <actuator_index> <actuator_value>\n"
              << "Connection URL format should be :\n"
              << '\n'
              << " For TCP : tcp://[server_host][:server_port]\n"
              << '\n'
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << '\n'
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << '\n'
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        usage(argv[0]);
        return 1;
    }

    const std::string connection_url = argv[1];
    const int index = std::stod(argv[2]);
    const float value = std::stof(argv[3]);

    Mavsdk mavsdk;
    const ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    std::cout << "Waiting to discover system...\n";
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    // We wait for new systems to be discovered, once we find one that has an
    // autopilot, we decide to use it.
    mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
        auto system = mavsdk.systems().back();

        if (system->has_autopilot()) {
            std::cout << "Discovered autopilot\n";

            // Unsubscribe again as we only want to find one system.
            mavsdk.subscribe_on_new_system(nullptr);
            prom.set_value(system);
        }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds max, surely.
    if (fut.wait_for(std::chrono::seconds(3)) == std::future_status::timeout) {
        std::cerr << "No autopilot found, exiting.\n";
        return 1;
    }

    // Get discovered system now.
    auto system = fut.get();

    // Instantiate plugins.
    auto action = Action{system};
    auto telemetry = Telemetry{system};

    //5HZ
    const Telemetry::Result set_actuator_output_rate = telemetry.set_rate_actuator_output_status(5.0);
    if (set_actuator_output_rate != Telemetry::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Setting rate failed:" << set_actuator_output_rate
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry.subscribe_actuator_output_status([](Telemetry::ActuatorOutputStatus actuator_output) {
        std::cout << TELEMETRY_CONSOLE_TEXT // set to blue
                  << "Main0 Output: " << sizeof(actuator_output) << std::endl
                  << "Main1 Output: " << actuator_output.actuator[1] << std::endl
                  << "Main2 Output: " << actuator_output.actuator[2] << std::endl
                  << "Main3 Output: " << actuator_output.actuator[3] << std::endl
                  << "Main4 Output: " << actuator_output.actuator[4] << std::endl
                  << NORMAL_CONSOLE_TEXT // set to default color again
                  << std::endl;
    });

    const Telemetry::Result set_actuator_controltarget_rate = telemetry.set_rate_actuator_control_target(5.0);
    if (set_actuator_controltarget_rate != Telemetry::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Setting rate failed:" << set_actuator_controltarget_rate
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry.subscribe_actuator_control_target([](Telemetry::ActuatorControlTarget actuator_control) {
        std::cout << TELEMETRY_CONSOLE_TEXT // set to blue
                  << "Target0 Output: " << sizeof(actuator_control.controls) << std::endl
                  << "Target1 Output: " << actuator_control.controls[1] << std::endl
                  << "Target2 Output: " << actuator_control.controls[2] << std::endl
                  << "Target3 Output: " << actuator_control.controls[3] << std::endl
                  << "Target4 Output: " << actuator_control.controls[4] << std::endl
                  << NORMAL_CONSOLE_TEXT // set to default color again
                  << std::endl;
    });

    std::cout << "Setting actuator...\n";
    const Action::Result set_actuator_result = action.set_actuator(index, value);

    if (set_actuator_result != Action::Result::Success) {
        std::cerr << "Setting actuator failed:" << set_actuator_result << '\n';
        return 1;
    }
    while(1)
    {
        sleep_for(seconds(1));

        const Action::Result set_actuator_result = action.set_actuator(index, value);
        if (set_actuator_result != Action::Result::Success) {
        std::cerr << "Setting actuator failed:" << set_actuator_result << '\n';
        return 1;
    }
    }

    return 0;
}