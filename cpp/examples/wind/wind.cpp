//
// Example for subscribe Wind data telemetry via MAVSDK
//
// Author: PannapatC <Github: Aminballoon>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <format>
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
        std::cerr << std::format("Usage: {} <connection_url>\n", argv[0]);
        return 1;
    }

    // Initialize MAVSDK with GroundStation component type
    mavsdk::Mavsdk mavsdk{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation}};

    // Add connection
    mavsdk::ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);
    if (connection_result != mavsdk::ConnectionResult::Success) {
        std::cerr << std::format("Connection failed: {}\n", connection_result);
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

    // Callback for wind updates
    auto wind_callback = [](mavsdk::Telemetry::Wind wind) {
        std::cout << std::format(
            "Wind_X_NED: {} m/s\nWind_Y_NED: {} m/s\nWind_Z_NED: {} m/s\nHorizontal_Variability: {} m/s\nVertical_Variability: {} m/s\nWind_Altitude: {} m\nHorizontal_Wind_Speed_Accuracy: {} m/s\nVertical_Wind_Speed_Accuracy: {} m/s\n\n",
            wind.wind_x_ned_m_s,
            wind.wind_y_ned_m_s,
            wind.wind_z_ned_m_s,
            wind.horizontal_variability_stddev_m_s,
            wind.vertical_variability_stddev_m_s,
            wind.wind_altitude_msl_m,
            wind.horizontal_wind_speed_accuracy_m_s,
            wind.vertical_wind_speed_accuracy_m_s);
    };

    // Subscribe to wind updates
    auto wind_handle = telemetry.subscribe_wind(wind_callback);

    // Register the signal handler for Ctrl+C
    std::signal(SIGINT, signal_handler);

    // Main loop
    std::cout << "Listening for wind updates. Press Ctrl+C to exit..." << std::endl;
    while (!should_exit.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Unsubscribe from wind updates
    telemetry.unsubscribe_wind(wind_handle);
    std::cout << "Unsubscribed from wind updates." << std::endl;

    std::cout << "Exiting program." << std::endl;
    return 0;
}