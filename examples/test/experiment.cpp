/*************************Summary***************************
Function: Rocket recycle throught MAVSDK
1. Connect to PX4 through Serial serial:///dev/ttyUSB0:57600;
2. Check the overload of PX4, if it is over 3g, start timing;
3. 00:05:00, fired the parachute;
3. 00:06:00, unfold the arms;
4. 00:07:00, armed the vehicle;
5. 00:07:30, started the return mission;
6. after the mission, disarmed the vehicle;
Author: YDM <1779876755@qq.com>
************************************************************/

/*****************Subscribe Message Frequency*******************
        NAME          Frequency            Rate Function            Callback
LOCAL_POSITION_NED      1HZ     set_rate_position_velocity_ned  subscribe_position_velocity_ned
GLOBAL_POSITION_INT     1HZ     set_rate_position               subscribe_position
HIGHRES_IMU             5HZ     set_rate_imu                    subscribe_imu


***************************************************************/

/*****************Advertise Message Frequency*******************
        NAME          Frequency            Rate Function            Callback
LOCAL_POSITION_NED      1HZ     set_rate_position_velocity_ned  subscribe_position_velocity_ned


***************************************************************/

/*****************Heard file and Namespace*********************/
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <future>
#include <memory>
#include <thread>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/mission/mission.h>
#include <mavsdk/plugins/telemetry/telemetry.h>


using namespace mavsdk;
using namespace std::placeholders; // for `_1`
using namespace std::chrono; // for seconds(), milliseconds()
using namespace std::this_thread; // for sleep_for()

#define ERROR_CONSOLE_TEXT "\033[31m" // Turn text on console red
#define TELEMETRY_CONSOLE_TEXT "\033[34m" // Turn text on console blue
#define NORMAL_CONSOLE_TEXT "\033[0m" // Restore normal console colour
/**************************************************************/

/**************************Exit Function*********************/
// Handles Action's result
inline void handle_action_err_exit(Action::Result result, const std::string& message);
// Handles Mission's result
inline void handle_mission_err_exit(Mission::Result result, const std::string& message);
// Handles Connection result
inline void handle_connection_err_exit(ConnectionResult result, const std::string& message);
/**************************************************************/

static Mission::MissionItem make_mission_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    Mission::MissionItem::CameraAction camera_action);

/*************************How to use it************************/
void usage(std::string bin_name)
{
    std::cout << NORMAL_CONSOLE_TEXT << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}

