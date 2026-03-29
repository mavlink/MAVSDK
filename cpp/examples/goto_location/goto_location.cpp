//
// Simple example to demonstrate how to use goto_location and set_current_speed.
//

#include <chrono>
#include <cmath>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
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

    // Instantiate plugins.
    auto telemetry = Telemetry{system.value()};
    auto action = Action{system.value()};

    // Check until vehicle is ready to arm
    while (telemetry.health_all_ok() != true) {
        std::cout << "Vehicle is getting ready to arm\n";
        sleep_for(seconds(1));
    }

    // Arm vehicle
    std::cout << "Arming...\n";
    const Action::Result arm_result = action.arm();

    if (arm_result != Action::Result::Success) {
        std::cerr << std::format("Arming failed: {}\n", arm_result);
        return 1;
    }

    // Take off
    std::cout << "Taking off...\n";
    const Action::Result takeoff_result = action.takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << std::format("Takeoff failed: {}\n", takeoff_result);
        return 1;
    }

    sleep_for(seconds(4));

    // Go to a location (use NAN for altitude and yaw to keep current values)
    std::cout << "Going to first location...\n";
    Action::Result goto_result = action.goto_location(47.4, 8.545592, NAN, NAN);
    if (goto_result != Action::Result::Success) {
        std::cerr << std::format("Goto failed: {}\n", goto_result);
        return 1;
    }

    sleep_for(seconds(4));

    // Go slow for a bit by setting current speed
    std::cout << "Slowing down to 1 m/s...\n";
    Action::Result speed_result = action.set_current_speed(1.0f);
    if (speed_result != Action::Result::Success) {
        std::cerr << std::format("Set speed failed: {}\n", speed_result);
        return 1;
    }

    sleep_for(seconds(4));

    // Speed up again
    std::cout << "Speeding up to 5 m/s...\n";
    speed_result = action.set_current_speed(5.0f);
    if (speed_result != Action::Result::Success) {
        std::cerr << std::format("Set speed failed: {}\n", speed_result);
        return 1;
    }

    sleep_for(seconds(4));

    // Go back to a different location
    std::cout << "Going back to second location...\n";
    goto_result = action.goto_location(47.3977233, 8.545592, NAN, NAN);
    if (goto_result != Action::Result::Success) {
        std::cerr << std::format("Goto failed: {}\n", goto_result);
        return 1;
    }

    sleep_for(seconds(10));

    // Land
    std::cout << "Landing...\n";
    const Action::Result land_result = action.land();
    if (land_result != Action::Result::Success) {
        std::cerr << std::format("Land failed: {}\n", land_result);
        return 1;
    }

    // Check if vehicle is still in air
    while (telemetry.in_air()) {
        std::cout << "Vehicle is landing...\n";
        sleep_for(seconds(1));
    }
    std::cout << "Landed!\n";

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.
    sleep_for(seconds(3));
    std::cout << "Finished...\n";

    return 0;
}
