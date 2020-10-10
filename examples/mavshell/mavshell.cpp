#include <future>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/shell/shell.h>
#include <iostream>

using namespace mavsdk;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

void print_usage(const std::string& bin_name);
bool start_discovery(Mavsdk& mavsdk, const std::string& connection_url);
void wait_until_discover(Mavsdk& mavsdk);
void run_interactive_shell(Mavsdk& mavsdk);

int main(int argc, char** argv)
{
    if (argc != 2) {
        const auto binary_name = argv[0];
        print_usage(binary_name);
        return 1;
    }

    Mavsdk mavsdk;
    const std::string connection_url(argv[1]);

    if (!start_discovery(mavsdk, connection_url)) {
        return 1;
    }

    wait_until_discover(mavsdk);
    run_interactive_shell(mavsdk);

    return 0;
}

void print_usage(const std::string& bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}

bool start_discovery(Mavsdk& mavsdk, const std::string& connection_url)
{
    const auto connection_result = mavsdk.add_any_connection(connection_url);

    if (connection_result != ConnectionResult::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Connection failed: " << connection_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return false;
    }

    return true;
}

void wait_until_discover(Mavsdk& mavsdk)
{
    std::cout << "Waiting to discover system..." << std::endl;
    std::promise<void> discover_promise;
    auto discover_future = discover_promise.get_future();

    mavsdk.subscribe_on_change([&mavsdk, &discover_promise]() {
        const auto system = mavsdk.systems().at(0);

        if (system->is_connected()) {
            std::cout << "Discovered system" << std::endl;
            discover_promise.set_value();
        }
    });

    discover_future.wait();
}

void run_interactive_shell(Mavsdk& mavsdk)
{
    Shell shell(mavsdk.systems().at(0));

    shell.subscribe_receive([](const std::string output) { std::cout << output; });

    while (true) {
        std::string command;
        getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        shell.send(command);
    }
    std::cout << std::endl;
}
