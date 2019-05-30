#include <iostream>
#include <functional>
#include <memory>
#include <future>
#include <chrono>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/mission/mission.h"

using namespace dronecode_sdk;

static std::shared_ptr<MissionItem> add_waypoint(double latitude_deg,
                                                 double longitude_deg,
                                                 float relative_altitude_m,
                                                 float speed_m_s,
                                                 bool is_fly_through,
                                                 float gimbal_pitch_deg,
                                                 float gimbal_yaw_deg,
                                                 bool take_photo);

TEST_F(SitlTest, MissionUploadCancellation)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    ASSERT_TRUE(poll_condition_with_timeout([&dc]() { return dc.is_connected(); },
                                            std::chrono::seconds(10)));

    System &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto mission = std::make_shared<Mission>(system);

    std::vector<std::shared_ptr<MissionItem>> mission_items;

    // We're going to try uploading 100 mission items.
    for (unsigned i = 0; i < 1000; ++i) {
        mission_items.push_back(
            add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, false));
    }

    std::promise<Mission::Result> prom{};
    std::future<Mission::Result> fut = prom.get_future();

    mission->upload_mission_async(mission_items, [&prom](Mission::Result result) {
        LogInfo() << "Upload mission result: " << Mission::result_str(result);
        prom.set_value(result);
    });

    // We should not be finished yet.
    auto future_status = fut.wait_for(std::chrono::milliseconds(100));
    EXPECT_EQ(future_status, std::future_status::timeout);

    mission->upload_mission_cancel();
    future_status = fut.wait_for(std::chrono::milliseconds(200));
    EXPECT_EQ(future_status, std::future_status::ready);
    auto future_result = fut.get();
    EXPECT_EQ(future_result, Mission::Result::CANCELLED);
}

TEST_F(SitlTest, MissionDownloadCancellation)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    ASSERT_TRUE(poll_condition_with_timeout([&dc]() { return dc.is_connected(); },
                                            std::chrono::seconds(10)));

    System &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto mission = std::make_shared<Mission>(system);

    std::vector<std::shared_ptr<MissionItem>> mission_items;

    // We're going to try uploading 100 mission items.
    for (unsigned i = 0; i < 1000; ++i) {
        mission_items.push_back(
            add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, false));
    }

    {
        std::promise<Mission::Result> prom{};
        std::future<Mission::Result> fut = prom.get_future();

        mission->upload_mission_async(mission_items, [&prom](Mission::Result result) {
            LogInfo() << "Upload mission result: " << Mission::result_str(result);
            prom.set_value(result);
        });

        auto future_result = fut.get();
        EXPECT_EQ(future_result, Mission::Result::SUCCESS);
    }

    mission_items.clear();

    {
        std::promise<Mission::Result> prom{};
        std::future<Mission::Result> fut = prom.get_future();

        mission->download_mission_async(
            [&prom](Mission::Result result,
                    std::vector<std::shared_ptr<MissionItem>> received_mission_items) {
                UNUSED(received_mission_items);
                LogInfo() << "Download mission result: " << Mission::result_str(result);
                prom.set_value(result);
            });

        // We should not be finished yet.
        auto future_status = fut.wait_for(std::chrono::milliseconds(100));
        EXPECT_EQ(future_status, std::future_status::timeout);

        mission->download_mission_cancel();
        future_status = fut.wait_for(std::chrono::milliseconds(200));
        EXPECT_EQ(future_status, std::future_status::ready);
        auto future_result = fut.get();
        EXPECT_EQ(future_result, Mission::Result::CANCELLED);
    }
}

std::shared_ptr<MissionItem> add_waypoint(double latitude_deg,
                                          double longitude_deg,
                                          float relative_altitude_m,
                                          float speed_m_s,
                                          bool is_fly_through,
                                          float gimbal_pitch_deg,
                                          float gimbal_yaw_deg,
                                          bool take_photo)
{
    std::shared_ptr<MissionItem> new_item(new MissionItem());
    new_item->set_position(latitude_deg, longitude_deg);
    new_item->set_relative_altitude(relative_altitude_m);
    new_item->set_speed(speed_m_s);
    new_item->set_fly_through(is_fly_through);
    new_item->set_gimbal_pitch_and_yaw(gimbal_pitch_deg, gimbal_yaw_deg);
    if (take_photo) {
        new_item->set_camera_action(MissionItem::CameraAction::TAKE_PHOTO);
    }
    return new_item;
}
