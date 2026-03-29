//
// Example how to use an FTP server with MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/ftp_server/ftp_server.h>

#include <chrono>
#include <format>
#include <iostream>
#include <string>
#include <thread>

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cerr << std::format("Usage : {} <connection> <root_dir>\n", bin_name);
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::CompanionComputer}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Error setting up Mavlink FTP server.\n";
        return 1;
    }

    auto component = mavsdk.server_component();
    auto ftp_server = FtpServer{component};
    ftp_server.set_root_dir(argv[2]);

    std::cout << std::format("MAVLink FTP server running\n  connection: {}\n  directory: {}\n  component ID: {}\n", argv[1], argv[2], std::to_string(component->component_id()));

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
