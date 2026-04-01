//
// Example to demonstrate how to use set_callback_executor to control
// which thread runs MAVSDK callbacks.
//
// Instead of MAVSDK's internal callback thread, callbacks are queued
// and processed on the main thread, which is useful for integrating
// with UI frameworks or custom event loops.
//
// Note: blocking/synchronous APIs like first_autopilot() must not be
// called from the thread that drains the executor queue, because they
// wait for a callback that the blocked thread would need to process.
// Use async APIs and drain callbacks in your event loop instead.
//

#include <array>
#include <chrono>
#include <deque>
#include <functional>
#include <iostream>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mutex>
#include <thread>

#ifdef __linux__
#include <pthread.h>
#endif

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

std::string get_thread_name()
{
#ifdef __linux__
    std::array<char, 16> name{};
    if (pthread_getname_np(pthread_self(), name.data(), name.size()) == 0 && name[0] != '\0') {
        return name.data();
    }
#endif
    return "unknown";
}

// Helper: drain all pending callbacks from the queue and execute them.
void drain_callbacks(std::mutex& queue_mutex, std::deque<std::function<void()>>& callback_queue)
{
    std::deque<std::function<void()>> pending;
    {
        std::lock_guard<std::mutex> lock(queue_mutex);
        pending.swap(callback_queue);
    }
    for (auto& cb : pending) {
        cb();
    }
}

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

#ifdef __linux__
    pthread_setname_np(pthread_self(), "my_main");
#endif

    // A simple thread-safe callback queue that the executor pushes into.
    std::mutex queue_mutex;
    std::deque<std::function<void()>> callback_queue;

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};

    // Set a custom executor: instead of running callbacks on MAVSDK's
    // internal thread, each callback is pushed into our queue.
    // The executor is called from MAVSDK's work thread, so it must be
    // fast -- just queue the callback, don't execute it here.
    mavsdk.set_callback_executor([&](std::function<void()> cb) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        callback_queue.push_back(std::move(cb));
    });

    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    // With a custom executor we can't use blocking APIs like first_autopilot()
    // from the drain thread. Use subscribe_on_new_system() and drain while waiting.
    std::shared_ptr<System> system;
    auto handle = mavsdk.subscribe_on_new_system([&]() {
        for (auto& s : mavsdk.systems()) {
            if (s->has_autopilot()) {
                system = s;
                break;
            }
        }
    });

    auto deadline = std::chrono::steady_clock::now() + seconds(3);
    while (!system && std::chrono::steady_clock::now() < deadline) {
        drain_callbacks(queue_mutex, callback_queue);
        sleep_for(std::chrono::milliseconds(50));
    }
    mavsdk.unsubscribe_on_new_system(handle);

    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    auto telemetry = Telemetry{system};

    // Note: don't use sync APIs like set_rate_position() from the drain
    // thread -- they block waiting for a command ACK callback that this
    // thread would need to process. Subscriptions are fine (non-blocking).
    telemetry.subscribe_position([](Telemetry::Position position) {
        std::cout << "[" << get_thread_name() << "] Position: lat=" << position.latitude_deg
                  << " lon=" << position.longitude_deg << " alt=" << position.relative_altitude_m
                  << " m\n";
    });

    // Subscribe to battery updates
    telemetry.subscribe_battery([](Telemetry::Battery battery) {
        std::cout << "[" << get_thread_name() << "] Battery: " << battery.remaining_percent * 100.0f
                  << "% (" << battery.voltage_v << " V)\n";
    });

    std::cout << "Receiving telemetry callbacks on thread '" << get_thread_name()
              << "' for 10 seconds...\n";

    // Main loop: drain the callback queue on this thread.
    // In a real application this would be your UI event loop or similar.
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < seconds(10)) {
        drain_callbacks(queue_mutex, callback_queue);
        sleep_for(std::chrono::milliseconds(50));
    }

    std::cout << "Done.\n";
    return 0;
}
