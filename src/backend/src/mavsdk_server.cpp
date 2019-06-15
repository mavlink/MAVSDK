#include "backend_api.h"
#include <iostream>
#include <string>

static void usage();

static auto constexpr default_connection = "udp://:14540";

int main(int argc, char **argv)
{
    if (argc > 2) {
        usage();
        return 1;
    }

    if (argc == 2) {
        const std::string positional_arg = argv[1];
        if (positional_arg.compare("-h") == 0 || positional_arg.compare("--help") == 0) {
            usage();
            return 0;
        }
        runBackend(positional_arg.c_str(), nullptr, nullptr);
    }

    runBackend(default_connection, nullptr, nullptr);
}

void usage()
{
    std::cout << "Usage: backend_bin [-h | --help]" << std::endl
              << "       backend_bin [Connection URL]" << std::endl
              << std::endl
              << "Connection URL format should be:" << std::endl
              << "  Serial: serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "  UDP:    udp://[bind_host][:bind_port]" << std::endl
              << "  TCP:    tcp://[server_host][:server_port]" << std::endl
              << std::endl
              << "For example to connect to SITL use: udp://:14540" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "  -h | --help : show this help" << std::endl;
}
