//
// Demonstrates how to add and fly Waypoint missions using the MAVSDK.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

Mission::MissionItem make_mission_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    Mission::MissionItem::CameraAction camera_action)
{
    Mission::MissionItem new_item{};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    new_item.speed_m_s = speed_m_s;
    new_item.is_fly_through = is_fly_through;
    new_item.gimbal_pitch_deg = gimbal_pitch_deg;
    new_item.gimbal_yaw_deg = gimbal_yaw_deg;
    new_item.camera_action = camera_action;
    return new_item;
}

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

    Mavsdk mavsdk;
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

    auto action = Action{system.value()};
    auto mission = Mission{system.value()};
    auto telemetry = Telemetry{system.value()};

    while (!telemetry.health_all_ok()) {
        std::cout << "Waiting for system to be ready\n";
        sleep_for(seconds(1));
    }

    std::cout << "System ready\n";
    std::cout << "Creating and uploading mission\n";

    std::vector<Mission::MissionItem> mission_items;

    mission_items.push_back(make_mission_item(
        47.398170327054473,
        8.5456490218639658,
        10.0f,
        5.0f,
        false,
        20.0f,
        60.0f,
        Mission::MissionItem::CameraAction::None));

    mission_items.push_back(make_mission_item(
        47.398241338125118,
        8.5455360114574432,
        10.0f,
        2.0f,
        true,
        0.0f,
        -60.0f,
        Mission::MissionItem::CameraAction::TakePhoto));

    mission_items.push_back(make_mission_item(
        47.398139363821485,
        8.5453846156597137,
        10.0f,
        5.0f,
        true,
        -45.0f,
        0.0f,
        Mission::MissionItem::CameraAction::StartVideo));

    mission_items.push_back(make_mission_item(
        47.398058617228855,
        8.5454618036746979,
        10.0f,
        2.0f,
        false,
        -90.0f,
        30.0f,
        Mission::MissionItem::CameraAction::StopVideo));

    mission_items.push_back(make_mission_item(
        47.398100366082858,
        8.5456969141960144,
        10.0f,
        5.0f,
        false,
        -45.0f,
        -30.0f,
        Mission::MissionItem::CameraAction::StartPhotoInterval));

    mission_items.push_back(make_mission_item(
        47.398001890458097,
        8.5455576181411743,
        10.0f,
        5.0f,
        false,
        0.0f,
        0.0f,
        Mission::MissionItem::CameraAction::StopPhotoInterval));

    std::cout << "Uploading mission...\n";
    Mission::MissionPlan mission_plan{};
    mission_plan.mission_items = mission_items;
    const Mission::Result upload_result = mission.upload_mission(mission_plan);

    if (upload_result != Mission::Result::Success) {
        std::cerr << "Mission upload failed: " << upload_result << ", exiting.\n";
        return 1;
    }

    std::cout << "Arming...\n";
    const Action::Result arm_result = action.arm();
    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return 1;
    }
    std::cout << "Armed.\n";

    std::atomic<bool> want_to_pause{false};
    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    mission.subscribe_mission_progress([&want_to_pause](Mission::MissionProgress mission_progress) {
        std::cout << "Mission status update: " << mission_progress.current << " / "
                  << mission_progress.total << '\n';

        if (mission_progress.current >= 2) {
            // We can only set a flag here. If we do more request inside the callback,
            // we risk blocking the system.
            want_to_pause = true;
        }
    });

    Mission::Result start_mission_result = mission.start_mission();
    if (start_mission_result != Mission::Result::Success) {
        std::cerr << "Starting mission failed: " << start_mission_result << '\n';
        return 1;
    }

    while (!want_to_pause) {
        sleep_for(seconds(1));
    }

    std::cout << "Pausing mission...\n";
    const Mission::Result pause_mission_result = mission.pause_mission();

    if (pause_mission_result != Mission::Result::Success) {
        std::cerr << "Failed to pause mission:" << pause_mission_result << '\n';
    }
    std::cout << "Mission paused.\n";

    // Pause for 5 seconds.
    sleep_for(seconds(5));

    // Then continue.
    Mission::Result start_mission_again_result = mission.start_mission();
    if (start_mission_again_result != Mission::Result::Success) {
        std::cerr << "Starting mission again failed: " << start_mission_again_result << '\n';
        return 1;
    }

    while (!mission.is_mission_finished().second) {
        sleep_for(seconds(1));
    }

    // We are done, and can do RTL to go home.
    std::cout << "Commanding RTL...\n";
    const Action::Result rtl_result = action.return_to_launch();
    if (rtl_result != Action::Result::Success) {
        std::cout << "Failed to command RTL: " << rtl_result << '\n';
        return 1;
    }
    std::cout << "Commanded RTL.\n";

    // We need to wait a bit, otherwise the armed state might not be correct yet.
    sleep_for(seconds(2));

    while (telemetry.armed()) {
        // Wait until we're done.
        sleep_for(seconds(1));
    }
    std::cout << "Disarmed, exiting.\n";
}
