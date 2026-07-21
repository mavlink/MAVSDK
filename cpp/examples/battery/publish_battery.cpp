//
// Simple example to demonstrate how to imitate a smart battery.
//
// Author: Julian Oes <julian@oes.ch>

#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;

static void subscribe_armed(Telemetry& telemetry);
static void send_battery_status(MavlinkDirect& mavlink_direct);

void usage(const std::string& bin_name)
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

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    // We identify ourselves as a battery component MAV_COMP_ID_BATTERY.
    // To match the autopilot we use sysid 1 which is the default, so likely
    // matching. If other sysids are used, we'd have to adapt this.
    Mavsdk mavsdk{Mavsdk::Configuration{1, 180, false}};
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
    auto mavlink_direct = MavlinkDirect{system.value()};

    subscribe_armed(telemetry);

    while (true) {
        send_battery_status(mavlink_direct);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

void subscribe_armed(Telemetry& telemetry)
{
    telemetry.subscribe_armed(
        [](bool is_armed) { std::cout << (is_armed ? "armed" : "disarmed") << '\n'; });
}

void send_battery_status(MavlinkDirect& mavlink_direct)
{
    MavlinkDirect::MavlinkMessage msg{};
    msg.message_name = "BATTERY_STATUS";

    // Two cells at 3700 mV and 3600 mV; remaining slots are 65535 (not used).
    // Enum values: MAV_BATTERY_FUNCTION_ALL=1, MAV_BATTERY_TYPE_LION=3,
    //              MAV_BATTERY_CHARGE_STATE_OK=1, MAV_BATTERY_MODE_UNKNOWN=0
    msg.fields_json = R"({
        "id": 0,
        "battery_function": 1,
        "type": 3,
        "temperature": 2500,
        "voltages": [3700, 3600, 65535, 65535, 65535, 65535, 65535, 65535, 65535, 65535],
        "current_battery": 4000,
        "current_consumed": 1000,
        "energy_consumed": -1,
        "battery_remaining": 80,
        "time_remaining": 3600,
        "charge_state": 1,
        "voltages_ext": [0, 0, 0, 0],
        "mode": 0,
        "fault_bitmask": 0
    })";

    mavlink_direct.send_message(msg);
}
