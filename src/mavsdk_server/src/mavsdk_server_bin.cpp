#include "mavsdk_server_api.h"

#include <cctype>
#include <iostream>
#include <limits>
#include <string>

static auto constexpr default_connection = "udp://:14540";
static auto constexpr default_mavsdk_server_port = 50051;
static auto constexpr default_sysid = 245;
static auto constexpr default_compid = 190;

static void usage(const char* bin_name);
static bool is_integer(const std::string& tested_integer);

int main(int argc, char** argv)
{
    std::string connection_url = default_connection;
    int mavsdk_server_port = default_mavsdk_server_port;
    int mavsdk_sysid = default_sysid;
    int mavsdk_compid = default_compid;

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
        } else if (current_arg == "--sysid") {
            if (argc <= i + 1) {
                usage(argv[0]);
                return 1;
            }

            const std::string sysid(argv[i + 1]);
            i++;

            if (!is_integer(sysid)) {
                usage(argv[0]);
                return 1;
            }

            mavsdk_sysid = std::stoi(sysid);

            if (mavsdk_sysid > std::numeric_limits<uint8_t>::max() || mavsdk_sysid < 1) {
                usage(argv[0]);
                return 1;
            }
        } else if (current_arg == "--compid") {
            if (argc <= i + 1) {
                usage(argv[0]);
                return 1;
            }

            const std::string compid(argv[i + 1]);
            i++;

            if (!is_integer(compid)) {
                usage(argv[0]);
                return 1;
            }

            mavsdk_compid = std::stoi(compid);

            if (mavsdk_compid > std::numeric_limits<uint8_t>::max() || mavsdk_compid < 1) {
                usage(argv[0]);
                return 1;
            }
        } else {
            connection_url = current_arg;
        }
    }

    MavsdkServer* mavsdk_server;
    mavsdk_server_init(&mavsdk_server);
    const auto is_started = mavsdk_server_run_with_mavlink_ids(
        mavsdk_server,
        connection_url.c_str(),
        mavsdk_server_port,
        static_cast<uint8_t>(mavsdk_sysid),
        static_cast<uint8_t>(mavsdk_compid));

    if (!is_started) {
        std::cout << "Failed to start, exiting...\n";
        mavsdk_server_destroy(mavsdk_server);
        return 1;
    }

    mavsdk_server_attach(mavsdk_server);

    mavsdk_server_destroy(mavsdk_server);
    return 0;
}

void usage(const char* bin_name)
{
    std::cout << "Usage: " << bin_name << " [Options] [Connection URL]" << '\n'
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
              << "  -p          : set the port on which to run the gRPC server,\n"
              << "                set to 0 to choose a free port automatically\n"
              << "                (default is " << default_mavsdk_server_port << ")\n"
              << "  --sysid     : set the MAVLink system ID of the MAVSDK server itself,\n"
              << "                (default is " << default_sysid << ", range 1..255)\n"
              << "  --compid    : set the MAVLink component ID of the MAVSDK server itself,\n"
              << "                (default is " << default_compid << ", range 1..255)\n";
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
