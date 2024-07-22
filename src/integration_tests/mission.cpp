#include <iostream>
#include <functional>
#include <memory>
#include <future>
#include <atomic>
#include <cmath>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/mission/mission.h"

using namespace mavsdk;

static void test_mission(
    std::shared_ptr<Telemetry> telemetry,
    std::shared_ptr<Mission> mission,
    std::shared_ptr<Action> action);

static Mission::MissionItem add_mission_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    float loiter_time_s,
    Mission::MissionItem::CameraAction camera_action,
    float acceptance_radius_m,
    float yaw_deg);

static void pause_and_resume(std::shared_ptr<Mission> mission);

static constexpr int NUM_MISSION_ITEMS = 6;

static std::atomic<bool> pause_already_done{false};

TEST(SitlTest, PX4MissionAddWaypointsAndFly)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        LogInfo() << "Waiting to discover system...";
        Mavsdk::NewSystemHandle handle = mavsdk.subscribe_on_new_system([&mavsdk, prom]() {
            const auto system = mavsdk.systems().at(0);

            if (system->is_connected()) {
                LogInfo() << "Discovered system";
                prom->set_value();
            }
        });

        ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
        ASSERT_EQ(ret, ConnectionResult::Success);

        auto status = future_result.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        future_result.get();
        // FIXME: This hack is to prevent that the promise is set twice.
        mavsdk.unsubscribe_on_new_system(handle);
    }

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto mission = std::make_shared<Mission>(system);
    auto action = std::make_shared<Action>(system);

    test_mission(telemetry, mission, action);
    // We do yet another mission to make sure this works repeatable.
    test_mission(telemetry, mission, action);
}

void test_mission(
    std::shared_ptr<Telemetry> telemetry,
    std::shared_ptr<Mission> mission,
    std::shared_ptr<Action> action)
{
    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    LogInfo() << "System ready";
    LogInfo() << "Creating and uploading mission";

    Mission::MissionPlan mission_plan{};

    while (mission_plan.mission_items.size() < NUM_MISSION_ITEMS) {
        mission_plan.mission_items.push_back(add_mission_item(
            47.398170327054473,
            8.5456490218639658,
            10.0f,
            5.0f,
            false,
            20.0f,
            60.0f,
            NAN,
            Mission::MissionItem::CameraAction::None,
            0.5f,
            NAN));

        mission_plan.mission_items.push_back(add_mission_item(
            47.398241338125118,
            8.5455360114574432,
            10.0f,
            2.0f,
            true,
            0.0f,
            -60.0f,
            5.0f,
            Mission::MissionItem::CameraAction::TakePhoto,
            4.0f,
            NAN));

        mission_plan.mission_items.push_back(add_mission_item(
            47.398139363821485,
            8.5453846156597137,
            10.0f,
            5.0f,
            true,
            -46.0f,
            0.0f,
            NAN,
            Mission::MissionItem::CameraAction::StartVideo,
            4.0f,
            0.0f));

        mission_plan.mission_items.push_back(add_mission_item(
            47.398058617228855,
            8.5454618036746979,
            10.0f,
            2.0f,
            false,
            -90.0f,
            30.0f,
            NAN,
            Mission::MissionItem::CameraAction::StopVideo,
            0.5f,
            90.0f));

        mission_plan.mission_items.push_back(add_mission_item(
            47.398100366082858,
            8.5456969141960144,
            10.0f,
            5.0f,
            false,
            -45.0f,
            -30.0f,
            NAN,
            Mission::MissionItem::CameraAction::StartPhotoInterval,
            0.5f,
            0.0f));

        mission_plan.mission_items.push_back(add_mission_item(
            47.398001890458097,
            8.5455576181411743,
            10.0f,
            5.0f,
            false,
            0.0f,
            0.0f,
            NAN,
            Mission::MissionItem::CameraAction::StopPhotoInterval,
            0.5f,
            NAN));

        mission_plan.mission_items.push_back(add_mission_item(
            47.393,
            8.544,
            10.0f,
            5.0f,
            false,
            -45.0f,
            -30.0f,
            NAN,
            Mission::MissionItem::CameraAction::StartPhotoDistance,
            0.5f,
            0.0f));

        mission_plan.mission_items.push_back(add_mission_item(
            47.395,
            8.542,
            10.0f,
            5.0f,
            false,
            0.0f,
            0.0f,
            NAN,
            Mission::MissionItem::CameraAction::StopPhotoDistance,
            0.5f,
            NAN));
    }

    mission->set_return_to_launch_after_mission(true);
    EXPECT_TRUE(mission->get_return_to_launch_after_mission().second);

    {
        LogInfo() << "Uploading mission...";
        // We only have the upload_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission->upload_mission_async(mission_plan, [prom](Mission::Result result) {
            ASSERT_EQ(result, Mission::Result::Success);
            prom->set_value();
            LogInfo() << "Mission uploaded.";
        });

        auto status = future_result.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        future_result.get();
    }

    {
        // Download the mission again and compare it.
        LogInfo() << "Downloading mission...";
        // We only have the download_mission function asynchronous for now, so we wrap it using
        // std::future.
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission->download_mission_async([prom, mission_plan](
                                            Mission::Result result,
                                            Mission::MissionPlan mission_plan_downloaded) {
            EXPECT_EQ(result, Mission::Result::Success);
            prom->set_value();
            LogInfo() << "Mission downloaded (to check it).";

            EXPECT_EQ(
                mission_plan.mission_items.size(), mission_plan_downloaded.mission_items.size());

            if (mission_plan.mission_items.size() == mission_plan_downloaded.mission_items.size()) {
                for (unsigned i = 0; i < mission_plan.mission_items.size(); ++i) {
                    const auto original = mission_plan.mission_items.at(i);
                    const auto downloaded = mission_plan_downloaded.mission_items.at(i);
                    EXPECT_EQ(original, downloaded);
                }
            }
        });

        auto status = future_result.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        future_result.get();
    }

    EXPECT_TRUE(mission->get_return_to_launch_after_mission().second);

    LogInfo() << "Arming...";
    const Action::Result arm_result = action->arm();
    EXPECT_EQ(arm_result, Action::Result::Success);
    LogInfo() << "Armed.";

    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    mission->subscribe_mission_progress([&mission](Mission::MissionProgress progress) {
        LogInfo() << "Mission status update: " << progress.current << " / " << progress.total;
        if (progress.current >= 2 && !pause_already_done) {
            pause_already_done = true;
            pause_and_resume(mission);
        }
    });

    {
        LogInfo() << "Starting mission.";
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission->start_mission_async([prom](Mission::Result result) {
            ASSERT_EQ(result, Mission::Result::Success);
            prom->set_value();
            LogInfo() << "Started mission.";
        });

        auto status = future_result.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        future_result.get();
    }

    // At the end of the mission it should RTL automatically, we can
    // just wait for auto disarm.

    while (!mission->is_mission_finished().second) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    while (telemetry->armed()) {
        // Wait until we're done.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LogInfo() << "Disarmed.";

    {
        LogInfo() << "Clearing mission.";
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission->clear_mission_async([prom](Mission::Result result) {
            ASSERT_EQ(result, Mission::Result::Success);
            prom->set_value();
            LogInfo() << "Cleared mission, exiting.";
        });

        auto status = future_result.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        future_result.get();
    }
}

