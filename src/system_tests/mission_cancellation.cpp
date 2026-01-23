#include "log.h"
#include "mavsdk.h"
#include "plugins/mission/mission.h"
#include "plugins/mission_raw_server/mission_raw_server.h"
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

static Mission::MissionItem add_waypoint(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    bool take_photo);

TEST(SystemTest, MissionUploadCancellation)
{
    // Create two MAVSDK instances: groundstation and autopilot
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    // Set up the autopilot side with MissionRawServer
    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    // Wait for groundstation to discover autopilot
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto mission = Mission{system};

    Mission::MissionPlan mission_plan{};

    // Create a large mission (1000 items) to ensure we have time to cancel
    for (unsigned i = 0; i < 1000; ++i) {
        mission_plan.mission_items.push_back(
            add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, false));
    }

    std::promise<Mission::Result> prom{};
    std::future<Mission::Result> fut = prom.get_future();

    LogInfo() << "Starting mission upload...";
    mission.upload_mission_async(mission_plan, [&prom](Mission::Result result) {
        LogInfo() << "Upload mission result: " << result;
        prom.set_value(result);
    });

    // We should not be finished yet (only wait 100ms)
    auto future_status = fut.wait_for(std::chrono::milliseconds(100));
    EXPECT_EQ(future_status, std::future_status::timeout);

    LogInfo() << "Cancelling mission upload...";
    mission.cancel_mission_upload();

    // Wait for cancellation to complete
    future_status = fut.wait_for(std::chrono::milliseconds(500));
    EXPECT_EQ(future_status, std::future_status::ready);
    auto future_result = fut.get();
    EXPECT_EQ(future_result, Mission::Result::TransferCancelled);
    LogInfo() << "Mission upload cancelled successfully.";

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(SystemTest, MissionDownloadCancellation)
{
    // Create two MAVSDK instances: groundstation and autopilot
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    // Set up the autopilot side with MissionRawServer
    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    // Wait for groundstation to discover autopilot
    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto mission = Mission{system};

    Mission::MissionPlan mission_plan{};

    // Create a large mission (1000 items) to ensure we have time to cancel during download
    for (unsigned i = 0; i < 1000; ++i) {
        mission_plan.mission_items.push_back(
            add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, false));
    }

    // First upload the mission
    {
        std::promise<Mission::Result> prom{};
        std::future<Mission::Result> fut = prom.get_future();

        LogInfo() << "Uploading mission first...";
        mission.upload_mission_async(mission_plan, [&prom](Mission::Result result) {
            LogInfo() << "Upload mission result: " << result;
            prom.set_value(result);
        });

        auto future_result = fut.get();
        EXPECT_EQ(future_result, Mission::Result::Success);
        LogInfo() << "Mission uploaded successfully.";
    }

    // Now try to download and cancel
    {
        std::promise<Mission::Result> prom{};
        std::future<Mission::Result> fut = prom.get_future();

        LogInfo() << "Starting mission download...";
        mission.download_mission_async([&prom](Mission::Result result, Mission::MissionPlan) {
            LogInfo() << "Download mission result: " << result;
            prom.set_value(result);
        });

        // We should not be finished yet (only wait 100ms)
        auto future_status = fut.wait_for(std::chrono::milliseconds(100));
        EXPECT_EQ(future_status, std::future_status::timeout);

        LogInfo() << "Cancelling mission download...";
        mission.cancel_mission_download();

        // Wait for cancellation to complete
        future_status = fut.wait_for(std::chrono::milliseconds(500));
        EXPECT_EQ(future_status, std::future_status::ready);
        auto future_result = fut.get();
        EXPECT_EQ(future_result, Mission::Result::TransferCancelled);
        LogInfo() << "Mission download cancelled successfully.";
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

Mission::MissionItem add_waypoint(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    float speed_m_s,
    bool is_fly_through,
    float gimbal_pitch_deg,
    float gimbal_yaw_deg,
    bool take_photo)
{
    Mission::MissionItem new_item{};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    new_item.speed_m_s = speed_m_s;
    new_item.is_fly_through = is_fly_through;

    new_item.gimbal_pitch_deg = gimbal_pitch_deg;
    new_item.gimbal_yaw_deg = gimbal_yaw_deg;
    if (take_photo) {
        new_item.camera_action = Mission::MissionItem::CameraAction::TakePhoto;
    }
    return new_item;
}
