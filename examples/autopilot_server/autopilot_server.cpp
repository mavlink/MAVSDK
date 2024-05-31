#include <future>
#include <iostream>
#include <thread>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/param_server/param_server.h>
#include <mavsdk/plugins/param/param.h>
#include <mavsdk/plugins/telemetry_server/telemetry_server.h>
#include <mavsdk/plugins/action_server/action_server.h>
#include <mavsdk/plugins/mission_raw_server/mission_raw_server.h>
#include <mavsdk/plugins/mission/mission.h>

/*
 This example runs a MAVLink "autopilot" utilising the MAVSDK server plugins
 on a seperate thread. This uses two MAVSDK instances, one GCS, one autopilot.

 The main thread acts as a GCS and reads telemetry, parameters, transmits across
 a mission, clears the mission, arms the vehicle and then triggers a vehicle takeoff.

 The autopilot thread handles all the servers and triggers callbacks, publishes telemetry,
 handles and stores parameters, prints received missions and sets the vehicle height to 10m on
 successful takeoff request.
*/

using namespace mavsdk;

using std::chrono::seconds;
using std::chrono::milliseconds;
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

int main(int argc, char** argv)
{
    // We run the server plugins on a seperate thread so we can use the main
    // thread as a ground station.
    std::thread autopilotThread([]() {
        mavsdk::Mavsdk mavsdkTester{
            mavsdk::Mavsdk::Configuration{mavsdk::Mavsdk::ComponentType::Autopilot}};

        auto result = mavsdkTester.add_any_connection("udp://127.0.0.1:14551");
        if (result == mavsdk::ConnectionResult::Success) {
            std::cout << "Connected autopilot server side!" << std::endl;
        }

        auto server_component = mavsdkTester.server_component();

        // Create server plugins
        auto paramServer = mavsdk::ParamServer{server_component};
        auto telemServer = mavsdk::TelemetryServer{server_component};
        auto actionServer = mavsdk::ActionServer{server_component};

        // These are needed for MAVSDK at the moment
        paramServer.provide_param_int("CAL_ACC0_ID", 1);
        paramServer.provide_param_int("CAL_GYRO0_ID", 1);
        paramServer.provide_param_int("CAL_MAG0_ID", 1);
        paramServer.provide_param_int("SYS_HITL", 0);
        paramServer.provide_param_int("MIS_TAKEOFF_ALT", 0);
        // Add a custom param
        paramServer.provide_param_int("my_param", 1);

        // Allow the vehicle to change modes, takeoff and arm
        actionServer.set_allowable_flight_modes({true, true, true});
        actionServer.set_allow_takeoff(true);
        actionServer.set_armable(true, true);

        // Create vehicle telemetry info
        TelemetryServer::Position position{55.953251, -3.188267, 0, 0};
        TelemetryServer::PositionVelocityNed positionVelocityNed{{0, 0, 0}, {0, 0, 0}};
        TelemetryServer::VelocityNed velocity{};
        TelemetryServer::Heading heading{60};
        TelemetryServer::RawGps rawGps{
            0, 55.953251, -3.188267, 0, NAN, NAN, 0, NAN, 0, 0, 0, 0, 0, 0};
        TelemetryServer::GpsInfo gpsInfo{11, TelemetryServer::FixType::Fix3D};
        TelemetryServer::Battery battery;

        // Publish home already, so that it is available.
        telemServer.publish_home(position);

        // Create a mission raw server
        // This will allow us to receive missions from a GCS
        auto missionRawServer = mavsdk::MissionRawServer{server_component};
        std::cout << "MissionRawServer created" << std::endl;

        auto mission_prom = std::promise<MissionRawServer::MissionPlan>{};
        MissionRawServer::IncomingMissionHandle handle =
            missionRawServer.subscribe_incoming_mission(
                [&mission_prom, &missionRawServer, &handle](
                    MissionRawServer::Result res, MissionRawServer::MissionPlan plan) {
                    std::cout << "Received Uploaded Mission!" << std::endl;
                    std::cout << plan << std::endl;
                    // Unsubscribe so we only recieve one mission
                    missionRawServer.unsubscribe_incoming_mission(handle);
                    mission_prom.set_value(plan);
                });
        missionRawServer.subscribe_current_item_changed([](MissionRawServer::MissionItem item) {
            std::cout << "Current Mission Item Changed!" << std::endl;
            std::cout << "Current Item: " << item << std::endl;
        });
        missionRawServer.subscribe_clear_all(
            [](uint32_t clear_all) { std::cout << "Clear All Mission!" << std::endl; });
        auto plan = mission_prom.get_future().get();

        // Set current item to complete to progress the current item state
        missionRawServer.set_current_item_complete();

        // As we're acting as an autopilot, lets just make the vehicle jump to 10m altitude on
        // successful takeoff
        actionServer.subscribe_takeoff([&position](ActionServer::Result result, bool takeoff) {
            if (result == ActionServer::Result::Success) {
                position.relative_altitude_m = 10;
            }
        });

        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // Publish the telemetry
            telemServer.publish_home(position);
            telemServer.publish_sys_status(battery, true, true, true, true, true);
            telemServer.publish_position(position, velocity, heading);
            telemServer.publish_position_velocity_ned(positionVelocityNed);
            telemServer.publish_raw_gps(rawGps, gpsInfo);

            // Just a silly test.
            telemServer.publish_unix_epoch_time(42);
        }
    });

    // Now this is the main thread, we run client plugins to act as the GCS
    // to communicate with the autopilot server plugins.
    mavsdk::Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    auto result = mavsdk.add_any_connection("udp://:14551");
    if (result == mavsdk::ConnectionResult::Success) {
        std::cout << "Connected!" << std::endl;
    }

    auto prom = std::promise<std::shared_ptr<mavsdk::System>>{};
    auto fut = prom.get_future();
    Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, &prom, &handle]() {
        auto system = mavsdk.systems().back();

        if (system->has_autopilot()) {
            std::cout << "Discovered Autopilot from Client" << std::endl;

            // Unsubscribe again as we only want to find one system.
            mavsdk.unsubscribe_on_new_system(handle);
            prom.set_value(system);
        } else {
            std::cout << "No MAVSDK found" << std::endl;
        }
    });

    if (fut.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
        std::cout << "No autopilot found, exiting" << std::endl;
        return 1;
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));

    auto system = fut.get();

    // Create plugins
    auto action = mavsdk::Action{system};
    auto param = mavsdk::Param{system};
    auto telemetry = mavsdk::Telemetry{system};

    std::this_thread::sleep_for(std::chrono::seconds(5));
    auto mission = mavsdk::Mission{system};
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Check for our custom param we have set in the server thread
    auto res = param.get_param_int("MY_PARAM");
    if (res.first == mavsdk::Param::Result::Success) {
        std::cout << "Found Param MY_PARAM: " << res.second << std::endl;
    }

    // Create a mission to send to our mission server
    std::cout << "Creating Mission" << std::endl;
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
    {
        std::cout << "Uploading mission..." << std::endl;
        // We only have the upload_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        Mission::MissionPlan mission_plan{};
        mission_plan.mission_items = mission_items;
        std::cout << "SystemID " << system->get_system_id() << std::endl;
        mission.upload_mission_async(
            mission_plan, [prom](Mission::Result result) { prom->set_value(result); });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::Success) {
            std::cout << "Mission upload failed (" << result << "), exiting." << std::endl;
            return 1;
        }
        std::cout << "Mission uploaded." << std::endl;

        // Now clear the mission!
        mission.clear_mission_async(
            [](Mission::Result callback) { std::cout << "Clear Mission Request" << std::endl; });
        mission.subscribe_mission_progress([](Mission::MissionProgress progress) {
            std::cout << "Current: " << progress.current << std::endl;
            std::cout << "Total: " << progress.total << std::endl;
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(20));

    // We want to listen to the altitude of the drone at 1 Hz.
    auto set_rate_result = telemetry.set_rate_position(1.0);
    if (set_rate_result != mavsdk::Telemetry::Result::Success) {
        std::cout << "Setting rate failed:" << set_rate_result << std::endl;
        return 1;
    }

    // Set up callback to monitor altitude while the vehicle is in flight
    telemetry.subscribe_position([](mavsdk::Telemetry::Position position) {
        std::cout << "Altitude: " << position.relative_altitude_m << " m" << std::endl;
    });

    // Set up callback to monitor Unix time
    telemetry.subscribe_unix_epoch_time([](uint64_t time_us) {
        std::cout << "Unix epoch time: " << time_us << " us" << std::endl;
    });

    // Check if vehicle is ready to arm
    while (telemetry.health_all_ok() != true) {
        std::cout << "Vehicle is getting ready to arm" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Arm vehicle
    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = action.arm();

    if (arm_result != Action::Result::Success) {
        std::cout << "Arming failed:" << arm_result << std::endl;
        return 1;
    }

    action.set_takeoff_altitude(10.f);

    // Take off
    std::cout << "Taking off..." << std::endl;
    bool takenOff = false;
    while (true) {
        const Action::Result takeoff_result = action.takeoff();
        if (takeoff_result != Action::Result::Success) {
            std::cout << "Takeoff failed!:" << takeoff_result << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        break;
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
