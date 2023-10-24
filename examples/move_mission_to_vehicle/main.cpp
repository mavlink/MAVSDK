/**
 * brief Demonstrates how to move a mission loaded from a .plan file such that the planned home
 * position if present or the first waypoint is colocated with the vehicle position.
 *
 * Steps to run this example:
 * 1.(a) Create a Mission in QGroundControl and save them to a file (.plan), or
 *   (b) Use a pre-created sample mission plan in
 *       "src/plugins/mission_raw/test_plans/qgroundcontrol_sample.plan".
 * 2. Run the example by passing the path of the QGC mission plan as argument.
 *
 * Example description:
 * 1. Imports QGC mission items from .plan file.
 * 2. Translates mission such that it starts at the vehicle location.
 * 3. Uploads mission items to vehicle.
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

    std::cout << "Waiting to discover system...\n";
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    // We wait for new systems to be discovered, once we find one that has an
    // autopilot, we decide to use it.
    mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
      auto system = mavsdk.systems().back();

      if (system->has_autopilot()) {
          std::cout << "Discovered autopilot\n";

          // Unsubscribe again as we only want to find one system.
          mavsdk.subscribe_on_new_system(nullptr);
          prom.set_value(system);
      }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds max, surely.
    if (fut.wait_for(std::chrono::seconds(3)) == std::future_status::timeout) {
        std::cerr << "No autopilot found, exiting.\n";
        return 1;
    }

    // Get discovered system now.
    auto system = fut.get();

    auto action = Action{system};
    auto mission_raw = MissionRaw{system};
    auto telemetry = Telemetry{system};

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


    const auto position = telemetry.position();

    double offset_x, offset_y = 0.0;

    if (import_res.second.has_planned_home) {
        std::cout << "Found planned home position, moving mission to vehicle position.\n";
        offset_x = import_res.second.planned_home_position.x -
                        static_cast<int32_t>(1e7 * position.latitude_deg);
        offset_y = import_res.second.planned_home_position.y -
                        static_cast<int32_t>(1e7 * position.longitude_deg);

    } else {
        std::cout << "No planned home position, move mission to first waypoint.\n";

        for (auto &item : import_res.second.mission_items) {
            if (item.frame == 3) { // MAV_FRAME_GLOBAL_RELATIVE_ALT
                offset_x = item.x - static_cast<int32_t>(1e7 * position.latitude_deg);
                offset_y = item.y - static_cast<int32_t>(1e7 * position.longitude_deg);
                break;
            }
        }
    }

    for (auto &item : import_res.second.mission_items) {
        if (item.frame == 3) { // MAV_FRAME_GLOBAL_RELATIVE_ALT
            item.x -= offset_x;
            item.y -= offset_y;
        }

    }

    std::cout << "Uploading mission...";
    const MissionRaw::Result upload_result =
        mission_raw.upload_mission(import_res.second.mission_items);
    if (upload_result != MissionRaw::Result::Success) {
        std::cerr << "Failed uploading mission: " << upload_result << '\n';
        return 1;
    }
    std::cout << "Mission uploaded.\n";


    return 0;
}

