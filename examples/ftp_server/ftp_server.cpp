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
    std::cerr << "Usage : " << bin_name << " <connection> <root_dir>" << std::endl;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::CompanionComputer}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Error setting up Mavlink FTP server.\n";
        return 1;
    }

    auto component = mavsdk.server_component();
    auto ftp_server = FtpServer{component};
    ftp_server.set_root_dir(argv[2]);

    std::cout << "MAVLink FTP server running\n"
              << "  connection: " << argv[1] << '\n'
              << "  directory: " << argv[2] << '\n'
              << "  component ID: " << std::to_string(component->component_id()) << std::endl;

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
