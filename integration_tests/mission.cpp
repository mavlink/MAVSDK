#include <iostream>
#include <functional>
#include <memory>
#include <future>
#include <atomic>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/mission/mission.h"

using namespace dronecore;
using namespace std::placeholders; // for `_1`


static std::shared_ptr<MissionItem> add_mission_item(double latitude_deg,
                                                     double longitude_deg,
                                                     float relative_altitude_m,
                                                     float speed_m_s,
                                                     bool is_fly_through,
                                                     float gimbal_pitch_deg,
                                                     float gimbal_yaw_deg,
                                                     float loiter_time_s,
                                                     MissionItem::CameraAction camera_action);

static void compare_mission_items(const std::shared_ptr<MissionItem> original,
                                  const std::shared_ptr<MissionItem> downloaded);

// Set to 50 to test with about 1200 mission items.
static constexpr int test_with_many_items = 1;


TEST_F(SitlTest, MissionAddWaypointsAndFly)
{
    DroneCore dc;

    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        LogInfo() << "Waiting to discover system...";
        dc.register_on_discover([prom](uint64_t uuid) {
            LogInfo() << "Discovered system with UUID: " << uuid;
            prom->set_value();
        });

        ConnectionResult ret = dc.add_udp_connection();
        ASSERT_EQ(ret, ConnectionResult::SUCCESS);

        future_result.get();
    }


    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);
    auto mission = std::make_shared<Mission>(system);
    auto action = std::make_shared<Action>(system);

    while (!telemetry->health_all_ok()) {
        LogInfo() << "Waiting for system to be ready";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LogInfo() << "System ready";
    LogInfo() << "Creating and uploading mission";

    std::vector<std::shared_ptr<MissionItem>> mission_items;

    for (int i = 0; i < test_with_many_items; ++i) {
        mission_items.push_back(
            add_mission_item(47.398170327054473,
                             8.5456490218639658,
                             10.0f, 5.0f, false,
                             20.0f, 60.0f,
                             NAN,
                             MissionItem::CameraAction::NONE));

        mission_items.push_back(
            add_mission_item(47.398241338125118,
                             8.5455360114574432,
                             10.0f, 2.0f, true,
                             0.0f, -60.0f,
                             5.0f,
                             MissionItem::CameraAction::TAKE_PHOTO));

        mission_items.push_back(
            add_mission_item(47.398139363821485, 8.5453846156597137,
                             10.0f, 5.0f, true,
                             -46.0f, 0.0f,
                             NAN,
                             MissionItem::CameraAction::START_VIDEO));

        mission_items.push_back(
            add_mission_item(47.398058617228855,
                             8.5454618036746979,
                             10.0f, 2.0f, false,
                             -90.0f, 30.0f,
                             NAN,
                             MissionItem::CameraAction::STOP_VIDEO));

        mission_items.push_back(
            add_mission_item(47.398100366082858,
                             8.5456969141960144,
                             10.0f, 5.0f, false,
                             -45.0f, -30.0f,
                             NAN,
                             MissionItem::CameraAction::START_PHOTO_INTERVAL));

        mission_items.push_back(
            add_mission_item(47.398001890458097,
                             8.5455576181411743,
                             10.0f, 5.0f, false,
                             0.0f, 0.0f,
                             NAN,
                             MissionItem::CameraAction::STOP_PHOTO_INTERVAL));

    }

    {
        LogInfo() << "Uploading mission...";
        // We only have the upload_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission->upload_mission_async(
        mission_items, [prom](Mission::Result result) {
            ASSERT_EQ(result, Mission::Result::SUCCESS);
            prom->set_value();
            LogInfo() << "Mission uploaded.";
        });

        future_result.get();
    }

    {
        // Download the mission again and compare it.
        LogInfo() << "Downloading mission...";
        // We only have the download_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission->download_mission_async(
            [prom, mission_items](
                Mission::Result result,
        std::vector<std::shared_ptr<MissionItem>> mission_items_downloaded) {
            EXPECT_EQ(result, Mission::Result::SUCCESS);
            prom->set_value();
            LogInfo() << "Mission downloaded (to check it).";

            EXPECT_EQ(mission_items.size(), mission_items_downloaded.size());

            if (mission_items.size() == mission_items_downloaded.size()) {
                for (unsigned i = 0; i < mission_items.size(); ++i) {
                    compare_mission_items(mission_items.at(i), mission_items_downloaded.at(i));
                }
            }
        });

        future_result.get();

    }

    LogInfo() << "Arming...";
    const ActionResult arm_result = action->arm();
    EXPECT_EQ(arm_result, ActionResult::SUCCESS);
    LogInfo() << "Armed.";


    std::atomic<bool> want_to_pause {false};

    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    mission->subscribe_progress(
    [&want_to_pause](int current, int total) {
        LogInfo() << "Mission status update: " << current << " / " << total;
        if (current >= 2) {
            // We can only set a flag here. If we do more request inside the callback,
            // we risk blocking the system.
            want_to_pause = true;
        }
    });

    {
        LogInfo() << "Starting mission.";
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission->start_mission_async(
        [prom](Mission::Result result) {
            ASSERT_EQ(result, Mission::Result::SUCCESS);
            prom->set_value();
            LogInfo() << "Started mission.";
        });

        future_result.get();
    }


    while (!want_to_pause) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        LogInfo() << "Pausing mission...";
        mission->pause_mission_async(
        [prom](Mission::Result result) {
            EXPECT_EQ(result, Mission::Result::SUCCESS);
            prom->set_value();
        });

        future_result.get();
        LogInfo() << "Mission paused.";
    }

    // Pause for 5 seconds.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Then continue.
    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        LogInfo() << "Resuming mission...";
        mission->start_mission_async(
        [prom](Mission::Result result) {
            EXPECT_EQ(result, Mission::Result::SUCCESS);
            prom->set_value();
        });

        future_result.get();
        LogInfo() << "Mission resumed.";
    }

    while (!mission->mission_finished()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    {
        // We are done, and can do RTL to go home.
        LogInfo() << "Commanding RTL...";
        const ActionResult result = action->return_to_launch();
        EXPECT_EQ(result, ActionResult::SUCCESS);
        LogInfo() << "Commanded RTL.";
    }

    // We need to wait a bit, otherwise the armed state might not be correct yet.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    while (telemetry->armed()) {
        // Wait until we're done.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LogInfo() << "Disarmed, exiting.";
}

