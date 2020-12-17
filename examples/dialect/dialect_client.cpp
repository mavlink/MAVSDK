#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>

using namespace mavsdk;

static void usage(const std::string& bin_name);
static std::string mood2str(const uint8_t mood_enum_value);

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
    fut.wait();

    auto system = mavsdk.systems().at(0);
    auto mavlink_passthrough = MavlinkPassthrough{system};

    mavlink_passthrough.subscribe_message_async(MAVLINK_MSG_ID_CURRENT_MOOD, [](const mavlink_message_t& message) {
        mavlink_current_mood_t current_mood;
        mavlink_msg_current_mood_decode(&message, &current_mood);

        std::cout << "Current mood is: " << mood2str(current_mood.mood) << std::endl;
    });

    std::promise<void> termination_prom;
    auto termination_fut = termination_prom.get_future();
    termination_fut.wait();

    return 0;
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

std::string mood2str(const uint8_t mood_enum_value)
{
    switch (mood_enum_value) {
        case 1:
            return "Happy";
        case 2:
            return "Sad";
        case 3:
        default:
            return "Not sure";
    }
}
