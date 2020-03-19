#include <future>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/shell/shell.h>
#include <iostream>

using namespace mavsdk;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

bool are_arguments_valid(const int, char**);
void print_usage(const std::string& bin_name);
bool start_discovery(Mavsdk&, const std::string&);
void wait_until_discover(Mavsdk&);
void run_interactive_shell(Mavsdk&, const uint16_t);

int main(int argc, char** argv)
{
    if (!are_arguments_valid(argc, argv)) {
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
    run_interactive_shell(mavsdk, 1000);

    return 0;
}

bool are_arguments_valid(const int argc, char**)
{
    return argc == 2;
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

    if (connection_result != ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT
                  << "Connection failed: " << connection_result_str(connection_result)
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

    mavsdk.register_on_discover([&discover_promise](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        discover_promise.set_value();
    });

    discover_future.wait();
}

void run_interactive_shell(Mavsdk& mavsdk, const uint16_t timeout_ms)
{
    Shell shell(mavsdk.system());

    shell.shell_command_response_async([](const Shell::Result, const Shell::ShellMessage msg) {
        std::string output(msg.data);

        if (output.length() > 2 && output.substr(1, output.length() - 2) == "nsh>") {
            output = "nsh> ";
        }

        std::cout << output;
    });

    while (true) {
        std::string command;
        getline(std::cin, command);

        if (command == "exit") {
            break;
        }

        Shell::ShellMessage msg{true, timeout_ms, command};
        shell.shell_command(msg);
    }
}
