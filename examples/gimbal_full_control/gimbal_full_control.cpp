//
// Example to demonstrate how to control a gimbal including roll.
//
// Can be tested against PX4 SITL with Typhoon H480:
//     make px4_sitl gazebo_typhoon_h480

#include <chrono>
#include <cstdint>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/gimbal/gimbal.h>
#include <iostream>
#include <future>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
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

    // Instantiate plugins.
    auto gimbal = Gimbal{system.value()};

    gimbal.subscribe_attitude([](Gimbal::Attitude attitude) {
        std::cout << "Gimbal angle pitch: " << attitude.euler_angle_forward.pitch_deg
                  << " deg, yaw: " << attitude.euler_angle_forward.yaw_deg << " deg\n";
    });

    std::cout << "Start controlling gimbal...\n";
    Gimbal::Result gimbal_result = gimbal.take_control(Gimbal::ControlMode::Primary);
    if (gimbal_result != Gimbal::Result::Success) {
        std::cerr << "Could not take gimbal control: " << gimbal_result << '\n';
        return 1;
    }

    std::cout << "Set yaw mode to lock to a specific direction...\n";
    gimbal_result = gimbal.set_mode(Gimbal::GimbalMode::YawLock);
    if (gimbal_result != Gimbal::Result::Success) {
        std::cerr << "Could not set to lock mode: " << gimbal_result << '\n';
        return 1;
    }

    std::cout << "Test SetAngles...\n";
    std::cout << "Roll=0 Pitch=0 Yaw=0\n";
    gimbal.set_angles(0.0f, 0.0f, 0.0f);
    sleep_for(seconds(5));
    std::cout << "Roll=-30 Pitch=0 Yaw=0\n";
    gimbal.set_angles(-30.0f, 0.0f, 0.0f);
    sleep_for(seconds(5));
    std::cout << "Roll=30 Pitch=0 Yaw=0\n";
    gimbal.set_angles(30.0f, 0.0f, 0.0f);
    sleep_for(seconds(5));
    std::cout << "Roll=0 Pitch=-30 Yaw=0\n";
    gimbal.set_angles(0.0f, -30.0f, 0.0f);
    sleep_for(seconds(5));
    std::cout << "Roll=0 Pitch=30 Yaw=0\n";
    gimbal.set_angles(0.0f, 30.0f, 0.0f);
    sleep_for(seconds(5));
    std::cout << "Roll=0 Pitch=0 Yaw=-30\n";
    gimbal.set_angles(0.0f, 0.0f, -30.0f);
    sleep_for(seconds(5));
    std::cout << "Roll=0 Pitch=0 Yaw=30\n";
    gimbal.set_angles(0.0f, 0.0f, 30.0f);
    sleep_for(seconds(5));
    std::cout << "Roll=0 Pitch=0 Yaw=0\n";
    gimbal.set_angles(0.0f, 0.0f, 0.0f);
    sleep_for(seconds(2));

    std::cout << "Stop controlling gimbal...\n";
    gimbal_result = gimbal.release_control();
    if (gimbal_result != Gimbal::Result::Success) {
        std::cerr << "Could not take gimbal control: " << gimbal_result << '\n';
        return 1;
    }

    std::cout << "Finished.\n";
    return 0;
}
