//
// Example that demonstrates how add waypoint missions using DroneCore
//
// Author: Julian Oes <julian@oes.ch>, Shakthi Prashanth <shakthi.prashanth.m@intel.com>
//

#include <dronecore/dronecore.h>
#include <iostream>
#include <functional>
#include <memory>
#include <future>


using namespace dronecore;
using namespace std::placeholders; // for `_1`


static std::shared_ptr<MissionItem> add_mission_item(double latitude_deg,
                                                     double longitude_deg,
                                                     float relative_altitude_m,
                                                     float speed_m_s,
                                                     bool is_fly_through,
                                                     float gimbal_pitch_deg,
                                                     float gimbal_yaw_deg,
                                                     MissionItem::CameraAction camera_action);

static void receive_mission_progress(int current, int total);

static std::atomic<bool> _want_to_pause {false};

int main(int /*argc*/, char ** /*argv*/)
{
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
            return 1;
        }

        future_result.get();
    }

    // We don't need to specifiy the UUID if it's only one device anyway.
    // If there were multiple, we could specify it with:
    // dc.device(uint64_t uuid);
    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "Waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Device ready" << std::endl;
    std::cout << "Creating and uploading mission" << std::endl;

    std::vector<std::shared_ptr<MissionItem>> mission_items;

    mission_items.push_back(
        add_mission_item(47.398170327054473,
                         8.5456490218639658,
                         10.0f, 5.0f, false,
                         20.0f, 60.0f,
                         MissionItem::CameraAction::NONE));

    mission_items.push_back(
        add_mission_item(47.398241338125118,
                         8.5455360114574432,
                         10.0f, 2.0f, true,
                         0.0f, -60.0f,
                         MissionItem::CameraAction::TAKE_PHOTO));

    mission_items.push_back(
        add_mission_item(47.398139363821485, 8.5453846156597137,
                         10.0f, 5.0f, true,
                         -46.0f, 0.0f,
                         MissionItem::CameraAction::START_VIDEO));

    mission_items.push_back(
        add_mission_item(47.398058617228855,
                         8.5454618036746979,
                         10.0f, 2.0f, false,
                         -90.0f, 30.0f,
                         MissionItem::CameraAction::STOP_VIDEO));

    mission_items.push_back(
        add_mission_item(47.398100366082858,
                         8.5456969141960144,
                         10.0f, 5.0f, false,
                         -45.0f, -30.0f,
                         MissionItem::CameraAction::START_PHOTO_INTERVAL));

    mission_items.push_back(
        add_mission_item(47.398001890458097,
                         8.5455576181411743,
                         10.0f, 5.0f, false,
                         0.0f, 0.0f,
                         MissionItem::CameraAction::STOP_PHOTO_INTERVAL));

    {
        std::cout << "Uploading mission..." << std::endl;
        // We only have the send_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        device.mission().send_mission_async(
        mission_items, [prom](Mission::Result result) {
            prom->set_value(result);
            std::cout << "Mission uploaded." << std::endl;
        });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Mission upload failed (" << Mission::result_str(result) << "), exiting." << std::endl;
            return 1;
        }
    }

    std::cout << "Arming..." << std::endl;
    const Action::Result arm_result = device.action().arm();
    if (arm_result != Action::Result::SUCCESS) {
        std::cout << "Arming failed (" << Action::result_str(arm_result) << "), exiting." << std::endl;
        return 1;
    }

    std::cout << "Armed." << std::endl;

    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    // We pass on device to receive_mission_progress because we need it in the callback.
    device.mission().subscribe_progress(std::bind(&receive_mission_progress, _1, _2));

    {
        std::cout << "Starting mission." << std::endl;
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();
        device.mission().start_mission_async(
        [prom](Mission::Result result) {
            prom->set_value(result);
            std::cout << "Started mission." << std::endl;
        });

        const Mission::Result result = future_result.get();
        if (result != Mission::Result::SUCCESS) {
            std::cout << "Mission start failed (" << Mission::result_str(result) << "), exiting." << std::endl;
            return 1;
        }
    }

    while (!_want_to_pause) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    {
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();

        std::cout << "Pausing mission..." << std::endl;
        device.mission().pause_mission_async(
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
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Then continue.
    {
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto future_result = prom->get_future();

        std::cout << "Resuming mission..." << std::endl;
        device.mission().start_mission_async(
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
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    {
        // We are done, and can do RTL to go home.
        std::cout << "Commanding RTL..." << std::endl;
        const Action::Result result = device.action().return_to_launch();
        if (result != Action::Result::SUCCESS) {
            std::cout << "Failed to command RTL (" << Action::result_str(result) << ")" << std::endl;
        } else {
            std::cout << "Commanded RTL." << std::endl;
        }
    }

    // We need to wait a bit, otherwise the armed state might not be correct yet.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    while (device.telemetry().armed()) {
        // Wait until we're done.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << "Disarmed, exiting." << std::endl;
}

std::shared_ptr<MissionItem> add_mission_item(double latitude_deg,
                                              double longitude_deg,
                                              float relative_altitude_m,
                                              float speed_m_s,
                                              bool is_fly_through,
                                              float gimbal_pitch_deg,
                                              float gimbal_yaw_deg,
                                              MissionItem::CameraAction camera_action)
{
    std::shared_ptr<MissionItem> new_item(new MissionItem());
    new_item->set_position(latitude_deg, longitude_deg);
    new_item->set_relative_altitude(relative_altitude_m);
    new_item->set_speed(speed_m_s);
    new_item->set_fly_through(is_fly_through);
    new_item->set_gimbal_pitch_and_yaw(gimbal_pitch_deg, gimbal_yaw_deg);
    new_item->set_camera_action(camera_action);
    return new_item;
}

void receive_mission_progress(int current, int total)
{
    std::cout << "Mission status update: " << current << " / " << total << std::endl;

    if (current >= 2) {
        // We can only set a flag here. If we do more request inside the callback,
        // we risk blocking the system.
        _want_to_pause = true;
    }
}