std::shared_ptr<MissionItem> add_mission_item(double latitude_deg,
                                              double longitude_deg,
                                              float relative_altitude_m,
                                              float speed_m_s,
                                              bool is_fly_through,
                                              float gimbal_pitch_deg,
                                              float gimbal_yaw_deg,
                                              float loiter_time_s,
                                              MissionItem::CameraAction camera_action)
{
    auto new_item = std::make_shared<MissionItem>();
    new_item->set_position(latitude_deg, longitude_deg);
    new_item->set_relative_altitude(relative_altitude_m);
    new_item->set_speed(speed_m_s);
    new_item->set_fly_through(is_fly_through);
    new_item->set_gimbal_pitch_and_yaw(gimbal_pitch_deg, gimbal_yaw_deg);
    new_item->set_loiter_time(loiter_time_s);
    new_item->set_camera_action(camera_action);

    // In order to test setting the interval, add it here.
    if (camera_action == MissionItem::CameraAction::START_PHOTO_INTERVAL) {
        new_item->set_camera_photo_interval(1.5);
    }

    return new_item;
}

void compare_mission_items(const std::shared_ptr<MissionItem> original,
                           const std::shared_ptr<MissionItem> downloaded)
{
    EXPECT_NEAR(original->get_latitude_deg(), downloaded->get_latitude_deg(), 1e-6);
    EXPECT_NEAR(original->get_longitude_deg(), downloaded->get_longitude_deg(), 1e-6);

    EXPECT_FLOAT_EQ(original->get_relative_altitude_m(),
                    downloaded->get_relative_altitude_m());

    EXPECT_EQ(original->get_fly_through(), downloaded->get_fly_through());
    if (std::isfinite(original->get_speed_m_s())) {
        EXPECT_FLOAT_EQ(original->get_speed_m_s(), downloaded->get_speed_m_s());
    }

    EXPECT_EQ(original->get_camera_action(), downloaded->get_camera_action());

    if (original->get_camera_action() == MissionItem::CameraAction::START_PHOTO_INTERVAL &&
        std::isfinite(original->get_camera_photo_interval_s())) {

        EXPECT_DOUBLE_EQ(original->get_camera_photo_interval_s(),
                         downloaded->get_camera_photo_interval_s());
    }

    if (std::isfinite(original->get_loiter_time_s())) {
        EXPECT_FLOAT_EQ(original->get_loiter_time_s(),
                        downloaded->get_loiter_time_s());
    }
}
