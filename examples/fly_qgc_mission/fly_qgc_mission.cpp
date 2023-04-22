/**
 * brief Demonstrates how to import mission items from a QGroundControl plan,
 * and fly them using the MAVSDK.
 *
 * Steps to run this example:
 * 1.(a) Create a Mission in QGroundControl and save them to a file (.plan), or
 *   (b) Use a pre-created sample mission plan in
 *       "src/plugins/mission_raw/test_plans/qgroundcontrol_sample.plan".
 * 2. Run the example by passing the path of the QGC mission plan as argument.
 *
 * Example description:
 * 1. Imports QGC mission items from .plan file.
 * 2. Uploads mission items to vehicle.
 * 3. Starts mission from first mission item.
 * 4. Commands RTL once QGC Mission is accomplished.
 */

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission_raw/mission_raw.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <thread>

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url> <mission_plan_path>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk;

    const ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);
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
    auto mission_raw = MissionRaw{system.value()};
    auto telemetry = Telemetry{system.value()};

    while (!telemetry.health_all_ok()) {
        std::cout << "Waiting for system to be ready\n";
        sleep_for(seconds(1));
    }

    std::cout << "System ready\n";

    std::cout << "Importing mission from mission plan: " << argv[2] << '\n';
    std::pair<MissionRaw::Result, MissionRaw::MissionImportData> import_res =
        mission_raw.import_qgroundcontrol_mission(argv[2]);
    if (import_res.first != MissionRaw::Result::Success) {
        std::cerr << "Failed to import mission items: " << import_res.first;
        return 1;
    }

    if (import_res.second.mission_items.size() == 0) {
        std::cerr << "No missions! Exiting...\n";
        return 1;
    }
    std::cout << "Found " << import_res.second.mission_items.size()
              << " mission items in the given QGC plan.\n";

    std::cout << "Uploading mission...";
    const MissionRaw::Result upload_result =
        mission_raw.upload_mission(import_res.second.mission_items);
    if (upload_result != MissionRaw::Result::Success) {
        std::cerr << "Failed uploading mission: " << upload_result << '\n';
        return 1;
    }
    std::cout << "Mission uploaded.\n";

    std::cout << "Arming...\n";
    const Action::Result arm_result = action.arm();
    if (arm_result != Action::Result::Success) {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return 1;
    }
    std::cout << "Armed.\n";

    auto prom = std::promise<void>{};
    auto fut = prom.get_future();

    // Before starting the mission subscribe to the mission progress.
    mission_raw.subscribe_mission_progress([&prom](MissionRaw::MissionProgress mission_progress) {
        std::cout << "Mission progress update: " << mission_progress.current << " / "
                  << mission_progress.total << '\n';
        if (mission_progress.current == mission_progress.total) {
            prom.set_value();
        }
    });

    const MissionRaw::Result start_mission_result = mission_raw.start_mission();
    if (start_mission_result != MissionRaw::Result::Success) {
        std::cerr << "Starting mission failed: " << start_mission_result << '\n';
        return 1;
    }

    if (fut.wait_for(std::chrono::seconds(240)) != std::future_status::ready) {
        std::cerr << "Mission not finished yet, giving up.\n";
        return 1;
    }

    // Mission complete. Command RTL to go home.
    std::cout << "Commanding RTL...\n";
    const Action::Result result = action.return_to_launch();
    if (result != Action::Result::Success) {
        std::cerr << "Failed to command RTL: " << result << '\n';
        return 1;
    }
    std::cout << "Commanded RTL.\n";

    return 0;
}
