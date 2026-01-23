//
// Simple example to demonstrate how to use hold (loiter) to pause movement,
// and how to interrupt return to launch with hold.
//

#include <chrono>
#include <cmath>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
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
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udp://<our_ip>:<port>\n"
              << " For UDP client: udp://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
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
        std::cerr << "Connection failed: " << connection_result << '\n';
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
        std::cerr << "Arming failed: " << arm_result << '\n';
        return 1;
    }

    // Take off
    std::cout << "Taking off...\n";
    const Action::Result takeoff_result = action.takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << "Takeoff failed: " << takeoff_result << '\n';
        return 1;
    }

    sleep_for(seconds(2));

    // Go somewhere with a GoTo command
    std::cout << "Commanding GoTo...\n";
    Action::Result goto_result = action.goto_location(47.398000, 8.545592, NAN, NAN);
    if (goto_result != Action::Result::Success) {
        std::cerr << "Goto failed: " << goto_result << '\n';
        return 1;
    }
    std::cout << "Commanded GoTo.\n";

    // Let it fly for a bit then do RTL
    sleep_for(seconds(15));

    std::cout << "Commanding RTL...\n";
    Action::Result rtl_result = action.return_to_launch();
    if (rtl_result != Action::Result::Success) {
        std::cerr << "RTL failed: " << rtl_result << '\n';
        return 1;
    }
    std::cout << "Commanded RTL.\n";

    // Let RTL proceed for a bit, then interrupt with Hold
    sleep_for(seconds(10));

    std::cout << "Commanding Hold/Loiter to interrupt RTL...\n";
    Action::Result hold_result = action.hold();
    if (hold_result != Action::Result::Success) {
        std::cerr << "Hold failed: " << hold_result << '\n';
        return 1;
    }
    std::cout << "Commanded Hold/Loiter.\n";

    // Hold position for a bit
    sleep_for(seconds(5));

    // Now land
    std::cout << "Landing...\n";
    const Action::Result land_result = action.land();
    if (land_result != Action::Result::Success) {
        std::cerr << "Land failed: " << land_result << '\n';
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
