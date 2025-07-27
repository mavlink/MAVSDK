#include "log.h"
#include "mavsdk.h"
#include "plugins/param/param.h"
#include "plugins/param_server/param_server.h"
#include <chrono>
#include <vector>
#include <thread>
#include <map>
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#include <cstring>

using namespace mavsdk;

static constexpr unsigned num_params_per_type = 3;
static constexpr double reduced_timeout_s = 0.1;

// Async-signal-safe crash handler
void crash_handler(int sig)
{
    // Use only async-signal-safe functions in signal handler
    const char msg1[] = "=== CRASH DETECTED ===\n";
    ssize_t unused = write(STDERR_FILENO, msg1, sizeof(msg1) - 1);
    (void)unused;

    // Format signal info using async-signal-safe functions
    char sig_msg[100];
    const char* signal_name = strsignal(sig);
    int len = 0;
    if (signal_name) {
        len = snprintf(sig_msg, sizeof(sig_msg), "Signal: %s (%d)\n", signal_name, sig);
    } else {
        len = snprintf(sig_msg, sizeof(sig_msg), "Signal: %d\n", sig);
    }
    if (len > 0 && len < (int)sizeof(sig_msg)) {
        unused = write(STDERR_FILENO, sig_msg, len);
        (void)unused;
    }

    // Process ID
    char pid_msg[50];
    len = snprintf(pid_msg, sizeof(pid_msg), "Process: %d\n", getpid());
    if (len > 0 && len < (int)sizeof(pid_msg)) {
        unused = write(STDERR_FILENO, pid_msg, len);
        (void)unused;
    }

    // Stack trace using async-signal-safe backtrace_symbols_fd
#if defined(__linux__) || defined(__APPLE__)
    const char trace_msg[] = "Stack trace (backtrace):\n";
    unused = write(STDERR_FILENO, trace_msg, sizeof(trace_msg) - 1);
    (void)unused;

    void* array[20];
    size_t size = backtrace(array, 20);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif

    const char msg2[] = "=== END CRASH INFO ===\n";
    unused = write(STDERR_FILENO, msg2, sizeof(msg2) - 1);
    (void)unused;

    // Force immediate exit to prevent hanging
    _exit(128 + sig);
}

static std::map<std::string, float> generate_float_params()
{
    std::map<std::string, float> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
        const auto id = std::string("TEST_FLOAT") + std::to_string(i);
        const float value = 42.0f + static_cast<float>(i);
        params[id] = value;
    }
    return params;
}

static std::map<std::string, int> generate_int_params()
{
    std::map<std::string, int> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
        const auto id = std::string("TEST_INT") + std::to_string(i);
        const int value = 42 + i;
        params[id] = value;
    }
    return params;
}

static std::map<std::string, std::string> generate_string_params()
{
    std::map<std::string, std::string> params;
    for (unsigned i = 0; i < num_params_per_type; ++i) {
        const auto id = std::string("TEST_STRING") + std::to_string(i);
        const std::string value = "VAL" + std::to_string(i);
        params[id] = value;
    }
    return params;
}

template<typename T1, typename T2>
static bool check_equal(const std::map<std::string, T1>& values, const std::vector<T2>& received)
{
    if (received.size() != values.size()) {
        std::cout << "Size mismatch: expected " << values.size() << ", got " << received.size()
                  << std::endl;
        return false;
    }

    for (const auto& param : received) {
        auto it = values.find(param.name);
        if (it == values.end()) {
            std::cout << "Parameter not found: " << param.name << std::endl;
            return false;
        }
        if (param.value != it->second) {
            std::cout << "Value mismatch for " << param.name << ": expected " << it->second
                      << ", got " << param.value << std::endl;
            return false;
        }
    }
    return true;
}

int main()
{
    // Install crash handler
    signal(SIGSEGV, crash_handler);
    signal(SIGABRT, crash_handler);
    signal(SIGFPE, crash_handler);
    signal(SIGILL, crash_handler);

    std::cout << "Running standalone param test..." << std::endl;

    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    mavsdk_groundstation.set_timeout_s(reduced_timeout_s);

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

    if (mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000") !=
        ConnectionResult::Success) {
        std::cout << "Failed to add groundstation connection" << std::endl;
        return 1;
    }

    if (mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000") !=
        ConnectionResult::Success) {
        std::cout << "Failed to add autopilot connection" << std::endl;
        return 1;
    }

    auto param_server = ParamServer{mavsdk_autopilot.server_component()};

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    if (!maybe_system) {
        std::cout << "No autopilot found" << std::endl;
        return 1;
    }
    auto system = maybe_system.value();

    if (!system->has_autopilot()) {
        std::cout << "System does not have autopilot" << std::endl;
        return 1;
    }

    const auto test_float_params = generate_float_params();
    const auto test_int_params = generate_int_params();
    const auto test_string_params = generate_string_params();

    // Add float params
    for (auto const& [key, val] : test_float_params) {
        if (param_server.provide_param_float(key, val) != ParamServer::Result::Success) {
            std::cout << "Failed to provide float param: " << key << std::endl;
            return 1;
        }
    }

    // Add int params
    for (auto const& [key, val] : test_int_params) {
        if (param_server.provide_param_int(key, val) != ParamServer::Result::Success) {
            std::cout << "Failed to provide int param: " << key << std::endl;
            return 1;
        }
    }

    // Add string params
    for (auto const& [key, val] : test_string_params) {
        if (param_server.provide_param_custom(key, val) != ParamServer::Result::Success) {
            std::cout << "Failed to provide custom param: " << key << std::endl;
            return 1;
        }
    }

    // Test V1 protocol
    {
        auto param_sender = Param{system};
        param_sender.select_component(1, Param::ProtocolVersion::V1);
        const auto all_params = param_sender.get_all_params();

        if (!check_equal<int, Param::IntParam>(test_int_params, all_params.int_params)) {
            std::cout << "V1 int params check failed" << std::endl;
            return 1;
        }
        if (!check_equal<float, Param::FloatParam>(test_float_params, all_params.float_params)) {
            std::cout << "V1 float params check failed" << std::endl;
            return 1;
        }
    }

    // Test extended protocol
    {
        auto param_sender = Param{system};
        param_sender.select_component(1, Param::ProtocolVersion::Ext);
        const auto all_params = param_sender.get_all_params();

        if (!check_equal<int, Param::IntParam>(test_int_params, all_params.int_params)) {
            std::cout << "Ext int params check failed" << std::endl;
            return 1;
        }
        if (!check_equal<float, Param::FloatParam>(test_float_params, all_params.float_params)) {
            std::cout << "Ext float params check failed" << std::endl;
            return 1;
        }
        if (!check_equal<std::string, Param::CustomParam>(
                test_string_params, all_params.custom_params)) {
            std::cout << "Ext custom params check failed" << std::endl;
            return 1;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::cout << "Standalone param test completed successfully" << std::endl;
    return 0;
}