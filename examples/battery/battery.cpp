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

using namespace mavsdk;

static void subscribe_armed(std::shared_ptr<Telemetry> telemetry);
static void send_battery_status(std::shared_ptr<MavlinkPassthrough> mavlink_passthrough);
static void usage(const std::string& bin_name);

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
        std::cout << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    std::promise<void> prom;
    std::future<void> fut = prom.get_future();
    std::cout << "Waiting to discover system..." << std::endl;
    mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
        const auto system = mavsdk.systems().at(0);

        if (system->is_connected()) {
            prom.set_value();
        }
    });

    if (fut.wait_for(std::chrono::seconds(2)) != std::future_status::ready) {
        std::cout << "No device found, exiting." << std::endl;
        return 1;
    }

    auto system = mavsdk.systems().at(0);

    auto telemetry = std::make_shared<Telemetry>(system);
    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);

    subscribe_armed(telemetry);

    while (true) {
        send_battery_status(mavlink_passthrough);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

void subscribe_armed(std::shared_ptr<Telemetry> telemetry)
{
    telemetry->subscribe_armed(
        [](bool is_armed) { std::cout << (is_armed ? "armed" : "disarmed") << std::endl; });
}

void send_battery_status(std::shared_ptr<MavlinkPassthrough> mavlink_passthrough)
{
    const uint16_t voltages[10]{
        3700,
        3600,
        UINT16_MAX,
        UINT16_MAX,
        UINT16_MAX,
        UINT16_MAX,
        UINT16_MAX,
        UINT16_MAX,
        UINT16_MAX,
        UINT16_MAX}; // mV

    const uint16_t voltages_ext[4]{0, 0, 0, 0};

    mavlink_message_t message;
    mavlink_msg_battery_status_pack(
        mavlink_passthrough->get_our_sysid(),
        mavlink_passthrough->get_our_compid(),
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

    mavlink_passthrough->send_message(message);
}

void usage(const std::string& bin_name)
{
    std::cout << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}
