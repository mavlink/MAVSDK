#include "backend_api.h"

#include <cctype>
#include <iostream>
#include <string>

static auto constexpr default_connection = "udp://:14540";
static auto default_mavsdk_server_port = 0;

static void usage();
static bool is_integer(const std::string& tested_integer);

int main(int argc, char** argv)
{
    std::string connection_url = default_connection;
    int mavsdk_server_port = default_mavsdk_server_port;

    for (int i = 1; i < argc; i++) {
        const std::string current_arg = argv[i];

        if (current_arg == "-h" || current_arg == "--help") {
            usage();
            return 0;
        } else if (current_arg == "-p") {
            if (argc <= i + 1) {
                usage();
                return 1;
            }

            const std::string port(argv[i + 1]);
            i++;

            if (!is_integer(port)) {
                usage();
                return 1;
            }

            mavsdk_server_port = std::stoi(port);
        } else {
            connection_url = current_arg;
        }
    }

    runBackend(connection_url.c_str(), mavsdk_server_port, nullptr, nullptr);
}

void usage()
{
    std::cout << "Usage: backend_bin [-h | --help]" << std::endl
              << "       backend_bin [-p mavsdk_server_port] [Connection URL]" << std::endl
              << std::endl
              << "Connection URL format should be:" << std::endl
              << "  Serial: serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "  UDP:    udp://[bind_host][:bind_port]" << std::endl
              << "  TCP:    tcp://[server_host][:server_port]" << std::endl
              << std::endl
              << "For example to connect to SITL use: udp://:14540" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "  -h | --help : show this help" << std::endl
              << "  -p          : set the port on which to run the gRPC server" << std::endl;
}

bool is_integer(const std::string& tested_integer)
{
    for (const auto& digit : tested_integer) {
        if (!std::isdigit(digit)) {
            return false;
        }
    }

    return true;
}