/*************************Main Function**************************/
int main(int argc, char** argv)
{
    // Initialing variable
    Mavsdk mavsdk;
    std::string connection_url;
    ConnectionResult connection_result;
    Telemetry::AccelerationFrd imu_current;
    float acc_x = 0.0;
    float acc_y = 0.0;
    float acc_z = 0.0;
    float now_alt = 0.0;            //for recording alt and checking land
    float pre_alt = 0.0;
    float pre_pre_alt = 0.0;

/**************************Connecting the vehicle*********************************/

    if (argc == 2) {
        connection_url = argv[1];
        connection_result = mavsdk.add_any_connection(connection_url);
    } else {
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Connection failed: " << connection_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    std::cout << "Waiting to discover system..." << std::endl;
    auto prom = std::promise<std::shared_ptr<System>>{};
    auto fut = prom.get_future();

    mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
        auto system = mavsdk.systems().back();

        if (system->has_autopilot()) {
            std::cout << "Discovered autopilot" << std::endl;

            // Unsubscribe again as we only want to find one system.
            mavsdk.subscribe_on_new_system(nullptr);
            prom.set_value(system);
        }
    });

    // We usually receive heartbeats at 1Hz, therefore we should find a
    // system after around 3 seconds.
    if (fut.wait_for(seconds(3)) == std::future_status::timeout) {
        std::cout << ERROR_CONSOLE_TEXT << "No autopilot found, exiting." << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

    auto system = fut.get();

    auto telemetry = Telemetry{system};
    auto action = Action{system};
    auto mission = Mission(system);

/***********************************************************************************/

/**************************Subscribe Message and Callback***************************/

    // We want to listen to the altitude of the drone at 1 Hz in local coordinate system.
    const Telemetry::Result set_local_position_rate_result = telemetry.set_rate_position_velocity_ned(1.0);
    if (set_local_position_rate_result != Telemetry::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Setting rate failed:" << set_local_position_rate_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry.subscribe_position_velocity_ned([](Telemetry::PositionVelocityNed local_position) {
        std::cout << TELEMETRY_CONSOLE_TEXT // set to blue
                  << "Local Altitude: " << local_position.position.down_m << " m"
                  << NORMAL_CONSOLE_TEXT // set to default color again
                  << std::endl;
    });

    // We want to listen to the altitude of the drone at 1 Hz in global coordinate system.
    const Telemetry::Result set_rate_result = telemetry.set_rate_position(1.0);
    if (set_rate_result != Telemetry::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Setting rate failed:" << set_rate_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry.subscribe_position([&now_alt, &pre_alt, &pre_pre_alt](Telemetry::Position position) {
        std::cout << TELEMETRY_CONSOLE_TEXT // set to blue
                  << "Global Altitude: " << position.relative_altitude_m << " m"
                  << NORMAL_CONSOLE_TEXT // set to default color again
                  << std::endl;
        pre_alt = now_alt;
        pre_pre_alt = pre_alt;
        now_alt = position.relative_altitude_m;
    });

    // We want to listen to the acceleration of the drone at 5 Hz in body coordinate system(frd).
    const Telemetry::Result set_imu_rate_result = telemetry.set_rate_imu(5.0);
    if (set_imu_rate_result != Telemetry::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Setting rate failed:" << set_imu_rate_result
                  << NORMAL_CONSOLE_TEXT << std::endl;
        return 1;
    }

    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry.subscribe_imu([&acc_x, &acc_y, &acc_z](Telemetry::Imu imu) {
        acc_x = imu.acceleration_frd.forward_m_s2 ;
        acc_y = imu.acceleration_frd.right_m_s2 ;
        acc_z = imu.acceleration_frd.down_m_s2 ;
        std::cout << TELEMETRY_CONSOLE_TEXT // set to blue
                  << "ACC_X: " << acc_x << " m/s^2"
                  << "ACC_Y: " << acc_y << " m/s^2"
                  << "ACC_Z: " << acc_z << " m/s^2"
                  << NORMAL_CONSOLE_TEXT // set to default color again
                  << std::endl;

    });

    // Set up callback to monitor flight mode 'changes'
    Telemetry::FlightMode oldFlightMode=Telemetry::FlightMode::Unknown;
    telemetry.subscribe_flight_mode([&oldFlightMode](Telemetry::FlightMode flightMode) {
    if (oldFlightMode != flightMode) {
        //Flight mode changed. Print!
        std::cout << "FlightMode: " << flightMode << '\n';
        oldFlightMode=flightMode; //Save updated mode.
        }
    });

/***********************************************************************************/

/********************************Flight Timing*************************************/
    // Check if vehicle is ready to arm
    // while (telemetry.health_all_ok() != true) {
    //     std::cout << "Vehicle is getting ready to arm" << std::endl;
    //     sleep_for(seconds(1));
    // }

/*************************Check for the fire signal**********************************/
    while(sqrt(acc_x * acc_x + acc_y * acc_y + acc_z * acc_z) < 15)
    {
        sleep_for(std::chrono::milliseconds(200));
        std::cout << "Waiting for launch..." << std::endl;
    }
/*************************Check for the fire signal**********************************/
    // while(sqrt(acc_x * acc_x + acc_y * acc_y + acc_z * acc_z) < 0.01 || 
    //         sqrt(acc_x * acc_x + acc_y * acc_y + acc_z * acc_z) > 5)
    // {
    //     sleep_for(std::chrono::milliseconds(200));
    //     std::cout << "Waiting for launch..." << std::endl;
    // }
/********************************Unfold Arms*************************************/
    std::cout << "Sleeping for 4.0 seconds..." << std::endl;
    sleep_for(seconds(4));

    // std::cout << "Unfolding Arms...\n";
    // const Action::Result set_actuator0_result = action.set_actuator(1, 1.0);

    // if (set_actuator0_result != Action::Result::Success) {
    //     std::cerr << "Setting actuator failed:" << set_actuator0_result << '\n';
    //     return 1;
    // }
    // std::cout << "Unfolded Arms !!!\n";
