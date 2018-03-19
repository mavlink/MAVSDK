
#include <chrono>
#include <cstdint>
#include <dronecore/action.h>
#include <dronecore/dronecore.h>
#include <dronecore/telemetry.h>
#include <iostream>
#include <thread>

using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using namespace dronecore;

#define ERROR_CONSOLE_TEXT "\033[31m" //Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" //Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m"  //Restore normal console colour

int main(int /*argc*/, char ** /*argv*/)
{
    DroneCore dc;

    bool discovered_system = false;

    ConnectionResult connection_result = dc.add_udp_connection();

    if (connection_result != ConnectionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Connection failed: "
                  << connection_result_str(connection_result)
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    std::cout << "Waiting to discover system..." << std::endl;
    dc.register_on_discover([&discovered_system](uint64_t uuid) {
        std::cout << "Discovered system with UUID: " << uuid << std::endl;
        discovered_system = true;
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a system after around 2 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (!discovered_system) {
        std::cout << ERROR_CONSOLE_TEXT << "No system found, exiting." << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // We don't need to specify the UUID if it's only one system anyway.
    // If there were multiple, we could specify it with:
    // dc.system(uint64_t uuid);
    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);

    // We want to listen to the altitude of the drone at 1 Hz.
    const Telemetry::Result set_rate_result = telemetry->set_rate_position(1.0);
    if (set_rate_result != Telemetry::Result::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Setting rate failed:" << Telemetry::result_str(
                      set_rate_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }


    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry->position_async([](Telemetry::Position position) {
        std::cout << TELEMETRY_CONSOLE_TEXT // set to blue
                  << "Altitude: " << position.relative_altitude_m << " m"
                  << NORMAL_CONSOLE_TEXT // set to default color again
                  << std::endl;
    });


    // Check if vehicle is ready to arm
    if (telemetry->health_all_ok() != true) {
        std::cout << ERROR_CONSOLE_TEXT << "Vehicle not ready to arm" << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    auto action = std::make_shared<Action>(system);

    // Arm vehicle
    std::cout << "Arming..." << std::endl;
    const ActionResult arm_result = action->arm();

    if (arm_result != ActionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Arming failed:" << action_result_str(
                      arm_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Take off
    std::cout << "Taking off..." << std::endl;
    const ActionResult takeoff_result = action->takeoff();
    if (takeoff_result != ActionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Takeoff failed:" << action_result_str(
                      takeoff_result) << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Wait
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Transition to fixedwing..." << std::endl;
    const ActionResult fw_result = action->transition_to_fixedwing();

    if (fw_result != ActionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Transition to fixed wing failed: " << action_result_str(
                      fw_result) << NORMAL_CONSOLE_TEXT << std::endl;
        //return 1;
    }

    // Wait
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "Transition back to multicopter..." << std::endl;
    const ActionResult mc_result = action->transition_to_multicopter();
    if (mc_result != ActionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Transition to multi copter failed:" << action_result_str(
                      mc_result) << NORMAL_CONSOLE_TEXT << std::endl;
        //    return 1;
    }

    // Wait
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Return to launch
    std::cout << "Return to launch..." << std::endl;
    const ActionResult rtl_result = action->return_to_launch();
    if (rtl_result != ActionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Returning to launch failed:" << action_result_str(
                      rtl_result) << NORMAL_CONSOLE_TEXT << std::endl;
        //    return 1;
    }

    // Wait
    std::this_thread::sleep_for(std::chrono::seconds(20));

    // Land
    std::cout << "Landing..." << std::endl;
    const ActionResult land_result = action->land();
    if (land_result != ActionResult::SUCCESS) {
        std::cout << ERROR_CONSOLE_TEXT << "Land failed:" << action_result_str(
                      land_result) << NORMAL_CONSOLE_TEXT << std::endl;
        //    return 1;
    }

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cout << "Finished..." << std::endl;
    return 0;
}
