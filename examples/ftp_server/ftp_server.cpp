//
// Example how to use an FTP server with MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/ftp_server/ftp_server.h>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <remote_ip> <remote_port> <root_dir>\n"
              << '\n'
              << "    Start mavlink FTP server on <root_dir>\n"
              << "    sending heartbeats to <remote_ip>:<remote_port>\n";
}

int main(int argc, char** argv)
{
    if (argc != 4) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
    Mavsdk::Configuration configuration(Mavsdk::Configuration::UsageType::CompanionComputer);
    mavsdk.set_configuration(configuration);
    ConnectionResult connection_result = mavsdk.setup_udp_remote(argv[1], std::stoi(argv[2]));
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Error setting up Mavlink FTP server.\n";
        return 1;
    }

    auto component =
        mavsdk.server_component_by_type(mavsdk::Mavsdk::ServerComponentType::CompanionComputer);
    auto ftp_server = FtpServer{component};
    ftp_server.set_root_dir(argv[3]);

    std::cout << "Mavlink FTP server running.\n"
              << '\n'
              << "Remote:       " << argv[1] << ":" << argv[2] << '\n'
              << "Component ID: " << std::to_string(component->component_id()) << '\n';

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
