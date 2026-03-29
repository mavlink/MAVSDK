//
// Simple example to demonstrate how to get and set params.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/param/param.h>

#include <charconv>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <format>
#include <iostream>
#include <future>
#include <utility>
#include <string>

std::from_chars_result our_from_chars_float(const char* first, const char* last, float& value);

using namespace mavsdk;

// Parsed value that can be either int or float
struct ParsedValue {
    bool is_float;
    int int_val;
    float float_val;
};

// Parse value string, detecting type based on presence of '.'
bool parse_value(const std::string& value, ParsedValue& parsed);

// Set a parameter with type fallback, returns success and the type actually used
bool set_param_with_fallback(
    Param& param, const std::string& name, const ParsedValue& value, bool& used_float);

// Get a parameter value given the type
bool get_param_value(Param& param, const std::string& name, bool is_float, ParsedValue& result);

void get_all(Param& param);
void get(Param& param, std::string name);
void set(Param& param, std::string name, std::string value);
void set_and_verify(Param& param, std::string name, std::string value);

class CommandLineParser {
public:
    static void print_usage(const std::string& bin_name)
    {
        std::cerr << std::format("Usage :{} <connection url> <action> [args]\n\nConnection URL format should be :\n For TCP server: tcpin://<our_ip>:<port>\n For TCP client: tcpout://<remote_ip>:<port>\n For UDP server: udpin://<our_ip>:<port>\n For UDP client: udpout://<remote_ip>:<port>\n For Serial : serial://</path/to/serial/dev>:<baudrate>]\nFor example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n\nActions:\n  get_all:          Print all parameters\n  get <param name>: Get one param\n  set <param name> <value>: Set one param\n  set_and_verify <param name> <value>: Set param and verify by reading back\n", bin_name) << std::flush;
    }

    enum class Result {
        Ok,
        Invalid,
    };

    enum class Action {
        GetAll,
        Get,
        Set,
        SetAndVerify,
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
        } else if (action_str == "set_and_verify") {
            if (argc != 5) {
                std::cerr << "set_and_verify requires a parameter name and value" << std::endl;
                return {Result::Invalid, {}};
            }

            args.action = Action::SetAndVerify;
            args.param_name = argv[3];
            args.value = argv[4];
        } else {
            std::cerr << std::format("Unknown action: {}\n", action_str);
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
        std::cerr << std::format("Connection failed: {}\n", connection_result);
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

        case CommandLineParser::Action::SetAndVerify:
            set_and_verify(param, args.param_name, args.value);
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
        std::cout << std::format("{}: {}\n", p.name, p.value);
    }

    std::cout << "Float params: \n";
    for (auto p : result.float_params) {
        std::cout << std::format("{}: {}\n", p.name, p.value);
    }
}

void get(Param& param, std::string name)
{
    // This is not very pretty, but it does seem to work."
    std::cerr << std::format("Try as float param...") << std::flush;
    auto result = param.get_param_float(name);

    if (result.first == Param::Result::Success) {
        std::cerr << "Ok" << std::endl;
        std::cout << std::format("{}: {}\n", name, result.second);
        return;
    }

    if (result.first != Param::Result::WrongType) {
        std::cerr << std::format("Failed: {}\n", result.first);
        return;
    }

    std::cout << "Wrong type" << std::endl;
    std::cerr << std::format("Try as int param next...") << std::flush;
    result = param.get_param_int(name);

    if (result.first == Param::Result::Success) {
        std::cerr << "Ok" << std::endl;
        std::cout << std::format("{}: {}\n", name, result.second);
        return;
    }

    if (result.first != Param::Result::WrongType) {
        std::cerr << std::format("Failed: {}\n", result.first);
        return;
    }

    std::cerr << std::format("Failed: {}\n", result.first);
    std::cerr << "Neither int or float worked, should maybe try custom? (not implemented)"
              << std::endl;
}

