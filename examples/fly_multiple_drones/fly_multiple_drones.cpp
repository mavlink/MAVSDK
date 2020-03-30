/*
Example to connect multiple vehicles and make them follow their own separate plan file. Also
saves the telemetry information to csv files
./fly_multiple_drones udp://:14540 udp://:14541 ../../../plugins/mission/test.plan
../../../plugins/mission/test2.plan

Author: Julian Oes <julian@oes.ch>
Author: Shayaan Haider (via Slack)
Author: Shusil Shapkota (Air Matrix)
*/

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/telemetry/telemetry.h>

#include <cstdint>
#include <iostream>
#include <thread>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <ctime>
#include <fstream>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

/*
How to Start Multiple Instances (for jMAVSim)

Note: The following instructions to start multiple vehicles in simulations are for Firmware
Version 1.9.0. If you have other versions running on your machine, please visit this site and select
appropriate version for the instructions:
--> https://dev.px4.io/en/simulation/multi_vehicle_jmavsim.html

To start multiple instances (on separate ports):

Build PX4
    make px4_sitl_default

Run sitl_multiple_run.sh, specifying the number of instances to start (e.g. 2):
    ./Tools/sitl_multiple_run.sh 2

Start the first instance:
    ./Tools/jmavsim_run.sh -l

Start subsequent instances, specifying the simulation TCP port for the instance:
    ./Tools/jmavsim_run.sh -p 4561 -l

The port should be set to 4560+i for i in [0, N-1].


Steps to run this example:
1. Run multiple simulations with different drones following the steps explained above
2. build the executable
3. (a) Create a Mission in QGroundControl and save them to a file (.plan) (OR)
   (b) Use a pre-created sample mission plan.
4. Run the executable by passing the connection urls (ex. udp://:14540) and path of QGC mission plan
as arguments Example: If you have test.plan and test2.plan in "../../../plugins/mission/" and you
are running two drones in udp://:14540 and udp://:14541 then you run the example as:

./fly_multiple_drones udp://:14540 udp://:14541 ../../../src/plugins/mission/test.plan
../../../src/plugins/mission/test2.plan

*/

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

static void complete_mission(std::string qgc_plan, System& system);

static void handle_action_err_exit(Action::Result result, const std::string& message);

static void handle_mission_err_exit(Mission::Result result, const std::string& message);

