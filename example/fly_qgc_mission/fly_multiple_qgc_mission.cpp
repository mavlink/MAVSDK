/**
* @file fly_multiple_qgc_mission.cpp
*
* @brief Demonstrates how to import multiple mission from QGroundControl plan,
* and fly them using DroneCore.
*
* Steps to run this example:
* 1. (a) Create a Mission in QGroundControl and save them to a file (.plan) (OR)
*    (b) Use a pre-created sample mission plan in "plugins/mission/qgroundcontrol_sample.plan".
* 2. Run the example by passing path of the QGC mission plan as argument (By default, sample mission plan is imported).
*
* Example description:
* 1. Imports next QGC mission from command line argument. (If there are none, it will fly a default one.)
* 2. Uploads mission items to vehicle.
* 3. Starts mission from first mission item.
* 4. Waits for the mission to complete.
* 5. Goto next mission: Repeat steps 2-4 for each mission.
* 6. Exit when there are no mission left.
*
* @author Shakthi Prashanth M <shakthi.prashanth.m@intel.com>

* @date 2018-02-21
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

// Does discover, add connection
void do_init(DroneCore &dc);
// Fly a QGC mission plan
void fly_qgc_mission(System &system, const std::string &plan);
// Handles Action's result
inline void handle_action_err_exit(Action::Result result, const std::string &message);
// Handles Connection result
inline void handle_connection_err_exit(ConnectionResult result,
                                       const std::string &message);

int main(int argc, char **argv)
{
    std::vector<std::string> plans;

    if (argc == 1) {
        auto default_plan = "../../../plugins/mission/qgroundcontrol_sample.plan";
        std::cout << "Usage: " << argv[0] << " [plan-0] [plan-1]..." << std::endl;
        plans.push_back(default_plan); // default plan
        std::cout << "Importing mission from Default mission plan: " << default_plan << std::endl;
    } else {
        for (argv++; *argv; argv++) {
            std::cout << "Adding plan to the list: " << *argv << std::endl;
            plans.push_back(*argv);
        }
    }

    DroneCore dc;

    // discover, add connection.
    do_init(dc);

    System &system = dc.system();

    // Fly each QGC mission , one by one.
    for (auto plan : plans) {
        fly_qgc_mission(system, plan);
        // Wait for some time.
        std::cout << "Lets wait for 2 seconds before starting next mission." << std::endl;
        sleep_for(seconds(2));
        }

    return 0;
}

void do_init(DroneCore &dc)
{
    auto connection_result = dc.add_udp_connection();
    handle_connection_err_exit(connection_result, "Connection failed: ");

    while (!dc.is_connected()) {
        std::cout << "Wait for system to connect via heartbeat" << std::endl;
        sleep_for(seconds(1));
    }
}

inline void handle_action_err_exit(Action::Result result, const std::string &message)
{
    if (result != Action::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << Action::result_str(
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

void fly_qgc_mission(System &system, const std::string &plan)
{
    std::shared_ptr<Action> action = std::make_shared<Action>(system);
    std::shared_ptr<Mission> mission = std::make_shared<Mission>(system);
    std::shared_ptr<Telemetry> telemetry = std::make_shared<Telemetry>(system);
    auto i = 0;

    // Health check
    while (!telemetry->health_all_ok()) {
        std::cout << "Waiting for system to be ready" << std::endl;
        sleep_for(seconds(1));
    }

    std::cout << "Mission path[" << i++ << "]: " << plan << std::endl;

    // Import Mission items from QGC plan
    Mission::mission_items_t mission_items;
    Mission::Result mission_result = Mission::import_qgroundcontrol_mission(mission_items, plan);
    if (mission_result != Mission::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << "Failed to import mission items: "
                  << Mission::result_str(mission_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return;
    }

    if (mission_items.size() == 0) {
        std::cerr << "No mission items! Going to next mission..." << std::endl;
        return;
    }
    std::cout << "Found " << mission_items.size()
              << " mission items in the given QGC plan." << std::endl;

    {
        std::cout << "Uploading mission..." << std::endl;
        // Wrap the asynchronous upload_mission function using std::future.
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        mission->upload_mission_async(
        mission_items, [prom](Mission::Result result) {
            prom->set_value(result);
        });

        mission_result = future_result.get();
        if (mission_result != Mission::Result::SUCCESS) {
            std::cerr << ERROR_CONSOLE_TEXT << "Mission upload failed: "
                      << Mission::result_str(mission_result)
                      << NORMAL_CONSOLE_TEXT << std::endl;
            return;
        }
        std::cout << "Mission uploaded." << std::endl;
    }

    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = action->arm();
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

        mission_result = future_result.get();
        if (mission_result != Mission::Result::SUCCESS) {
            std::cerr << ERROR_CONSOLE_TEXT << "Mission start failed: "
                      << Mission::result_str(mission_result)
                      << NORMAL_CONSOLE_TEXT << std::endl;
            return;
        }
    }

    while (!mission->mission_finished()) {
        sleep_for(seconds(1));
    }
}
