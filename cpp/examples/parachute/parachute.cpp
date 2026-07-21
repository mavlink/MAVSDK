//
// This is a simple example for how a parachute device could be implemented.
// Basically, this would be a component which has control over a parachute
// and would release it as soon as PX4 (or anyone else) tells it to do so.
//
// For the sake of this example there is nothing happening when a parachute
// should be triggered, all that happens is a log output.
//

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.hpp>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <limits>
#include <memory>
#include <optional>
#include <thread>

using namespace mavsdk;

// MAVLink constants (from MAVLink common.xml)
static constexpr uint8_t MAV_COMP_ID_PARACHUTE = 161;
static constexpr uint16_t MAV_CMD_DO_PARACHUTE = 208;
static constexpr int PARACHUTE_DISABLE = 0;
static constexpr int PARACHUTE_ENABLE = 1;
static constexpr int PARACHUTE_RELEASE = 2;

static void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
}

// Extract a float value from a flat JSON object string by field name.
// Returns quiet_NaN if the field value is JSON null.
static std::optional<float> json_float(const std::string& json, const std::string& key)
{
    const std::string pattern = '"' + key + '"';
    auto pos = json.find(pattern);
    if (pos == std::string::npos)
        return std::nullopt;
    pos = json.find(':', pos + pattern.size());
    if (pos == std::string::npos)
        return std::nullopt;
    pos = json.find_first_not_of(" \t\n\r", pos + 1);
    if (pos == std::string::npos)
        return std::nullopt;
    if (json.compare(pos, 4, "null") == 0)
        return std::numeric_limits<float>::quiet_NaN();
    try {
        std::size_t len;
        return std::stof(json.substr(pos), &len);
    } catch (...) {
        return std::nullopt;
    }
}

static void process_command_long(const MavlinkDirect::MavlinkMessage& message, uint8_t our_sysid)
{
    const auto command = json_float(message.fields_json, "command");
    if (!command || std::lround(*command) != MAV_CMD_DO_PARACHUTE) {
        return;
    }

    // target_system_id and target_component_id are pre-parsed from the message fields.
    if (message.target_system_id != 0 &&
        static_cast<uint8_t>(message.target_system_id) != our_sysid) {
        std::cout << "Received parachute command_long, but not for us (wrong target_system).\n";
        return;
    }

    if (message.target_component_id != 0 &&
        static_cast<uint8_t>(message.target_component_id) != MAV_COMP_ID_PARACHUTE) {
        std::cout << "Received parachute command_long, but not for us (wrong target_component).\n";
        return;
    }

    const auto param1 = json_float(message.fields_json, "param1");
    if (!param1)
        return;

    const int action = std::lround(*param1);
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
            std::cerr << "Unknown parachute action (" << *param1 << ")\n";
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
    auto mavlink_direct = MavlinkDirect{system};

    mavlink_direct.subscribe_message(
        "COMMAND_LONG", [&our_sysid](const MavlinkDirect::MavlinkMessage& message) {
            process_command_long(message, our_sysid);
        });

    std::cout << "Waiting for parachute commands\n";
    // Wait for parachute command.
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
