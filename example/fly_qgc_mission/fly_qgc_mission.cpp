/**
* @file fly_qgc_mission.cpp
*
* @brief Demonstrates how to import mission items from QGroundControl plan,
* and fly them using DroneCore.
*
* Steps to run this example:
* 1. (a) Create a Mission in QGroundControl and save them to a file (.plan) (OR)
*    (b) Use a pre-created sample mission plan in "plugins/mission/qgroundcontrol_sample.plan".
*    Click [here](https://user-images.githubusercontent.com/26615772/31763673-972c5bb6-b4dc-11e7-8ff0-f8b39b6b88c3.png) to see what sample mission plan in QGroundControl looks like.
* 2. Run the example by passing path of the QGC mission plan as argument (By default, sample mission plan is imported).
*
* Example description:
* 1. Imports QGC mission items from .plan file.
* 2. Uploads mission items to vehicle.
* 3. Starts mission from first mission item.
* 4. Commands RTL once QGC Mission is accomplished.
*
* @author Shakthi Prashanth M <shakthi.prashanth.m@intel.com>,
*         Julian Oes <julian@oes.ch>
* @date 2018-02-04
*/

#include <dronecore/action.h>
#include <dronecore/dronecore.h>
#include <dronecore/mission.h>
#include <dronecore/telemetry.h>
#include <functional>
#include <future>
#include <iostream>
#include <memory>

#define ERROR_CONSOLE_TEXT "\033[31m" //Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" //Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m"  //Restore normal console colour

using namespace dronecore;
using namespace std::chrono; // for seconds(), milliseconds()
using namespace std::this_thread; // for sleep_for()

// Handles Action's result
inline void handle_action_err_exit(ActionResult result, const std::string &message);
// Handles Mission's result
inline void handle_mission_err_exit(Mission::Result result, const std::string &message);
// Handles Connection result
inline void handle_connection_err_exit(ConnectionResult result,
                                       const std::string &message);

int main(int argc, char **argv)
{
    // Locate path of QGC Sample plan
    std::string qgc_plan = "../../../plugins/mission/qgroundcontrol_sample.plan";

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <path of QGC Mission plan>\n";
        std::cout << "Importing mission from Default mission plan: " << qgc_plan << std::endl;
    } else if (argc == 2) {
        std::cout << "Importing mission from mission plan: " << qgc_plan << std::endl;
        qgc_plan = argv[1];
    }

    DroneCore dc;

    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        std::cout << "Waiting to discover system..." << std::endl;
        dc.register_on_discover([prom](uint64_t uuid) {
            std::cout << "Discovered system with UUID: " << uuid << std::endl;
            prom->set_value();
        });

        ConnectionResult connection_result = dc.add_udp_connection();
        handle_connection_err_exit(connection_result, "Connection failed: ");

        future_result.get();
    }

    dc.register_on_timeout([](uint64_t uuid) {
        std::cout << "System with UUID timed out: " << uuid << std::endl;
        std::cout << "Exiting." << std::endl;
        exit(0);
    });

    // We don't need to specify the UUID if it's only one system anyway.
    // If there were multiple, we could specify it with:
    // dc.system(uint64_t uuid);
    System &system = dc.system();
    auto action = std::make_shared<Action>(system);
    auto mission = std::make_shared<Mission>(system);
    auto telemetry = std::make_shared<Telemetry>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "Waiting for system to be ready" << std::endl;
        sleep_for(seconds(1));
    }

    std::cout << "System ready" << std::endl;

    // Import Mission items from QGC plan
    Mission::mission_items_t mission_items;
    Mission::Result import_res = Mission::import_qgroundcontrol_mission(mission_items, qgc_plan);
    handle_mission_err_exit(import_res, "Failed to import mission items: ");

    if (mission_items.size() == 0) {
        std::cerr << "No missions! Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Found " << mission_items.size() << " mission items in the given QGC plan." <<
              std::endl;

    {
        std::cout << "Uploading mission..." << std::endl;
        // Wrap the asynchronous upload_mission function using std::future.
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        mission->upload_mission_async(
        mission_items, [prom](Mission::Result result) {
            prom->set_value(result);
        });

        const Mission::Result result = future_result.get();
        handle_mission_err_exit(result, "Mission upload failed: ");
        std::cout << "Mission uploaded." << std::endl;
    }

    std::cout << "Arming..." << std::endl;
    const ActionResult arm_result = action->arm();
    handle_action_err_exit(arm_result, "Arm failed: ");
    std::cout << "Armed." << std::endl;

    // Before starting the mission subscribe to the mission progress.
    mission->subscribe_progress(
    [](int current, int total) {
        std::cout << "Mission status update: " << current << " / " << total << std::endl;
    });

    {
        std::cout << "Starting mission." << std::endl;
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        mission->start_mission_async(
        [prom](Mission::Result result) {
            prom->set_value(result);
            std::cout << "Started mission." << std::endl;
        });

        const Mission::Result result = future_result.get();
        handle_mission_err_exit(result, "Mission start failed: ");
    }

    while (!mission->mission_finished()) {
        sleep_for(seconds(1));
    }

    // Wait for some time.
    sleep_for(seconds(5));

    {
        // Mission complete. Command RTL to go home.
        std::cout << "Commanding RTL..." << std::endl;
        const ActionResult result = action->return_to_launch();
        if (result != ActionResult::SUCCESS) {
            std::cout << "Failed to command RTL (" << action_result_str(result) << ")" << std::endl;
        } else {
            std::cout << "Commanded RTL." << std::endl;
        }
    }

    return 0;
}

inline void handle_action_err_exit(ActionResult result, const std::string &message)
{
    if (result != ActionResult::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << action_result_str(
                      result) << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

inline void handle_mission_err_exit(Mission::Result result, const std::string &message)
{
    if (result != Mission::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << Mission::result_str(
                      result) << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Handles connection result
inline void handle_connection_err_exit(ConnectionResult result,
                                       const std::string &message)
{
    if (result != ConnectionResult::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message
                  << connection_result_str(result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}
