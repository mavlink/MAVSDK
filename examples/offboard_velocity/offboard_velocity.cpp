/**
 * @file offboard_velocity.cpp
 * @brief Example that demonstrates offboard velocity control in local NED and
 * body coordinates
 *
 * @authors Author: Julian Oes <julian@oes.ch>,
 *                  Shakthi Prashanth <shakthi.prashanth.m@intel.com>
 */

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
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

// Handles Action's result
inline void action_error_exit(Action::Result result, const std::string& message)
{
    if (result != Action::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Handles Offboard's result
inline void offboard_error_exit(Offboard::Result result, const std::string& message)
{
    if (result != Offboard::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Handles connection result
inline void connection_error_exit(ConnectionResult result, const std::string& message)
{
    if (result != ConnectionResult::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Logs during Offboard control
inline void offboard_log(const std::string& offb_mode, const std::string msg)
{
    std::cout << "[" << offb_mode << "] " << msg << std::endl;
}

/**
 * Does Offboard control using NED co-ordinates.
 *
 * returns true if everything went well in Offboard control, exits with a log
 * otherwise.
 */
bool offb_ctrl_ned(mavsdk::Offboard& offboard)
{
    const std::string offb_mode = "NED";
    // Send it once before starting offboard, otherwise it will be rejected.
    const Offboard::VelocityNedYaw stay{};
    offboard.set_velocity_ned(stay);

    Offboard::Result offboard_result = offboard.start();
    offboard_error_exit(offboard_result, "Offboard start failed");
    offboard_log(offb_mode, "Offboard started");

    offboard_log(offb_mode, "Turn to face East");

    Offboard::VelocityNedYaw turn_east{};
    turn_east.yaw_deg = 90.0f;
    offboard.set_velocity_ned(turn_east);
    sleep_for(seconds(1)); // Let yaw settle.

    {
        const float step_size = 0.01f;
        const float one_cycle = 2.0f * (float)M_PI;
        const unsigned steps = 2 * unsigned(one_cycle / step_size);

        offboard_log(offb_mode, "Go North and back South");
        for (unsigned i = 0; i < steps; ++i) {
            float vx = 5.0f * sinf(i * step_size);
            Offboard::VelocityNedYaw north_and_back_south{};
            north_and_back_south.north_m_s = vx;
            north_and_back_south.yaw_deg = 90.0f;
            offboard.set_velocity_ned(north_and_back_south);
            sleep_for(milliseconds(10));
        }
    }

    offboard_log(offb_mode, "Turn to face West");
    Offboard::VelocityNedYaw turn_west{};
    turn_west.yaw_deg = 270.0f;
    offboard.set_velocity_ned(turn_west);
    sleep_for(seconds(2));

    offboard_log(offb_mode, "Go up 2 m/s, turn to face South");
    Offboard::VelocityNedYaw up_and_south{};
    up_and_south.down_m_s = -2.0f;
    up_and_south.yaw_deg = 180.0f;
    offboard.set_velocity_ned(up_and_south);
    sleep_for(seconds(4));

    offboard_log(offb_mode, "Go down 1 m/s, turn to face North");
    Offboard::VelocityNedYaw down_and_north{};
    up_and_south.down_m_s = 1.0f;
    offboard.set_velocity_ned(down_and_north);
    sleep_for(seconds(4));

    // Now, stop offboard mode.
    offboard_result = offboard.stop();
    offboard_error_exit(offboard_result, "Offboard stop failed: ");
    offboard_log(offb_mode, "Offboard stopped");

    return true;
}

/**
 * Does Offboard control using body co-ordinates.
 *
 * returns true if everything went well in Offboard control, exits with a log
 * otherwise.
 */
bool offb_ctrl_body(mavsdk::Offboard& offboard)
{
    const std::string offb_mode = "BODY";

    // Send it once before starting offboard, otherwise it will be rejected.
    Offboard::VelocityBodyYawspeed stay{};
    offboard.set_velocity_body(stay);

    Offboard::Result offboard_result = offboard.start();
    offboard_error_exit(offboard_result, "Offboard start failed: ");
    offboard_log(offb_mode, "Offboard started");

    offboard_log(offb_mode, "Turn clock-wise and climb");
    Offboard::VelocityBodyYawspeed cc_and_climb{};
    cc_and_climb.down_m_s = -1.0f;
    cc_and_climb.yawspeed_deg_s = 60.0f;
    offboard.set_velocity_body(cc_and_climb);
    sleep_for(seconds(5));

    offboard_log(offb_mode, "Turn back anti-clockwise");
    Offboard::VelocityBodyYawspeed ccw{};
    ccw.down_m_s = -1.0f;
    ccw.yawspeed_deg_s = -60.0f;
    offboard.set_velocity_body(ccw);
    sleep_for(seconds(5));

    offboard_log(offb_mode, "Wait for a bit");
    offboard.set_velocity_body(stay);
    sleep_for(seconds(2));

    offboard_log(offb_mode, "Fly a circle");
    Offboard::VelocityBodyYawspeed circle{};
    circle.forward_m_s = 5.0f;
    circle.yawspeed_deg_s = 30.0f;
    offboard.set_velocity_body(circle);
    sleep_for(seconds(15));

    offboard_log(offb_mode, "Wait for a bit");
    offboard.set_velocity_body(stay);
    sleep_for(seconds(5));

    offboard_log(offb_mode, "Fly a circle sideways");
    circle.right_m_s = -5.0f;
    circle.yawspeed_deg_s = 30.0f;
    offboard.set_velocity_body(circle);
    sleep_for(seconds(15));

    offboard_log(offb_mode, "Wait for a bit");
    offboard.set_velocity_body(stay);
    sleep_for(seconds(8));

    offboard_result = offboard.stop();
    offboard_error_exit(offboard_result, "Offboard stop failed: ");
    offboard_log(offb_mode, "Offboard stopped");

    return true;
}

/**
 * Does Offboard control using attitude commands.
 *
 * returns true if everything went well in Offboard control, exits with a log
 * otherwise.
 */
bool offb_ctrl_attitude(mavsdk::Offboard& offboard)
{
    const std::string offb_mode = "ATTITUDE";

    // Send it once before starting offboard, otherwise it will be rejected.
    Offboard::Attitude roll{};
    roll.roll_deg = 30.0f;
    roll.thrust_value = 0.6f;
    offboard.set_attitude(roll);

    Offboard::Result offboard_result = offboard.start();
    offboard_error_exit(offboard_result, "Offboard start failed");
    offboard_log(offb_mode, "Offboard started");

    offboard_log(offb_mode, "ROLL 30");
    offboard.set_attitude(roll);
    sleep_for(seconds(2)); // rolling

    offboard_log(offb_mode, "ROLL -30");
    roll.roll_deg = -30.0f;
    offboard.set_attitude(roll);
    sleep_for(seconds(2)); // Let yaw settle.

    offboard_log(offb_mode, "ROLL 0");
    roll.roll_deg = 0.0f;
    offboard.set_attitude(roll);
    sleep_for(seconds(2)); // Let yaw settle.

    // Now, stop offboard mode.
    offboard_result = offboard.stop();
    offboard_error_exit(offboard_result, "Offboard stop failed: ");
    offboard_log(offb_mode, "Offboard stopped");

    return true;
}

void wait_until_discover(Mavsdk& mavsdk)
{
    std::cout << "Waiting to discover system..." << std::endl;
    std::promise<void> discover_promise;
    auto discover_future = discover_promise.get_future();

    mavsdk.subscribe_on_new_system([&mavsdk, &discover_promise]() {
        const auto system = mavsdk.systems().at(0);

        if (system->is_connected()) {
            std::cout << "Discovered system" << std::endl;
            discover_promise.set_value();
        }
    });

    discover_future.wait();
}

void usage(std::string bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}

Telemetry::LandedStateCallback
landed_state_callback(Telemetry& telemetry, std::promise<void>& landed_promise)
{
    return [&landed_promise, &telemetry](Telemetry::LandedState landed) {
        switch (landed) {
            case Telemetry::LandedState::OnGround:
                std::cout << "On ground" << std::endl;
                break;
            case Telemetry::LandedState::TakingOff:
                std::cout << "Taking off..." << std::endl;
                break;
            case Telemetry::LandedState::Landing:
                std::cout << "Landing..." << std::endl;
                break;
            case Telemetry::LandedState::InAir:
                std::cout << "Taking off has finished." << std::endl;
                telemetry.subscribe_landed_state(nullptr);
                landed_promise.set_value();
                break;
            case Telemetry::LandedState::Unknown:
                std::cout << "Unknown landed state." << std::endl;
                break;
        }
    };
}

int main(int argc, char** argv)
{
    Mavsdk mavsdk;
    std::string connection_url;
    ConnectionResult connection_result;

    if (argc == 2) {
        connection_url = argv[1];
        connection_result = mavsdk.add_any_connection(connection_url);
    } else {
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Connection failed: " << connection_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Wait for the system to connect via heartbeat
    wait_until_discover(mavsdk);

    // System got discovered.
    auto system = mavsdk.systems().at(0);
    auto action = Action{system};
    auto offboard = Offboard{system};
    auto telemetry = Telemetry{system};

    while (!telemetry.health_all_ok()) {
        std::cout << "Waiting for system to be ready" << std::endl;
        sleep_for(seconds(1));
    }
    std::cout << "System is ready" << std::endl;

    std::promise<void> in_air_promise;
    auto in_air_future = in_air_promise.get_future();

    Action::Result arm_result = action.arm();
    action_error_exit(arm_result, "Arming failed");
    std::cout << "Armed" << std::endl;

    Action::Result takeoff_result = action.takeoff();
    action_error_exit(takeoff_result, "Takeoff failed");

    telemetry.subscribe_landed_state(landed_state_callback(telemetry, in_air_promise));
    in_air_future.wait();

    //  using attitude control
    bool ret = offb_ctrl_attitude(offboard);
    if (ret == false) {
        return EXIT_FAILURE;
    }

    //  using local NED co-ordinates
    ret = offb_ctrl_ned(offboard);
    if (ret == false) {
        return EXIT_FAILURE;
    }

    //  using body co-ordinates
    ret = offb_ctrl_body(offboard);
    if (ret == false) {
        return EXIT_FAILURE;
    }

    const Action::Result land_result = action.land();
    action_error_exit(land_result, "Landing failed");

    // Check if vehicle is still in air
    while (telemetry.in_air()) {
        std::cout << "Vehicle is landing..." << std::endl;
        sleep_for(seconds(1));
    }
    std::cout << "Landed!" << std::endl;

    // We are relying on auto-disarming but let's keep watching the telemetry for
    // a bit longer.
    sleep_for(seconds(3));
    std::cout << "Finished..." << std::endl;

    return EXIT_SUCCESS;
}
