#include <iostream>
#include <functional>
#include <memory>
#include <future>
#include <chrono>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/mission/mission.h"

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

TEST_F(SitlTest, MissionUploadCancellation)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    ASSERT_TRUE(poll_condition_with_timeout(
        [&dc]() { return dc.is_connected(); }, std::chrono::seconds(10)));

    System& system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto mission = std::make_shared<Mission>(system);

    Mission::MissionPlan mission_plan{};

    // We're going to try uploading 100 mission items.
    for (unsigned i = 0; i < 1000; ++i) {
        mission_plan.mission_items.push_back(
            add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, false));
    }

    std::promise<Mission::Result> prom{};
    std::future<Mission::Result> fut = prom.get_future();

    mission->upload_mission_async(mission_plan, [&prom](Mission::Result result) {
        LogInfo() << "Upload mission result: " << Mission::result_str(result);
        prom.set_value(result);
    });

    // We should not be finished yet.
    auto future_status = fut.wait_for(std::chrono::milliseconds(100));
    EXPECT_EQ(future_status, std::future_status::timeout);

    mission->cancel_mission_upload();
    future_status = fut.wait_for(std::chrono::milliseconds(200));
    EXPECT_EQ(future_status, std::future_status::ready);
    auto future_result = fut.get();
    EXPECT_EQ(future_result, Mission::Result::TransferCancelled);

    // FIXME: older PX4 versions don't support CANCEL and need time to timeout.
    std::this_thread::sleep_for(std::chrono::seconds(5));
}

TEST_F(SitlTest, MissionDownloadCancellation)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    ASSERT_TRUE(poll_condition_with_timeout(
        [&dc]() { return dc.is_connected(); }, std::chrono::seconds(10)));

    System& system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto mission = std::make_shared<Mission>(system);

    Mission::MissionPlan mission_plan{};

    // We're going to try uploading 100 mission items.
    for (unsigned i = 0; i < 1000; ++i) {
        mission_plan.mission_items.push_back(
            add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, false));
    }

    {
        std::promise<Mission::Result> prom{};
        std::future<Mission::Result> fut = prom.get_future();

        mission->upload_mission_async(mission_plan, [&prom](Mission::Result result) {
            LogInfo() << "Upload mission result: " << Mission::result_str(result);
            prom.set_value(result);
        });

        auto future_result = fut.get();
        EXPECT_EQ(future_result, Mission::Result::Success);
    }

    {
        std::promise<Mission::Result> prom{};
        std::future<Mission::Result> fut = prom.get_future();

        mission->download_mission_async(
            [&prom](Mission::Result result, Mission::MissionPlan received_mission_plan) {
                UNUSED(received_mission_plan);
                LogInfo() << "Download mission result: " << Mission::result_str(result);
                prom.set_value(result);
            });

        // We should not be finished yet.
        auto future_status = fut.wait_for(std::chrono::milliseconds(100));
        EXPECT_EQ(future_status, std::future_status::timeout);

        mission->cancel_mission_download();
        future_status = fut.wait_for(std::chrono::milliseconds(200));
        EXPECT_EQ(future_status, std::future_status::ready);
        auto future_result = fut.get();
        EXPECT_EQ(future_result, Mission::Result::TransferCancelled);
    }

    // FIXME: older PX4 versions don't support CANCEL and need time to timeout.
    std::this_thread::sleep_for(std::chrono::seconds(5));
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
