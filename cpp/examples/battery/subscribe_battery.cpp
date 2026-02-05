//
// Example for subscribe Battery data via MAVSDK
//
// Author: PannapatC <Github: Aminballoon>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>

// Global atomic flag to indicate if the program should exit
std::atomic<bool> should_exit{false};

// Signal handler for Ctrl+C
void signal_handler(int signal)
{
    if (signal == SIGINT) {
        std::cout << "\nCtrl+C pressed. Exiting..." << std::endl;
        should_exit.store(true);
    }
}

int main(int argc, char** argv)
{
    // Check for connection string argument
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <connection_url>" << std::endl;
        return 1;
    }

    // Initialize MAVSDK with GroundStation component type
    mavsdk::Mavsdk mavsdk{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};

    // Add connection
    mavsdk::ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != mavsdk::ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    // Wait for the system to connect
    std::cout << "Waiting for system to connect..." << std::endl;
    while (mavsdk.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Get the connected system
    auto system = mavsdk.systems().at(0);
    if (!system->is_connected()) {
        std::cerr << "System not connected" << std::endl;
        return 1;
    }

    // Instantiate the Telemetry plugin
    auto telemetry = mavsdk::Telemetry{system};

    // Callback for battery updates
    auto battery_callback = [](mavsdk::Telemetry::Battery battery) {
        std::cout << "Battery ID: " << battery.id << " \n"
                  << "Battery Temp: " << battery.temperature_degc << " degc\n"
                  << "Battery Voltage: " << battery.voltage_v << " v\n"
                  << "Battery Current: " << battery.current_battery_a << " a\n"
                  << "Battery Capacity Consumed: " << battery.capacity_consumed_ah << " ah\n"
                  << "Battery Remaining Percent: " << battery.remaining_percent << " %\n"
                  << "Battery Remaining Time: " << battery.time_remaining_s << " s\n"
                  << "Battery Function: " << battery.battery_function << "\n"
                  << std::endl;
    };

    // Subscribe to battery updates
    auto battery_handle = telemetry.subscribe_battery(battery_callback);

    // Register the signal handler for Ctrl+C
    std::signal(SIGINT, signal_handler);

    // Main loop
    std::cout << "Listening for battery updates. Press Ctrl+C to exit..." << std::endl;
    while (!should_exit.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Unsubscribe from battery updates
    telemetry.unsubscribe_battery(battery_handle);
    std::cout << "Unsubscribed from battery updates." << std::endl;

    std::cout << "Exiting program." << std::endl;
    return 0;
}