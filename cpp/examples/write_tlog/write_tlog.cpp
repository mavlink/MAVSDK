//
// Example: record all incoming MAVLink traffic to a TLOG file.
//
// Usage:
//   write_tlog <connection_url> <output.tlog>
//
// Example (SITL):
//   write_tlog udpin://0.0.0.0:14540 flight.tlog
//
// Press Ctrl+C to stop recording. The file can be opened in QGroundControl.
//

#include <mavsdk/mavsdk.hpp>

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <future>
#include <iostream>
#include <thread>

using namespace mavsdk;

static std::atomic<bool> should_exit{false};

static void signal_handler(int /*signum*/)
{
    should_exit.store(true, std::memory_order_relaxed);
}

static void usage(const char* bin)
{
    std::cerr << "Usage: " << bin << " <connection_url> <output.tlog>\n"
              << "Connection URL examples:\n"
              << "  UDP server : udpin://0.0.0.0:14540\n"
              << "  UDP client : udpout://127.0.0.1:14540\n"
              << "  TCP client : tcpout://192.168.1.1:5760\n"
              << "  Serial     : serial:///dev/ttyUSB0:57600\n";
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    const std::string connection_url = argv[1];
    const std::string tlog_path = argv[2];

    // Register signal handler so Ctrl+C flushes and closes the file cleanly.
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::CompanionComputer}};

    const ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    std::cout << "Waiting to discover system...\n";

    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();
    bool promise_set = false;

    Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&]() {
        auto system = mavsdk.systems().back();
        if (!promise_set) {
            promise_set = true;
            prom.set_value(system);
        }
    });

    if (fut.wait_for(std::chrono::seconds(5)) == std::future_status::timeout) {
        std::cerr << "No system found within 5 s. Is the vehicle connected?\n";
        return 1;
    }

    mavsdk.unsubscribe_on_new_system(handle);
    auto system = fut.get();
    std::cout << "Discovered system (sysid " << static_cast<int>(system->get_system_id()) << ")\n";

    // Start recording. All received MAVLink messages are appended to the file
    // as long as it is open, even while the program runs other code.
    if (!mavsdk.start_tlog_recording(tlog_path)) {
        std::cerr << "Failed to open '" << tlog_path << "' for writing.\n";
        return 1;
    }

    std::cout << "Recording to '" << tlog_path << "'. Press Ctrl+C to stop...\n";

    while (!should_exit.load(std::memory_order_relaxed)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << "\nStopping recording.\n";
    mavsdk.stop_tlog_recording();

    return 0;
}
