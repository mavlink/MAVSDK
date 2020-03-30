/**
 * @file mavlink_ftpd.cpp
 *
 * @brief Demonstrates how to use Mavlink FTP server with the MAVSDK.
 *
 * @author Matej Frančeškin <matej@auterion.com>,
 * @date 2019-09-06
 */

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_ftp/mavlink_ftp.h>

#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cout
        << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <remote_ip> <remote_port> <root_dir>"
        << std::endl
        << "Start mavlink FTP server on <root_dir> sending heartbeats to <remote_ip>:<remote_port>"
        << std::endl;
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
    if (connection_result != ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Error setting up Mavlink FTP server." << std::endl;
        return 1;
    }
    System& system_cc = mavsdk.system();

    auto mavlink_ftp_server = std::make_shared<MavlinkFTP>(system_cc);
    mavlink_ftp_server->set_root_dir(argv[3]);

    std::cout << NORMAL_CONSOLE_TEXT << "Mavlink FTP server running." << std::endl
              << "Remote:       " << argv[1] << ":" << argv[2] << std::endl
              << "Component ID: " << static_cast<int>(mavlink_ftp_server->get_our_compid())
              << std::endl;

    for (;;) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
