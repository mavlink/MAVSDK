/**
* @file fly_mission.cpp
*
* @brief Demonstrates how add & fly waypoint missions from QGC plan using DroneCore.
* The example is summarised below:
* 1. Imports mission items from QGroundControl plan file.
* 2. Starts mission from first mission item.
* 3. Illustrates Pause/Resume mission item.
* 4. Exits after the mission is accomplished.
* Note: Before you run this example, plan your missions in QGroundControl & save them to a file.
*
* @author Julian Oes <julian@oes.ch>,
*         Shakthi Prashanth M <shakthi.prashanth.m@intel.com>

* @date 2017-09-06
*/


#include <dronecore/action.h>
#include <dronecore/dronecore.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <functional>
#include <future>
#include <iostream>
#include <memory>

using namespace dronecore;
using namespace std::placeholders; // for `_1`
using namespace std::this_thread; // for sleep_for()
using namespace std::chrono; // for milliseconds(), seconds().

int main(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Usage: fly_mission <path of QGC mission plan file>" << std::endl;
        exit(EXIT_FAILURE);
    }

    DroneCore dc;

    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        std::cout << "Waiting to discover device..." << std::endl;
        dc.register_on_discover([prom](uint64_t uuid) {
            std::cout << "Discovered device with UUID: " << uuid << std::endl;
            prom->set_value();
        });

        DroneCore::ConnectionResult connection_result = dc.add_udp_connection();
        if (connection_result != DroneCore::ConnectionResult::SUCCESS) {
            std::cout << "Connection failed: " << DroneCore::connection_result_str(
                          connection_result) << std::endl;
            exit(EXIT_FAILURE);
        }

        future_result.get();
    }

    dc.register_on_timeout([](uint64_t uuid) {
        std::cout << "Device with UUID timed out: " << uuid << std::endl;
        std::cout << "Exiting." << std::endl;
        exit(EXIT_SUCCESS);
    });

    // We don't need to specifiy the UUID if it's only one device anyway.
    // If there were multiple, we could specify it with:
    // dc.device(uint64_t uuid);
    Device &device = dc.device();
    std::shared_ptr<Action> action = std::make_shared<Action>(&device);
    std::shared_ptr<Mission> mission = std::make_shared<Mission>(&device);
    std::shared_ptr<Telemetry> telemetry = std::make_shared<Telemetry>(&device);

    while (!telemetry->health_all_ok()) {
        std::cout << "Waiting for device to be ready" << std::endl;
        sleep_for(seconds(1));
    }
    std::cout << "Device ready" << std::endl;

    // Import Mission items from QGC plan
    Mission::mission_items_t mission_items;
    std::string qgc_plan(argv[1]);
    Mission::Result import_res = Mission::import_mission_items_from_QGC_plan(mission_items, qgc_plan);
    if (import_res != Mission::Result::SUCCESS) {
        std::cerr << "Failed to import mission items from QGC plan! "
                  "Error: " << Mission::result_str(import_res) <<
                  ". Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (mission_items.size() == 0) {
        std::cerr << "No missions! Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Found " << mission_items.size() << " mission items in the given QGC plan." <<
              std::endl;

    {
        std::cout << "Uploading mission..." << std::endl;
        // We only have the upload_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        mission->upload_mission_async(
        mission_items, [prom](Mission::Result result) {
            prom->set_value(result);
        });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Mission upload failed (" << Mission::result_str(result) << "), exiting." << std::endl;
            exit(EXIT_FAILURE);
        }
        std::cout << "Mission uploaded." << std::endl;
    }

    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = action->arm();
    if (arm_result != Action::Result::SUCCESS) {
        std::cout << "Arming failed (" << Action::result_str(arm_result) << "), exiting." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Armed." << std::endl;

    std::atomic<bool> want_to_pause {false};
    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    mission->subscribe_progress(
    [&want_to_pause](int current, int total) {
        std::cout << "Mission status update: " << current << " / " << total << std::endl;

        if (current >= 2) {
            // We can only set a flag here. If we do more request inside the callback,
            // we risk blocking the system.
            want_to_pause = true;
        }
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
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Mission start failed (" << Mission::result_str(result) << "), exiting." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    while (!want_to_pause) {
        sleep_for(seconds(1));
    }

    {
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();

        std::cout << "Pausing mission..." << std::endl;
        mission->pause_mission_async(
        [prom](Mission::Result result) {
            prom->set_value(result);
        });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Failed to pause mission (" << Mission::result_str(result) << ")" << std::endl;
        } else {
            std::cout << "Mission paused." << std::endl;
        }
    }

    // Pause for 5 seconds.
    sleep_for(seconds(5));

    // Then continue.
    {
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();

        std::cout << "Resuming mission..." << std::endl;
        mission->start_mission_async(
        [prom](Mission::Result result) {
            prom->set_value(result);
        });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Failed to resume mission (" << Mission::result_str(result) << ")" << std::endl;
        } else {
            std::cout << "Resumed mission." << std::endl;
        }
    }

    while (!device.mission().mission_finished()) {
        sleep_for(seconds(1));
    }

    {
        // We are done, and can do RTL to go home.
        std::cout << "Commanding RTL..." << std::endl;
        const Action::Result result = action->return_to_launch();
        if (result != Action::Result::SUCCESS) {
            std::cout << "Failed to command RTL (" << Action::result_str(result) << ")" << std::endl;
        } else {
            std::cout << "Commanded RTL." << std::endl;
        }
    }

    // We need to wait a bit, otherwise the armed state might not be correct yet.
    sleep_for(seconds(2));

    while (telemetry->armed()) {
        // Wait until we're done.
        sleep_for(seconds(1));
    }
    std::cout << "Disarmed, exiting." << std::endl;
}
