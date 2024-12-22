//
// Simple example to demonstrate how to get and set params.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/param/param.h>

#include <charconv>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <iostream>
#include <future>
#include <utility>
#include <string>

std::from_chars_result our_from_chars_float(const char* first, const char* last, float& value);

using namespace mavsdk;

void get_all(Param& param);
void get(Param& param, std::string name);
void set(Param& param, std::string name, std::string value);

class CommandLineParser {
public:
    static void print_usage(const std::string& bin_name)
    {
        std::cerr << "Usage :" << bin_name << " <connection url> <action> [args]\n"
                  << "\n"
                  << "Connection URL format should be :\n"
                  << "  For TCP : tcp://[server_host][:server_port]\n"
                  << "  For UDP : udp://[bind_host][:bind_port]\n"
                  << "  For Serial : serial:///path/to/serial/dev[:baudrate]\n"
                  << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n"
                  << "\n"
                  << "Actions:\n"
                  << "  get_all:          Print all parameters\n"
                  << "  get <param name>: Get one param\n"
                  << "  set <param name> <value>: Set one param\n"
                  << std::flush;
    }

    enum class Result {
        Ok,
        Invalid,
    };

    enum class Action {
        GetAll,
        Get,
        Set,
    };

    struct Args {
        std::string connection;
        Action action;
        std::string param_name; // Empty for GetAll
        std::string value; // Only used for Set
    };

    std::pair<Result, Args> parse(int argc, char** argv)
    {
        if (argc < 3) { // Need at least program_name, url, and action
            std::cerr << "Insufficient arguments" << std::endl;
            return {Result::Invalid, {}};
        }

        Args args{};
        args.connection = argv[1];

        std::string action_str = argv[2];

        if (action_str == "get_all") {
            if (argc != 3) {
                std::cerr << "get_all takes no additional arguments" << std::endl;
                return {Result::Invalid, {}};
            }
            args.action = Action::GetAll;
        } else if (action_str == "get") {
            if (argc != 4) {
                std::cerr << "get requires a parameter name" << std::endl;
                return {Result::Invalid, {}};
            }
            args.action = Action::Get;
            args.param_name = argv[3];
        } else if (action_str == "set") {
            if (argc != 5) {
                std::cerr << "set requires a parameter name and value" << std::endl;
                return {Result::Invalid, {}};
            }

            args.action = Action::Set;
            args.param_name = argv[3];
            args.value = argv[4];
        } else {
            std::cerr << "Unknown action: " << action_str << std::endl;
            return {Result::Invalid, {}};
        }
        return {Result::Ok, args};
    }
};

int main(int argc, char** argv)
{
    CommandLineParser parser;
    auto result = parser.parse(argc, argv);
    switch (result.first) {
        case CommandLineParser::Result::Invalid:
            CommandLineParser::print_usage(argv[0]);
            return 1;

        case CommandLineParser::Result::Ok:
            break;
    }

    const auto& args = result.second;

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    const ConnectionResult connection_result = mavsdk.add_any_connection(args.connection);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    std::cout << "Waiting to discover system...\n";
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    // We wait for new systems to be discovered, once we find one that has an
    // autopilot, we decide to use it.
    Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
        auto system = mavsdk.systems().back();

        if (system->has_autopilot()) {
            std::cout << "Discovered autopilot\n";

            // Unsubscribe again as we only want to find one system.
            mavsdk.unsubscribe_on_new_system(handle);
            prom.set_value(system);
        }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds max, surely.
    if (fut.wait_for(std::chrono::seconds(3)) == std::future_status::timeout) {
        std::cerr << "No autopilot found, exiting.\n";
        return 1;
    }

    // Get discovered system now.
    auto system = fut.get();

    // Instantiate plugins.
    auto param = Param{system};

    switch (args.action) {
        case CommandLineParser::Action::GetAll:
            get_all(param);
            break;

        case CommandLineParser::Action::Get:
            get(param, args.param_name);
            break;

        case CommandLineParser::Action::Set:
            set(param, args.param_name, args.value);
            break;
    };

    return 0;
}

void get_all(Param& param)
{
    // Print params once.
    const auto result = param.get_all_params();

    std::cout << "Int params: \n";
    for (auto p : result.int_params) {
        std::cout << p.name << ": " << p.value << '\n';
    }

    std::cout << "Float params: \n";
    for (auto p : result.float_params) {
        std::cout << p.name << ": " << p.value << '\n';
    }
}

void get(Param& param, std::string name)
{
    // This is not very pretty, but it does seem to work."
    std::cerr << "Try as float param..." << std::flush;
    auto result = param.get_param_float(name);

    if (result.first == Param::Result::Success) {
        std::cerr << "Ok" << std::endl;
        std::cout << name << ": " << result.second << '\n';
        return;
    }

    if (result.first != Param::Result::WrongType) {
        std::cerr << "Failed: " << result.first << std::endl;
        return;
    }

    std::cout << "Wrong type" << std::endl;
    std::cerr << "Try as int param next..." << std::flush;
    result = param.get_param_int(name);

    if (result.first == Param::Result::Success) {
        std::cerr << "Ok" << std::endl;
        std::cout << name << ": " << result.second << '\n';
        return;
    }

    if (result.first != Param::Result::WrongType) {
        std::cerr << "Failed: " << result.first << std::endl;
        return;
    }

    std::cerr << "Failed: " << result.first << std::endl;
    std::cerr << "Neither int or float worked, should maybe try custom? (not implemented)"
              << std::endl;
}

void set(Param& param, std::string name, std::string value)
{
    // Assume integer first
    int int_value;
    const auto int_result = std::from_chars(value.data(), value.data() + value.size(), int_value);
    if (int_result.ec == std::errc() && int_result.ptr == value.data() + value.size()) {
        std::cerr << "Setting " << value << " as integer..." << std::flush;

        auto result = param.set_param_int(name, int_value);

        if (result == Param::Result::Success) {
            std::cerr << "Ok" << std::endl;
            return;
        }

        if (result != Param::Result::WrongType) {
            std::cerr << "Failed: " << result << std::endl;
            return;
        }

        // If we got the type wrong, we try as float next.
    }

    // Try float if integer failed
    float float_value;
    const auto float_result =
        our_from_chars_float(value.data(), value.data() + value.size(), float_value);
    if (float_result.ec == std::errc() && float_result.ptr == value.data() + value.size()) {
        std::cerr << "Setting " << value << " as float..." << std::flush;

        auto result = param.set_param_float(name, float_value);

        if (result == Param::Result::Success) {
            std::cerr << "Ok" << std::endl;
            return;
        }

        if (result != Param::Result::WrongType) {
            std::cerr << "Failed: " << result << std::endl;
            return;
        }

        std::cerr << "Failed: " << result << std::endl;
    }

    std::cerr << "Neither int or float worked, should maybe try custom? (not implemented)"
              << std::endl;
}

// GCC 9/10 don't have std::from_chars for float yet, so we have to have our own (simplified) one.
std::from_chars_result our_from_chars_float(const char* first, const char* last, float& value)
{
    float parsed_value;
    int chars_read;

    // sscanf returns number of successfully parsed items (should be 1)
    // %n stores the number of characters read
    if (sscanf(first, "%f%n", &parsed_value, &chars_read) == 1) {
        if (first + chars_read <= last) {
            value = parsed_value;
            return {first + chars_read, std::errc{}};
        }
    }

    return {first, std::errc::invalid_argument};
}
