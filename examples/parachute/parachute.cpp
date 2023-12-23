//
// This is a simple example for how a parachute device could be implemented.
// Basically, this would be a component which has control over a parachute
// and would release it as soon as PX4 (or anyone else) tells it to do so.
//
// For the sake of this example there is nothing happening when a parachute
// should be triggered, all that happens is a log output.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;

static void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

static void process_command_long(const mavlink_message_t& message, uint8_t our_sysid)
{
    mavlink_command_long_t command_long;
    mavlink_msg_command_long_decode(&message, &command_long);

    // Only listen to parachute commands.
    if (command_long.command != MAV_CMD_DO_PARACHUTE) {
        return;
    }

    // Check if it's meant for us.
    if (command_long.target_system != 0 && command_long.target_system != our_sysid) {
        std::cout << "Received parachute command_long, but not for us (wrong target_system).\n";
        return;
    }

    if (command_long.target_component != 0 &&
        command_long.target_component != MAV_COMP_ID_PARACHUTE) {
        std::cout << "Received parachute command_long, but not for us (wrong target_component).\n";
        return;
    }

    const int action = std::lround(command_long.param1);
    switch (action) {
        case PARACHUTE_DISABLE:
            // Actual implementation would go here.
            std::cout << "Parachute disabled!\n";
            break;
        case PARACHUTE_ENABLE:
            // Actual implementation would go here.
            std::cout << "Parachute enabled!\n";
            break;
        case PARACHUTE_RELEASE:
            // Actual implementation would go here.
            std::cout << "Parachute release!\n";
            break;
        default:
            std::cerr << "Unknown parachute action (" << command_long.param1 << ")\n";
            break;
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    const std::string connection_url = argv[1];

    // We start with sysid 1 but adapt to the one of the autopilot once
    // we have discoverd it.
    uint8_t our_sysid = 1;

    Mavsdk mavsdk{Mavsdk::Configuration{our_sysid, MAV_COMP_ID_PARACHUTE, false}};

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
    Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
        auto system = mavsdk.systems().back();

        if (system->has_autopilot()) {
            std::cout << "Discovered autopilot\n";

            // Unsubscribe again as we only want to find one system.
            mavsdk.unsubscribe_on_new_system(handle);
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

    // Update system ID if required.
    if (system->get_system_id() != our_sysid) {
        our_sysid = system->get_system_id();
        mavsdk.set_configuration(Mavsdk::Configuration{our_sysid, MAV_COMP_ID_PARACHUTE, false});
    }

    // Instantiate plugins.
    auto mavlink_passthrough = MavlinkPassthrough{system};

    mavlink_passthrough.subscribe_message(
        MAVLINK_MSG_ID_COMMAND_LONG, [&our_sysid](const mavlink_message_t& message) {
            process_command_long(message, our_sysid);
        });

    std::cout << "Waiting for parachute commands\n";
    // Wait for parachute command.
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
