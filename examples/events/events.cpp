//
// Simple example to demonstrate how to use the events plugin.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/events/events.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <iostream>
#include <thread>
#include <future>

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url> [-v] [monitor]\n"
              << "Connection URL format should be:\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n\n"
              << " -v: enable verbose output\n"
              << " monitor: listen for events (instead of printing the arming report)\n";
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }

    // Parse CLI arguments
    bool monitor = false;
    bool verbose = false;
    for (int i = 2; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "monitor") == 0) {
            monitor = true;
        }
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
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

    auto events = Events{*system};

    if (monitor) {
        // Listen for events
        events.subscribe_events([verbose](const Events::Event& event) {
            std::cout << "[" << event.log_level << "] " << event.message << std::endl;
            if (verbose) {
                if (!event.description.empty()) {
                    std::cout << "    Description: " << event.description << std::endl;
                }
                std::cout << "    Event name: " << event.event_namespace << "/" << event.event_name
                          << std::endl;
            }
        });
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else {
        // Listen for arming checks report
        const auto telemetry = Telemetry{*system};
        std::promise<bool> promise;
        auto future = promise.get_future();
        bool reported = false;
        events.subscribe_health_and_arming_checks(
            [&telemetry, &promise, &verbose, &reported](
                const Events::HealthAndArmingCheckReport& report) {
                if (reported) { // Report only once
                    return;
                }
                reported = true;
                std::cout << "Current Mode (intention): " << report.current_mode_intention.mode_name
                          << std::endl;
                const std::string can_arm_or_run = telemetry.armed() ? "Can Run: " : "Can Arm: ";
                std::cout << can_arm_or_run
                          << (report.current_mode_intention.can_arm_or_run ? "yes" : "No")
                          << std::endl;
                if (!report.current_mode_intention.problems.empty()) {
                    std::cout << "Reports:" << std::endl;
                    for (const auto& problem : report.current_mode_intention.problems) {
                        std::cout << "  [" << problem.log_level << "] [" << problem.health_component
                                  << "]: " << problem.message << std::endl;
                    }
                }

                if (verbose && !report.all_problems.empty()) {
                    std::cout << "All Reports:" << std::endl;
                    for (const auto& problem : report.all_problems) {
                        std::cout << "  [" << problem.log_level << "] [" << problem.health_component
                                  << "]: " << problem.message << std::endl;
                    }
                }
                promise.set_value(true);
            });
        future.get();
    }

    return 0;
}
