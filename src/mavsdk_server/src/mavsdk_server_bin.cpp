#include "mavsdk_server_api.h"

#include <cctype>
#include <iostream>
#include <string>

static auto constexpr default_connection = "udp://:14540";
static auto constexpr default_mavsdk_server_port = 50051;

static void usage(const char* bin_name);
static bool is_integer(const std::string& tested_integer);

int main(int argc, char** argv)
{
    std::string connection_url = default_connection;
    int mavsdk_server_port = default_mavsdk_server_port;

    for (int i = 1; i < argc; i++) {
        const std::string current_arg = argv[i];

        if (current_arg == "-h" || current_arg == "--help") {
            usage(argv[0]);
            return 0;
        } else if (current_arg == "-p") {
            if (argc <= i + 1) {
                usage(argv[0]);
                return 1;
            }

            const std::string port(argv[i + 1]);
            i++;

            if (!is_integer(port)) {
                usage(argv[0]);
                return 1;
            }

            mavsdk_server_port = std::stoi(port);
        } else {
            connection_url = current_arg;
        }
    }

    auto mavsdk_server = mavsdk_server_run(connection_url.c_str(), mavsdk_server_port);
    mavsdk_server_attach(mavsdk_server);
}

void usage(const char* bin_name)
{
    std::cout << "Usage: " << bin_name << " [-h | --help]" << '\n'
              << "       " << bin_name << " [-p mavsdk_server_port] [Connection URL]" << '\n'
              << '\n'
              << "Connection URL format should be:" << '\n'
              << "  Serial: serial:///path/to/serial/dev[:baudrate]" << '\n'
              << "  UDP:    udp://[bind_host][:bind_port]" << '\n'
              << "  TCP:    tcp://[server_host][:server_port]" << '\n'
              << '\n'
              << "For example to connect to SITL use: udp://:14540" << '\n'
              << '\n'
              << "Options:" << '\n'
              << "  -h | --help : show this help" << '\n'
              << "  -p          : set the port on which to run the gRPC server\n"
              << "                (default is " << default_mavsdk_server_port << ")\n";
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
