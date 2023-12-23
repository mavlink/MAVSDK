//
// Simple example to demonstrate how to imitate a smart battery.
//
// Author: Julian Oes <julian@oes.ch>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;

static void subscribe_armed(Telemetry& telemetry);
static void send_battery_status(MavlinkPassthrough& mavlink_passthrough);

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

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
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
    auto mavlink_passthrough = MavlinkPassthrough{system.value()};

    subscribe_armed(telemetry);

    while (true) {
        send_battery_status(mavlink_passthrough);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

void subscribe_armed(Telemetry& telemetry)
{
    telemetry.subscribe_armed(
        [](bool is_armed) { std::cout << (is_armed ? "armed" : "disarmed") << '\n'; });
}

void send_battery_status(MavlinkPassthrough& mavlink_passthrough)
{
    const uint16_t voltages[10]{
        3700,
        3600,
        std::numeric_limits<uint16_t>::max(),
        std::numeric_limits<uint16_t>::max(),
        std::numeric_limits<uint16_t>::max(),
        std::numeric_limits<uint16_t>::max(),
        std::numeric_limits<uint16_t>::max(),
        std::numeric_limits<uint16_t>::max(),
        std::numeric_limits<uint16_t>::max(),
        std::numeric_limits<uint16_t>::max()}; // mV

    const uint16_t voltages_ext[4]{0, 0, 0, 0};

    mavlink_passthrough.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_battery_status_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            0, // id
            MAV_BATTERY_FUNCTION_ALL, // battery_function
            MAV_BATTERY_TYPE_LION, // type
            2500, // 100*temperature C
            &voltages[0],
            4000, // 100*current_battery A
            1000, // current_consumed, mAh
            -1, // energy consumed hJ
            80, // battery_remaining %
            3600, // time_remaining
            MAV_BATTERY_CHARGE_STATE_OK,
            voltages_ext,
            MAV_BATTERY_MODE_UNKNOWN, // mode
            0); // fault_bitmask
        return message;
    });
}
