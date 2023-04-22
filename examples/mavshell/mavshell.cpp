#include <future>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/shell/shell.h>
#include <iostream>

using namespace mavsdk;
using std::chrono::seconds;

void run_interactive_shell(std::shared_ptr<System> system);

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    // Instantiate plugins.
    run_interactive_shell(system.value());

    return 0;
}

void run_interactive_shell(std::shared_ptr<System> system)
{
    Shell shell{system};

    shell.subscribe_receive([](const std::string output) { std::cout << output; });

    while (true) {
        std::string command;
        getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        shell.send(command);
    }
    std::cout << '\n';
}
