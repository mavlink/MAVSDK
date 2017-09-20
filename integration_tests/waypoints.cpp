#include <iostream>
#include <functional>
#include <memory>
#include <future>
#include "integration_test_helper.h"
#include "dronecore.h"

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

static void receive_mission_progress(int current, int total, Device *device);

static bool _break_done = false;


TEST_F(SitlTest, MissionAddWaypointsAndFly)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, DroneCore::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        LogInfo() << "waiting for device to be ready";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LogInfo() << "Device ready, let's start";

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
        // We only have the send_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        device.mission().send_mission_async(
        mission_items, [prom](Mission::Result result) {
            EXPECT_EQ(result, Mission::Result::SUCCESS);
            prom->set_value();
        });

        future_result.wait();
        future_result.get();
    }

    const Action::Result arm_result = device.action().arm();
    EXPECT_EQ(arm_result, Action::Result::SUCCESS);

    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    // We pass on device to receive_mission_progress because we need it in the callback.
    device.mission().subscribe_progress(std::bind(&receive_mission_progress, _1, _2, &device));

    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        device.mission().start_mission_async(
        [prom](Mission::Result result) {
            EXPECT_EQ(result, Mission::Result::SUCCESS);
            prom->set_value();
        });

        future_result.wait();
        future_result.get();
    }

    // Make sure we have taken off before checking for landed.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // FIXME: this check does currently not work in SITL.
    while (device.telemetry().in_air()) {
        // Wait until we're done.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        LogInfo() << "in air";

    }
    LogInfo() << "Landed, exiting.";
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

void receive_mission_progress(int current, int total, Device *device)
{
    LogInfo() << "Mission status update: " << current << " / " << total;

    if (current == 2) {

        // Some time after the mission item 2, we take a quick break.
        std::this_thread::sleep_for(std::chrono::seconds(2));

        {
            auto prom = std::make_shared<std::promise<void>>();
            auto future_result = prom->get_future();

            device->mission().pause_mission_async(
            [prom](Mission::Result result) {
                EXPECT_EQ(result, Mission::Result::SUCCESS);
                prom->set_value();
            });

            future_result.wait();
            future_result.get();
        }
        // We don't want to pause muptiple times in case we get the progress notification
        // several times.
        _break_done = true;

        // Pause for 5 seconds.
        std::this_thread::sleep_for(std::chrono::seconds(5));

        // Then continue.
        {
            auto prom = std::make_shared<std::promise<void>>();
            auto future_result = prom->get_future();
            device->mission().start_mission_async(
            [prom](Mission::Result result) {
                EXPECT_EQ(result, Mission::Result::SUCCESS);
                prom->set_value();
            });

            future_result.wait();
            future_result.get();
        }
    }


    if (current == total) {
        // We are done, and can do RTL to go home.
        const Action::Result result = device->action().arm();
        EXPECT_EQ(result, Action::Result::SUCCESS);
    }
}

