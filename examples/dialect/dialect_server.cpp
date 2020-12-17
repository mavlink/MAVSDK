#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#include <memory>

using namespace mavsdk;

static void send_current_mood(MavlinkPassthrough& mavlink_passthrough);
static void usage(const std::string& bin_name);

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
    Mavsdk::Configuration configuration(Mavsdk::Configuration::UsageType::CompanionComputer);
    mavsdk.set_configuration(configuration);
    ConnectionResult connection_result = mavsdk.setup_udp_remote(argv[1], std::stoi(argv[2]));

    if (connection_result != ConnectionResult::Success) {
        std::cout << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    auto system = mavsdk.systems().at(0);
    auto mavlink_passthrough = MavlinkPassthrough{system};

    std::cout << "Starting to broadcast mood to " << argv[1] << ":" << argv[2] << std::endl;

    while (true) {
        std::cout << "Sending mood" << std::endl;
        send_current_mood(mavlink_passthrough);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

void send_current_mood(MavlinkPassthrough& mavlink_passthrough)
{
    mavlink_message_t message;
    mavlink_msg_current_mood_pack(
        mavlink_passthrough.get_our_sysid(),
        mavlink_passthrough.get_our_compid(),
        &message,
        HAPPY
        );

    mavlink_passthrough.send_message(message);
}

void usage(const std::string& bin_name)
{
    std::cout << "Usage : " << bin_name << " <remote_ip> <remote_port>" << std::endl;
}
