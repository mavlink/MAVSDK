//
// Example demonstrating how to set GPS global origin after flying to a new location.
// This takes off, flies 50m north using offboard velocity control, lands,
// and then sets the GPS global origin at the new location.
//

#include <chrono>
#include <cmath>
#include <future>
#include <iostream>
#include <thread>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/offboard/offboard.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP server: tcpin://<our_ip>:<port>\n"
              << " For TCP client: tcpout://<remote_ip>:<port>\n"
              << " For UDP server: udpin://<our_ip>:<port>\n"
              << " For UDP client: udpout://<remote_ip>:<port>\n"
              << " For Serial : serial://</path/to/serial/dev>:<baudrate>]\n"
              << "For example, to connect to the simulator use URL: udpin://0.0.0.0:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
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

    // Instantiate plugins
    auto action = Action{system.value()};
    auto offboard = Offboard{system.value()};
    auto telemetry = Telemetry{system.value()};

    // Wait for system to be ready
    while (!telemetry.health_all_ok()) {
        std::cout << "Waiting for system to be ready\n";
        sleep_for(seconds(1));
    }
    std::cout << "System is ready\n";

    // Get initial GPS origin
    auto origin_result = telemetry.get_gps_global_origin();
    if (origin_result.first == Telemetry::Result::Success) {
        std::cout << "Initial GPS origin: " << origin_result.second << '\n';
    }

    // Get initial position
    auto initial_position = telemetry.position();
    std::cout << "Initial position: lat=" << initial_position.latitude_deg
              << ", lon=" << initial_position.longitude_deg
              << ", alt=" << initial_position.absolute_altitude_m << "m AMSL\n";

    // Arm
    std::cout << "Arming...\n";
    const auto arm_result = action.arm();
    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return 1;
    }
    std::cout << "Armed\n";

    // Take off
    std::cout << "Taking off...\n";
    const auto takeoff_result = action.takeoff();
    if (takeoff_result != Action::Result::Success) {
        std::cerr << "Takeoff failed: " << takeoff_result << '\n';
        return 1;
    }

    // Wait until in air
    auto in_air_promise = std::promise<void>{};
    auto in_air_future = in_air_promise.get_future();
    Telemetry::LandedStateHandle landed_handle = telemetry.subscribe_landed_state(
        [&telemetry, &in_air_promise, &landed_handle](Telemetry::LandedState state) {
            if (state == Telemetry::LandedState::InAir) {
                std::cout << "Takeoff finished\n";
                telemetry.unsubscribe_landed_state(landed_handle);
                in_air_promise.set_value();
            }
        });

    if (in_air_future.wait_for(seconds(10)) == std::future_status::timeout) {
        std::cerr << "Takeoff timed out\n";
        telemetry.unsubscribe_landed_state(landed_handle);
        return 1;
    }

    // Let it stabilize at takeoff altitude
    sleep_for(seconds(2));

    // Start offboard mode and fly 50m north
    std::cout << "Starting offboard mode to fly 50m north...\n";

    // Send setpoint once before starting offboard
    Offboard::VelocityNedYaw stay{};
    offboard.set_velocity_ned(stay);

    Offboard::Result offboard_result = offboard.start();
    if (offboard_result != Offboard::Result::Success) {
        std::cerr << "Offboard start failed: " << offboard_result << '\n';
        return 1;
    }

    // Fly north at 5 m/s for 10 seconds = 50m
    std::cout << "Flying north at 5 m/s for 10 seconds...\n";
    Offboard::VelocityNedYaw fly_north{};
    fly_north.north_m_s = 5.0f;
    fly_north.yaw_deg = 0.0f;
    offboard.set_velocity_ned(fly_north);
    sleep_for(seconds(10));

    // Stop and hover
    std::cout << "Stopping...\n";
    offboard.set_velocity_ned(stay);
    sleep_for(seconds(2));

    // Stop offboard mode
    offboard_result = offboard.stop();
    if (offboard_result != Offboard::Result::Success) {
        std::cerr << "Offboard stop failed: " << offboard_result << '\n';
        return 1;
    }

    // Get position before landing
    auto position_before_landing = telemetry.position();
    std::cout << "Position before landing: lat=" << position_before_landing.latitude_deg
              << ", lon=" << position_before_landing.longitude_deg
              << ", alt=" << position_before_landing.absolute_altitude_m << "m AMSL\n";

    // Land
    std::cout << "Landing...\n";
    const auto land_result = action.land();
    if (land_result != Action::Result::Success) {
        std::cerr << "Landing failed: " << land_result << '\n';
        return 1;
    }

    // Wait until landed
    while (telemetry.in_air()) {
        std::cout << "Waiting for landing...\n";
        sleep_for(seconds(1));
    }
    std::cout << "Landed!\n";

    // Wait a bit for things to settle
    sleep_for(seconds(2));

    // Get final position
    auto final_position = telemetry.position();
    std::cout << "Final position: lat=" << final_position.latitude_deg
              << ", lon=" << final_position.longitude_deg
              << ", alt=" << final_position.absolute_altitude_m << "m AMSL\n";

    // Set the GPS global origin at the current position
    std::cout << "Setting GPS global origin at current position...\n";
    const auto set_origin_result = action.set_gps_global_origin(
        final_position.latitude_deg,
        final_position.longitude_deg,
        final_position.absolute_altitude_m);

    if (set_origin_result != Action::Result::Success) {
        std::cerr << "Setting GPS global origin failed: " << set_origin_result << '\n';
        return 1;
    }
    std::cout << "GPS global origin set successfully!\n";

    // Verify by getting the new origin
    auto new_origin_result = telemetry.get_gps_global_origin();
    if (new_origin_result.first == Telemetry::Result::Success) {
        std::cout << "New GPS origin: " << new_origin_result.second << '\n';
    } else {
        std::cerr << "Failed to get new GPS origin: " << new_origin_result.first << '\n';
    }

    // Wait a bit before exiting
    sleep_for(seconds(3));
    std::cout << "Finished!\n";

    return 0;
}
