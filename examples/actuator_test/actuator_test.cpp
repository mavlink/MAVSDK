//
// Example that demonstrates how to do an actuator test
//

#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <thread>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/param/param.h>

using namespace mavsdk;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

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

void set_servo(MavlinkPassthrough& mavlink_passthrough, float servo1)
{
    MavlinkPassthrough::CommandLong command{};
    command.command = MAV_CMD_ACTUATOR_TEST;
    command.param1 = servo1;
    command.param2 = 1.0f;
    command.param5 = ACTUATOR_OUTPUT_FUNCTION_SERVO1;
    command.target_sysid = mavlink_passthrough.get_target_sysid();
    command.target_compid = mavlink_passthrough.get_target_compid();

    mavlink_passthrough.send_command_long(command);
}

void set_motor(MavlinkPassthrough& mavlink_passthrough, float motor1)
{
    MavlinkPassthrough::CommandLong command{};
    command.command = MAV_CMD_ACTUATOR_TEST;
    command.param1 = motor1;
    command.param2 = 1.0f;
    command.param5 = ACTUATOR_OUTPUT_FUNCTION_MOTOR1;
    command.target_sysid = mavlink_passthrough.get_target_sysid();
    command.target_compid = mavlink_passthrough.get_target_compid();

    mavlink_passthrough.send_command_long(command);
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

    auto system = get_system(mavsdk);
    if (!system) {
        return 1;
    }

    // Instantiate plugins.
    auto action = Action{system};
    auto mavlink_passthrough = MavlinkPassthrough{system};

    // Excite servo a bit
    sleep_for(seconds(1));

    std::cout << "Move servo 1..." << std::flush;

    for (int i = -100; i < 100; ++i) {
        set_servo(mavlink_passthrough, 0.01f * i);
        sleep_for(milliseconds(10));
    }

    set_servo(mavlink_passthrough, 0.0f);

    std::cout << "done.\n";

    sleep_for(seconds(1));

    std::cout << "Run motor 1..." << std::flush;

    // Switch motor on and off
    set_motor(mavlink_passthrough, 1.0f);
    sleep_for(seconds(1));
    set_motor(mavlink_passthrough, -1.0f);
    sleep_for(seconds(1));

    std::cout << "done.\n";

    return 0;
}