Mission::MissionItem add_mission_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    float loiter_time_s,
    Mission::MissionItem::CameraAction camera_action,
    float acceptance_radius_m,
    float yaw_deg)
{
    Mission::MissionItem new_item{};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    new_item.speed_m_s = speed_m_s;
    new_item.is_fly_through = is_fly_through;
    new_item.gimbal_pitch_deg = gimbal_pitch_deg;
    new_item.gimbal_yaw_deg = gimbal_yaw_deg;
    new_item.loiter_time_s = loiter_time_s;
    new_item.camera_action = camera_action;
    new_item.acceptance_radius_m = acceptance_radius_m;
    new_item.yaw_deg = yaw_deg;

    // In order to test setting the interval, add it here.
    if (camera_action == Mission::MissionItem::CameraAction::StartPhotoInterval) {
        new_item.camera_photo_interval_s = 1.5;
    }
    // In order to test setting the interval, add it here.
    else if (camera_action == Mission::MissionItem::CameraAction::StartPhotoDistance) {
        new_item.camera_photo_distance_m = 5.5;
    }

    return new_item;
}

void pause_and_resume(std::shared_ptr<Mission> mission)
{
    std::thread([mission]() {
        {
            auto prom = std::make_shared<std::promise<void>>();
            auto future_result = prom->get_future();
            mission->pause_mission_async([prom](Mission::Result result) {
                EXPECT_EQ(result, Mission::Result::Success);
                prom->set_value();
                LogInfo() << "Mission paused.";
            });

            auto status = future_result.wait_for(std::chrono::seconds(2));
            ASSERT_EQ(status, std::future_status::ready);
            future_result.get();
        }

        // Pause for 5 seconds.
        std::this_thread::sleep_for(std::chrono::seconds(5));

        {
            auto prom = std::make_shared<std::promise<void>>();
            // Then continue.
            auto future_result = prom->get_future();
            LogInfo() << "Resuming mission...";
            mission->start_mission_async([prom](Mission::Result result) {
                EXPECT_EQ(result, Mission::Result::Success);
                prom->set_value();
            });

            auto status = future_result.wait_for(std::chrono::seconds(2));
            ASSERT_EQ(status, std::future_status::ready);
            future_result.get();
            LogInfo() << "Mission resumed.";
        }
    }).detach();
}
