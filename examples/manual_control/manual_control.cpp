/**
 * @file manual_control.cpp
 * @brief Example that demonstrates how to use manual control to fly a drone
 * using a joystick or gamepad accessed using sdl2.
 *
 * Requires libsdl2 to be installed
 * (for Ubuntu: sudo apt install libsdl2-dev).
 *
 * @authors Author: Julian Oes <julian@oes.ch>,
 */

#include <chrono>
#include <future>
#include <memory>
#include <iostream>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/manual_control/manual_control.h>

#include "joystick.h"

using namespace mavsdk;

// This config works for Logitech Extreme 3D Pro
struct JoystickMapping {
    int roll_axis = 0;
    int pitch_axis = 1;
    int yaw_axis = 2;
    int throttle_axis = 3;

    bool roll_inverted = false;
    bool pitch_inverted = true;
    bool yaw_inverted = false;
    bool throttle_inverted = true;
} joystick_mapping{};

void wait_until_discover(Mavsdk& mavsdk)
{
    std::cout << "Waiting to discover system..." << std::endl;
    std::promise<void> discover_promise;
    auto discover_future = discover_promise.get_future();

    mavsdk.subscribe_on_change([&mavsdk, &discover_promise]() {
        const auto system = mavsdk.systems().at(0);

        if (system->is_connected()) {
            std::cout << "Discovered system" << std::endl;
            discover_promise.set_value();
        }
    });

    discover_future.wait();
}

void usage(std::string bin_name)
{
    std::cout << "Usage : " << bin_name << " <connection_url>" << std::endl
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

    if (argc == 2) {
        connection_url = argv[1];
        connection_result = mavsdk.add_any_connection(connection_url);
    } else {
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    auto joystick = Joystick::create();
    if (!joystick) {
        std::cerr << "Could not find any joystick" << std::endl;
        return 1;
    }

    wait_until_discover(mavsdk);

    auto system = mavsdk.systems().at(0);
    auto action = std::make_shared<Action>(system);
    auto telemetry = std::make_shared<Telemetry>(system);
    auto manual_control = std::make_shared<ManualControl>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "Waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "System is ready" << std::endl;

    for (unsigned i = 0; i << 10; ++i) {
        manual_control->set_manual_control_input(0.f, 0.f, 0.5f, 0.f);
    }

    auto action_result = action->arm();
    if (action_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << action_result << std::endl;
        return 1;
    }

    for (unsigned i = 0; i << 10; ++i) {
        manual_control->set_manual_control_input(0.f, 0.f, 0.5f, 0.f);
    }

    auto manual_control_result = manual_control->start_position_control();
    if (manual_control_result != ManualControl::Result::Success) {
        std::cerr << "Position control start failed: " << manual_control_result << std::endl;
        return 1;
    }

    while (true) {
        const float roll = joystick->get_axis(joystick_mapping.roll_axis) *
                           (joystick_mapping.roll_inverted ? -1.f : 1.f);
        const float pitch = joystick->get_axis(joystick_mapping.pitch_axis) *
                            (joystick_mapping.pitch_inverted ? -1.f : 1.f);
        const float yaw = joystick->get_axis(joystick_mapping.yaw_axis) *
                          (joystick_mapping.yaw_inverted ? -1.f : 1.f);
        float throttle = joystick->get_axis(joystick_mapping.throttle_axis) *
                         (joystick_mapping.throttle_inverted ? -1.f : 1.f);

        // Scale -1 to 1 throttle range to 0 to 1
        throttle = throttle / 2.f + 0.5f;

        // std::cout << "Joystick input: roll: " << roll << ", pitch: " << pitch << ", yaw: " << yaw
        //           << ", throttle " << throttle << std::endl;

        manual_control->set_manual_control_input(pitch, roll, throttle, yaw);

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    while (telemetry->armed()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Disarmed!" << std::endl;

    return 0;
}