std::string getCurrentTimeString()
{
    time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string s(30, '\0');
    strftime(&s[0], s.size(), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return s;
}

int main(int argc, char* argv[])
{
    // There needs to be odd number of arguments (including ./fly_multiple_drones) otherwise
    // it would suggest either plan files or udp ports hasn't been specified
    if (argc % 2 == 0 || argc == 1) {
        std::cerr
            << ERROR_CONSOLE_TEXT
            << "Please make sure you have specified the connections and plan files for each drones"
            << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    Mavsdk dc;

    // Half of argc is how many udp ports is being used
    int total_ports_used = argc / 2;

    // the loop below adds the number of ports the sdk monitors.
    // Loop must start from 1 since we are ignoring argv[0] which would be the name of the
    // executable
    for (int i = 1; i <= total_ports_used; ++i) {
        ConnectionResult connection_result = dc.add_any_connection(argv[i]);
        if (connection_result != ConnectionResult::SUCCESS) {
            std::cerr << ERROR_CONSOLE_TEXT
                      << "Connection error: " << connection_result_str(connection_result)
                      << NORMAL_CONSOLE_TEXT << std::endl;
            return 1;
        }
    }

    std::atomic<signed> num_systems_discovered{0};

    std::cout << "Waiting to discover system..." << std::endl;
    dc.register_on_discover([&num_systems_discovered](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        ++num_systems_discovered;
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a system after around 2
    // seconds.
    sleep_for(seconds(2));

    if (num_systems_discovered != total_ports_used) {
        std::cerr << ERROR_CONSOLE_TEXT << "Not all systems found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    std::vector<std::thread> threads;

    int planFile_provided =
        total_ports_used + 1; // +1 because first plan is specified at argv[total_ports_used+1]
    for (auto uuid : dc.system_uuids()) {
        System& system = dc.system(uuid);
        std::thread t(&complete_mission, argv[planFile_provided], std::ref(system));
        threads.push_back(
            std::move(t)); // Instead of copying, move t into the vector (less expensive)
        planFile_provided += 1;
    }

    for (auto& t : threads) {
        t.join();
    }
    return 0;
}

void complete_mission(std::string qgc_plan, System& system)
{
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);
    auto mission = std::make_shared<Mission>(system);

    // We want to listen to the telemetry data at 1 Hz.
    const Telemetry::Result set_rate_result = telemetry->set_rate_position(1.0);

    if (set_rate_result != Telemetry::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT
                  << "Setting rate failed:" << Telemetry::result_str(set_rate_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return;
    }

    std::cout << "Importing mission from mission plan: " << qgc_plan << std::endl;

    // Creates a file named with vehicle's last few digits of uuid number to store lat and lng with
    // time
    std::ofstream myFile;
    myFile.open((std::to_string(system.get_uuid() % 100000) + ".csv"));
    myFile << "Time, Vehicle_ID, Altitude, Latitude, Longitude, Absolute_Altitude, \n";

    // Setting up the callback to monitor lat and longitude
    telemetry->position_async([&](Telemetry::Position position) {
        myFile << getCurrentTimeString() << "," << (system.get_uuid()) % 100000 << ","
               << position.relative_altitude_m << "," << position.latitude_deg << ","
               << position.longitude_deg << "," << position.absolute_altitude_m << ", \n";
    });

    // Check if vehicle is ready to arm
    while (telemetry->health_all_ok() != true) {
        std::cout << "Vehicle is getting ready to arm" << std::endl;
        sleep_for(seconds(1));
    }

    // Import Mission items from QGC plan
    Mission::mission_items_t mission_items;
    Mission::Result import_res = Mission::import_qgroundcontrol_mission(mission_items, qgc_plan);
    handle_mission_err_exit(import_res, "Failed to import mission items: ");

    if (mission_items.size() == 0) {
        std::cerr << "No missions! Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Found " << mission_items.size() << " mission items in the given QGC plan."
              << std::endl;

    {
        std::cout << "Uploading mission..." << std::endl;
        // Wrap the asynchronous upload_mission function using std::future.
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        mission->upload_mission_async(
            mission_items, [prom](Mission::Result result) { prom->set_value(result); });

        const Mission::Result result = future_result.get();
        handle_mission_err_exit(result, "Mission upload failed: ");
        std::cout << "Mission uploaded." << std::endl;
    }

    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = action->arm();
    handle_action_err_exit(arm_result, "Arm failed: ");
    std::cout << "Armed." << std::endl;

    // Before starting the mission subscribe to the mission progress.
    mission->subscribe_progress([&](int current, int total) {
        std::cout << "Mission status update, VehicleID: " << system.get_uuid() % 100000 << " --> "
                  << current << " / " << total << std::endl;
    });

    {
        std::cout << "Starting mission." << std::endl;
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        mission->start_mission_async([prom](Mission::Result result) {
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
        const Action::Result result = action->return_to_launch();
        if (result != Action::Result::Success) {
            std::cout << "Failed to command RTL (" << Action::result_str(result) << ")"
                      << std::endl;
        } else {
            std::cout << "Commanded RTL." << std::endl;
        }
    }
}

static void handle_action_err_exit(Action::Result result, const std::string& message)
{
    if (result != Action::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << Action::result_str(result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

static void handle_mission_err_exit(Mission::Result result, const std::string& message)
{
    if (result != Mission::Result::SUCCESS) {
        std::cerr << ERROR_CONSOLE_TEXT << message << Mission::result_str(result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}
