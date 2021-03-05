/**
 * Example to connect multiple vehicles and make them follow their own separate
 * plan file. Also saves the telemetry information to csv files
 * ./fly_multiple_drones udp://:14540 udp://:14541 test1.plan test2.plan
 */

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission_raw/mission_raw.h>
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
 * How to Start Multiple Instances (for jMAVSim)
 *
 * Note: The following instructions to start multiple vehicles in simulations
 * are last tested for Firmware Version 1.9.0.
 *
 * To start multiple instances (on separate ports):
 *
 * Build PX4
 *     make px4_sitl_default
 *
 * Run sitl_multiple_run.sh, specifying the number of instances to start (e.g. 2):
 *     ./Tools/sitl_multiple_run.sh 2
 *
 * Start the first instance:
 *     ./Tools/jmavsim_run.sh -l
 *
 * Start subsequent instances, specifying the simulation TCP port for the instance:
 *     ./Tools/jmavsim_run.sh -p 4561 -l
 *
 * The port should be set to 4560+i for i in [0, N-1].
 *
 *
 * Steps to run this example:
 * 1. Run multiple simulations with different drones following the steps above
 * 2. Build the executable
 * 3. (a) Create a Mission in QGroundControl and save them to a file (.plan), or
 *    (b) Use a pre-created sample mission plan.
 * 4. Run the executable by passing the connection urls (ex. udp://:14540) and
 *    path of QGC mission plan as arguments Example: If you have test1.plan and
 *    test2.plan in "../../../plugins/mission/" and you are running two drones
 *    in udp://:14540 and udp://:14541 then you run the example as:
 *
 *   ./fly_multiple_drones udp://:14540 udp://:14541 test1.plan test2.plan
 *
 *
 * Note: The mission needs to end with RTL or land, otherwise it will get stuck
 *       waiting for the system to be disarmed.
 */

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour

static void complete_mission(std::string qgc_plan, std::shared_ptr<System> system);

static void handle_action_err_exit(Action::Result result, const std::string& message);

static void handle_mission_err_exit(MissionRaw::Result result, const std::string& message);

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

    Mavsdk mavsdk;

    // Half of argc is how many udp ports is being used
    size_t total_ports_used = argc / 2;

    // the loop below adds the number of ports the sdk monitors.
    // Loop must start from 1 since we are ignoring argv[0] which would be the name of the
    // executable
    for (size_t i = 1; i <= total_ports_used; ++i) {
        ConnectionResult connection_result = mavsdk.add_any_connection(argv[i]);
        if (connection_result != ConnectionResult::Success) {
            std::cerr << ERROR_CONSOLE_TEXT << "Connection error: " << connection_result
                      << NORMAL_CONSOLE_TEXT << std::endl;
            return 1;
        }
    }

    std::atomic<size_t> num_systems_discovered{0};

    std::cout << "Waiting to discover system..." << std::endl;
    mavsdk.subscribe_on_new_system([&mavsdk, &num_systems_discovered]() {
        const auto systems = mavsdk.systems();

        if (systems.size() > num_systems_discovered) {
            std::cout << "Discovered system" << std::endl;
            num_systems_discovered = systems.size();
        }
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
    for (auto system : mavsdk.systems()) {
        std::thread t(&complete_mission, argv[planFile_provided], system);
        threads.push_back(
            std::move(t)); // Instead of copying, move t into the vector (less expensive)
        planFile_provided += 1;
        sleep_for(seconds(1));
    }

    for (auto& t : threads) {
        t.join();
    }
    return 0;
}

void complete_mission(std::string qgc_plan, std::shared_ptr<System> system)
{
    auto telemetry = Telemetry{system};
    auto action = Action{system};
    auto mission_raw = MissionRaw{system};

    // We want to listen to the telemetry data at 1 Hz.
    const Telemetry::Result set_rate_result = telemetry.set_rate_position(1.0);

    if (set_rate_result != Telemetry::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << "Setting rate failed:" << set_rate_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return;
    }

    std::cout << "Importing mission from mission plan: " << qgc_plan << std::endl;

    // Creates a file named with vehicle's last few digits of uuid number to store lat and lng with
    // time
    std::ofstream myFile;
    myFile.open((std::to_string(system->get_system_id()) + ".csv"));
    myFile << "Time, Vehicle_ID, Altitude, Latitude, Longitude, Absolute_Altitude, \n";

    // Setting up the callback to monitor lat and longitude
    telemetry.subscribe_position([&](Telemetry::Position position) {
        myFile << getCurrentTimeString() << "," << (system->get_system_id()) << ","
               << position.relative_altitude_m << "," << position.latitude_deg << ","
               << position.longitude_deg << "," << position.absolute_altitude_m << ", \n";
    });

    // Check if vehicle is ready to arm
    while (telemetry.health_all_ok() != true) {
        std::cout << "Vehicle is getting ready to arm" << std::endl;
        sleep_for(seconds(1));
    }

    // Import Mission items from QGC plan
    std::pair<MissionRaw::Result, MissionRaw::MissionImportData> import_res =
        mission_raw.import_qgroundcontrol_mission(qgc_plan);
    handle_mission_err_exit(import_res.first, "Failed to import mission items: ");

    if (import_res.second.mission_items.size() == 0) {
        std::cerr << "No missions! Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::cout << "Found " << import_res.second.mission_items.size()
              << " mission items in the given QGC plan." << std::endl;

    std::cout << "Uploading mission..." << std::endl;
    const MissionRaw::Result upload_result =
        mission_raw.upload_mission(import_res.second.mission_items);
    handle_mission_err_exit(upload_result, "MissionRaw upload failed: ");
    std::cout << "MissionRaw uploaded." << std::endl;

    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = action.arm();
    handle_action_err_exit(arm_result, "Arm failed: ");
    std::cout << "Armed." << std::endl;

    // Before starting the mission subscribe to the mission progress.
    mission_raw.subscribe_mission_progress([&](MissionRaw::MissionProgress mission_progress) {
        std::cout << "Mission progress update, VehicleID: " << system->get_system_id() << " --> "
                  << mission_progress.current << " / " << mission_progress.total << std::endl;
    });

    std::cout << "Starting mission." << std::endl;
    const MissionRaw::Result start_result = mission_raw.start_mission();
    handle_mission_err_exit(start_result, "Mission start failed: ");
    std::cout << "Started mission." << std::endl;

    // This only works if the mission ends with an automatic RTL.
    while (telemetry.armed()) {
        sleep_for(seconds(1));
    }
}

static void handle_action_err_exit(Action::Result result, const std::string& message)
{
    if (result != Action::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

static void handle_mission_err_exit(MissionRaw::Result result, const std::string& message)
{
    if (result != MissionRaw::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}
