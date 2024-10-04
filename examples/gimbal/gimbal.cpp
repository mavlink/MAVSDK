//
// Simple example to demonstrate how to control a gimbal.
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
#include <mutex>
#include <vector>
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

    // Instantiate plugins.
    auto gimbal = Gimbal{system.value()};

    auto prom = std::promise<int32_t>();
    auto fut = prom.get_future();
    std::once_flag once_flag;
    // Wait for a gimbal.
    gimbal.subscribe_gimbal_list([&prom, &once_flag](const Gimbal::GimbalList& gimbal_list) {
        std::cout << "Found a gimbal: " << gimbal_list.gimbals.back().model_name << " by "
                  << gimbal_list.gimbals.back().vendor_name << '\n';
        std::call_once(once_flag, [&prom, &gimbal_list]() {
            prom.set_value(gimbal_list.gimbals.back().gimbal_id);
        });
    });

    if (fut.wait_for(std::chrono::seconds(3)) == std::future_status::timeout) {
        std::cerr << "Could not find a gimbal, exiting...";
        return 1;
    }

    const int32_t gimbal_id = fut.get();
    std::cout << "Use gimbal ID: " << gimbal_id << '\n';

    // Set up callback to monitor camera/gimbal angle
    gimbal.subscribe_attitude([](Gimbal::Attitude attitude) {
        std::cout << "Gimbal angle pitch: " << attitude.euler_angle_forward.pitch_deg
                  << " deg, yaw: " << attitude.euler_angle_forward.yaw_deg
                  << " yaw (relative to forward)\n";
        std::cout << "Gimbal angle pitch: " << attitude.euler_angle_north.pitch_deg
                  << " deg, yaw: " << attitude.euler_angle_north.yaw_deg
                  << " yaw (relative to North)\n";
    });

    std::cout << "Start controlling gimbal...\n";
    Gimbal::Result gimbal_result = gimbal.take_control(gimbal_id, Gimbal::ControlMode::Primary);
    if (gimbal_result != Gimbal::Result::Success) {
        std::cerr << "Could not take gimbal control: " << gimbal_result << '\n';
        return 1;
    }

    // Fetch the gimbal attitude
    auto [result, attitude] = gimbal.get_attitude(gimbal_id);

    std::cout << "Use yaw mode to lock to a specific direction...\n";

    std::cout << "Look North...\n";
    gimbal.set_angles(
        gimbal_id,
        0.0f,
        0.0f,
        -attitude.euler_angle_north.yaw_deg,
        Gimbal::GimbalMode::YawLock,
        Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Look East...\n";
    gimbal.set_angles(
        gimbal_id,
        0.0f,
        0.0f,
        -attitude.euler_angle_north.yaw_deg + 90.0f,
        Gimbal::GimbalMode::YawLock,
        Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Look South...\n";
    gimbal.set_angles(
        gimbal_id,
        0.0f,
        0.0f,
        -attitude.euler_angle_north.yaw_deg + 180.0f,
        Gimbal::GimbalMode::YawLock,
        Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Look West...\n";
    gimbal.set_angles(
        gimbal_id,
        0.0f,
        0.0f,
        -attitude.euler_angle_north.yaw_deg - 90.0f,
        Gimbal::GimbalMode::YawLock,
        Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Now use follow mode...\n";

    std::cout << "And center first...\n";
    gimbal.set_angles(
        gimbal_id, 0.0f, 0.0f, 0.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Tilt gimbal down...\n";
    gimbal.set_angles(
        gimbal_id, 0.0f, -90.0f, 0.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Tilt gimbal back up...\n";
    gimbal.set_angles(
        gimbal_id, 0.0f, 0.0f, 0.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Slowly tilt up ...\n";
    gimbal.set_angular_rates(
        gimbal_id, 0.0f, 10.0f, 0.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Back to horizontal...\n";
    gimbal.set_angles(
        gimbal_id, 0.0f, 0.0f, 0.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Pan to the right...\n";
    gimbal.set_angles(
        gimbal_id, 0.0f, 0.0f, 90.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Back to the center...\n";
    gimbal.set_angles(
        gimbal_id, 0.0f, 0.0f, 0.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Pan slowly to the left...\n";
    gimbal.set_angular_rates(
        gimbal_id, 0.0f, 0.0f, -10.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Back to the center...\n";
    gimbal.set_angles(
        gimbal_id, 0.0f, 0.0f, 0.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Set ROI (region of interested) location...\n";
    gimbal.set_roi_location(gimbal_id, 47.39743832, 8.5463316, 488.0f);
    sleep_for(seconds(10));

    std::cout << "Back to the center...\n";
    gimbal.set_angles(0, 0.0f, 0.0f, 0.0f, Gimbal::GimbalMode::YawFollow, Gimbal::SendMode::Once);
    sleep_for(seconds(5));

    std::cout << "Stop controlling gimbal...\n";
    gimbal_result = gimbal.release_control(gimbal_id);
    if (gimbal_result != Gimbal::Result::Success) {
        std::cerr << "Could not release gimbal control: " << gimbal_result << '\n';
        return 1;
    }

    std::cout << "Finished.\n";
    return 0;
}