bool parse_value(const std::string& value, ParsedValue& parsed)
{
    parsed.is_float = (value.find('.') != std::string::npos);

    std::cout << std::format("Detected type: {} (based on {} of '.')\n", (parsed.is_float ? "float" : "int"), (parsed.is_float ? "presence" : "absence"));

    if (parsed.is_float) {
        const auto result =
            our_from_chars_float(value.data(), value.data() + value.size(), parsed.float_val);
        if (result.ec != std::errc() || result.ptr != value.data() + value.size()) {
            std::cerr << std::format("Failed to parse '{}' as float\n", value);
            return false;
        }
        parsed.int_val = static_cast<int>(parsed.float_val);
    } else {
        const auto result =
            std::from_chars(value.data(), value.data() + value.size(), parsed.int_val);
        if (result.ec != std::errc() || result.ptr != value.data() + value.size()) {
            std::cerr << std::format("Failed to parse '{}' as int\n", value);
            return false;
        }
        parsed.float_val = static_cast<float>(parsed.int_val);
    }
    return true;
}

bool set_param_with_fallback(
    Param& param, const std::string& name, const ParsedValue& value, bool& used_float)
{
    used_float = value.is_float;

    if (value.is_float) {
        std::cout << std::format("Setting {} to {} as float...", name, value.float_val) << std::flush;
        auto result = param.set_param_float(name, value.float_val);

        if (result == Param::Result::Success) {
            std::cout << "Ok" << std::endl;
            return true;
        }

        if (result != Param::Result::WrongType) {
            std::cout << std::format("Failed: {}\n", result);
            return false;
        }

        // Fallback to int
        std::cout << std::format("WrongType, trying as int...") << std::flush;
        used_float = false;
        result = param.set_param_int(name, value.int_val);

        if (result == Param::Result::Success) {
            std::cout << "Ok" << std::endl;
            return true;
        }

        std::cout << std::format("Failed: {}\n", result);
        return false;

    } else {
        std::cout << std::format("Setting {} to {} as int...", name, value.int_val) << std::flush;
        auto result = param.set_param_int(name, value.int_val);

        if (result == Param::Result::Success) {
            std::cout << "Ok" << std::endl;
            return true;
        }

        if (result != Param::Result::WrongType) {
            std::cout << std::format("Failed: {}\n", result);
            return false;
        }

        // Fallback to float
        std::cout << std::format("WrongType, trying as float...") << std::flush;
        used_float = true;
        result = param.set_param_float(name, value.float_val);

        if (result == Param::Result::Success) {
            std::cout << "Ok" << std::endl;
            return true;
        }

        std::cout << std::format("Failed: {}\n", result);
        return false;
    }
}

bool get_param_value(Param& param, const std::string& name, bool is_float, ParsedValue& result)
{
    std::cout << std::format("Reading back {} as {}...", name, (is_float ? "float" : "int")) << std::flush;

    if (is_float) {
        auto get_result = param.get_param_float(name);
        if (get_result.first != Param::Result::Success) {
            std::cout << std::format("Failed: {}\n", get_result.first);
            return false;
        }
        result.is_float = true;
        result.float_val = get_result.second;
        result.int_val = static_cast<int>(get_result.second);
    } else {
        auto get_result = param.get_param_int(name);
        if (get_result.first != Param::Result::Success) {
            std::cout << std::format("Failed: {}\n", get_result.first);
            return false;
        }
        result.is_float = false;
        result.int_val = get_result.second;
        result.float_val = static_cast<float>(get_result.second);
    }

    std::cout << "Ok" << std::endl;
    return true;
}

void set(Param& param, std::string name, std::string value)
{
    ParsedValue parsed;
    if (!parse_value(value, parsed)) {
        return;
    }

    bool used_float;
    set_param_with_fallback(param, name, parsed, used_float);
}

void set_and_verify(Param& param, std::string name, std::string value)
{
    ParsedValue parsed;
    if (!parse_value(value, parsed)) {
        return;
    }

    bool used_float;
    if (!set_param_with_fallback(param, name, parsed, used_float)) {
        return;
    }

    ParsedValue readback;
    if (!get_param_value(param, name, used_float, readback)) {
        return;
    }

    if (used_float) {
        std::cout << std::format("Verifying: set={}, got={}", parsed.float_val, readback.float_val);
        if (parsed.float_val == readback.float_val) {
            std::cout << " -> MATCH" << std::endl;
        } else {
            std::cout << " -> MISMATCH!" << std::endl;
        }
    } else {
        std::cout << std::format("Verifying: set={}, got={}", parsed.int_val, readback.int_val);
        if (parsed.int_val == readback.int_val) {
            std::cout << " -> MATCH" << std::endl;
        } else {
            std::cout << " -> MISMATCH!" << std::endl;
        }
    }
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
