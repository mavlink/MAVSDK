//
// Example showing how to make a VTOL takeoff, transition to fixedwing, loiter
// for a bit, and then transition back and return to launch.
//

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>
#include <cmath>
#include <future>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

using std::this_thread::sleep_for;
using std::chrono::seconds;
using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
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

    // We want to listen to the altitude of the drone at 1 Hz.
    const Telemetry::Result set_rate_result = telemetry.set_rate_position(1.0);
    if (set_rate_result != Telemetry::Result::Success) {
        std::cerr << "Setting rate failed: " << set_rate_result << '\n';
        return 1;
    }

    // Set up callback to monitor altitude.
    telemetry.subscribe_position([](Telemetry::Position position) {
        std::cout << "Altitude: " << position.relative_altitude_m << " m\n";
    });

    // Wait until we are ready to arm.
    while (!telemetry.health_all_ok()) {
        std::cout << "Waiting for vehicle to be ready to arm...\n";
        sleep_for(seconds(1));
    }

    // Arm vehicle
    std::cout << "Arming.\n";
    const Action::Result arm_result = action.arm();

    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return 1;
    }

    // Take off
    std::cout << "Taking off.\n";
    const Action::Result takeoff_result = action.takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << "Takeoff failed:n" << takeoff_result << '\n';
        return 1;
    }

    // Wait while it takes off.
    sleep_for(seconds(10));

    std::cout << "Transition to fixedwing.\n";
    const Action::Result fw_result = action.transition_to_fixedwing();

    if (fw_result != Action::Result::Success) {
        std::cerr << "Transition to fixed wing failed: " << fw_result << '\n';
        return 1;
    }

    // Let it transition and start loitering.
    sleep_for(seconds(30));

    // Send it South.
    std::cout << "Sending it to location.\n";
    // We pass latitude and longitude but leave altitude and yaw unset by passing NAN.
    const Action::Result goto_result = action.goto_location(47.3633001, 8.5428515, NAN, NAN);
    if (goto_result != Action::Result::Success) {
        std::cerr << "Goto command failed: " << goto_result << '\n';
        return 1;
    }

    // Let it fly South for a bit.
    sleep_for(seconds(20));

    // Let's stop before reaching the goto point and go back to hover.
    std::cout << "Transition back to multicopter...\n";
    const Action::Result mc_result = action.transition_to_multicopter();
    if (mc_result != Action::Result::Success) {
        std::cerr << "Transition to multi copter failed: " << mc_result << '\n';
        return 1;
    }

    // Wait for the transition to be carried out.
    sleep_for(seconds(5));

    // Now just land here.
    std::cout << "Landing...\n";
    const Action::Result land_result = action.land();
    if (land_result != Action::Result::Success) {
        std::cerr << "Land failed: " << land_result << '\n';
        return 1;
    }

    // Wait until disarmed.
    while (telemetry.armed()) {
        std::cout << "Waiting for vehicle to land and disarm\n.";
        sleep_for(seconds(1));
    }

    std::cout << "Disarmed, exiting.\n";

    return 0;
}
