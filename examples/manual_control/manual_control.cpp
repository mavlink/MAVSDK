//
// Example that demonstrates how to use manual control to fly a drone
// using a joystick or gamepad accessed using SDL2.
//
// Requires libsdl2 to be installed
// (for Ubuntu: sudo apt install libsdl2-dev).
//

#include <chrono>
#include <future>
#include <memory>
#include <iostream>
#include <thread>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/manual_control/manual_control.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>

using namespace mavsdk;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;


std::shared_ptr<System> get_system(Mavsdk& mavsdk)
{
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
    if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
        std::cerr << "No autopilot found.\n";
        return {};
    }

    // Get discovered system now.
    return fut.get();
}

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
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1], ForwardingOption::ForwardingOn);

    // To enable forwarding to QGC
    // ConnectionResult connection_result2 = mavsdk.add_any_connection("udp://127.0.0.1:14550", ForwardingOption::ForwardingOn);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = get_system(mavsdk);
    if (!system) {
        return 1;
    }

    // Instantiate plugins.
    auto action = Action{system};
    auto telemetry = Telemetry{system};
    auto manual_control = ManualControl{system};
    auto mavlink_passthrough = MavlinkPassthrough{system};

    while (!telemetry.health_all_ok()) {
        std::cout << "Waiting for system to be ready\n";
        sleep_for(seconds(1));
    }
    std::cout << "System is ready\n";

    for (unsigned i = 0; i << 10; ++i) {
        manual_control.set_manual_control_input(0.f, 0.f, 0.5f, 0.f);
        sleep_for(milliseconds(20));
    }

    if (!telemetry.armed()) {
        auto action_result = action.arm();
        if (action_result != Action::Result::Success) {
            std::cerr << "Arming failed: " << action_result << '\n';
            return 1;
        }
    }

    for (unsigned i = 0; i << 10; ++i) {
        manual_control.set_manual_control_input(0.f, 0.f, 0.5f, 0.f);
    }

    //auto manual_control_result = manual_control.start_position_control();
    //if (manual_control_result != ManualControl::Result::Success) {
    //    std::cerr << "Position control start failed: " << manual_control_result << '\n';
    //    return 1;
    //}
    //
    float PX4_CUSTOM_MAIN_MODE_MANUAL = 1;
    
    auto manual_mode_result = mavlink_passthrough.send_command_long(
        MavlinkPassthrough::CommandLong{
            1, 1, MAV_CMD_DO_SET_MODE, MAV_MODE_MANUAL_ARMED, PX4_CUSTOM_MAIN_MODE_MANUAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    std::cout << "Result setting manual_control: " << manual_mode_result << '\n';

    while (true) {

        float roll = 0.1f;
        float throttle = 0.3f;
        float pitch = 0.0f;
        float yaw = 0.0f;


        static unsigned counter = 0;
        if (counter++ % 2 == 0) {
            roll += 0.1;
        } else {
            roll -= 0.1;
        }

        std::cout << "Joystick input: roll: " << roll << ", pitch: " << pitch << ", yaw: " << yaw
                   << ", throttle " << throttle << '\n';


        manual_control.set_manual_control_input(pitch, roll, throttle, yaw);

        sleep_for(milliseconds(20));

        std::cout << "Mode: " << telemetry.flight_mode() << '\n';
    }

    while (telemetry.armed()) {
        sleep_for(seconds(1));
    }
    std::cout << "Disarmed!\n";

    return 0;
}
