//
// Example how to use an FTP server with MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/ftp/ftp.h>

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

    auto system_cc = mavsdk.systems().at(0);

    auto ftp_server = Ftp{system_cc};
    ftp_server.set_root_directory(argv[3]);

    std::cout << "Mavlink FTP server running.\n"
              << '\n'
              << "Remote:       " << argv[1] << ":" << argv[2] << '\n'
              << "Component ID: " << static_cast<int>(ftp_server.get_our_compid()) << '\n';

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