/*************************Arm vehicle**********************************/
    // std::cout << "Sleeping for 2 seconds..." << std::endl;
    // sleep_for(seconds(2));
    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = action.arm();

    if (arm_result != Action::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Arming failed:" << arm_result << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }

/********************************Fire Parachute*************************************/
    std::cout << "Sleeping for 0.5 seconds..." << std::endl;
    sleep_for(milliseconds(500));

    // std::cout << "Firing Parachute...\n";
    // const Action::Result set_actuator1_result = action.set_actuator(2, 0.8);

    // if (set_actuator1_result != Action::Result::Success) {
    //     std::cerr << "Setting actuator failed:" << set_actuator1_result << '\n';
    //     return 1;
    // }
    // std::cout << "Fired Parachute !!!\n";

/***************************Started Mission***************************************/
{
    std::cout << "Creating and uploading mission" << std::endl;

    std::vector<Mission::MissionItem> mission_items;

    mission_items.push_back(make_mission_item(
        34.5786541,
        109.9553620,
        0.5f,
        3.0f,
        false,
        20.0f,
        60.0f,
        Mission::MissionItem::CameraAction::None));

    {
        std::cout << "Uploading mission..." << std::endl;
        // We only have the upload_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        Mission::MissionPlan mission_plan{};
        mission_plan.mission_items = mission_items;
        mission.upload_mission_async(
            mission_plan, [prom](Mission::Result result) { prom->set_value(result); });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::Success) {
            std::cout << "Mission upload failed (" << result << "), exiting." << std::endl;
            return 1;
        }
        std::cout << "Mission uploaded." << std::endl;
    }

}
    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    mission.subscribe_mission_progress([](Mission::MissionProgress mission_progress) {
        std::cout << "Mission status update: " << mission_progress.current << " / "
                  << mission_progress.total << std::endl;
    });

    {
        std::cout << "Starting mission." << std::endl;
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        mission.start_mission_async([prom](Mission::Result result) {
            prom->set_value(result);
            std::cout << "Started mission." << std::endl;
        });

        const Mission::Result result = future_result.get();
        handle_mission_err_exit(result, "Mission start failed: ");
    }

{
    while (!mission.is_mission_finished().second) {
        sleep_for(seconds(1));
    }

    /***************************Land********************************/
    // std::cout << "Sleeping for 1 seconds..." << std::endl;

    std::cout << "Landing..." << std::endl;
    const Action::Result land_result = action.land();
    if (land_result != Action::Result::Success) {
        std::cout << ERROR_CONSOLE_TEXT << "Land failed:" << land_result << NORMAL_CONSOLE_TEXT
                  << std::endl;
        return 1;
    }
}
    // Check if vehicle is still in air
    while (telemetry.in_air()) {
        std::cout << "Vehicle is landing..." << std::endl;
        sleep_for(seconds(1));

        
        // if(pre_alt - now_alt < 0.1 && pre_pre_alt - pre_alt < 0.1){

        //     std::cout << "Deteced Landed!!!" << std::endl;

        //     /********************************KIll*************************************/
        //     std::cout << "Kill Rotors..." << std::endl;
        //     const Action::Result kill_result = action.terminate();
        //     if (kill_result != Action::Result::Success) {
        //         std::cout << ERROR_CONSOLE_TEXT << "Kill failed:" << kill_result
        //                 << NORMAL_CONSOLE_TEXT << std::endl;
        //         return 1;
        //     }

        //     return 1;
        // }

    }
    std::cout << "Landed!" << std::endl;

    // We are relying on auto-disarming but let's keep watching the telemetry for a bit longer.
    sleep_for(seconds(1));
    std::cout << "Finished..." << std::endl;

    return 0;
}

inline void handle_action_err_exit(Action::Result result, const std::string& message)
{
    if (result != Action::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

inline void handle_mission_err_exit(Mission::Result result, const std::string& message)
{
    if (result != Mission::Result::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

// Handles connection result
inline void handle_connection_err_exit(ConnectionResult result, const std::string& message)
{
    if (result != ConnectionResult::Success) {
        std::cerr << ERROR_CONSOLE_TEXT << message << result << NORMAL_CONSOLE_TEXT << std::endl;
        exit(EXIT_FAILURE);
    }
}

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