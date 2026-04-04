#include "log.h"
#include "mavsdk.h"
#include "plugins/mission/mission.h"
#include "plugins/mission_raw_server/mission_raw_server.h"
#include <atomic>
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

static Mission::MissionPlan create_mission_plan()
{
    Mission::MissionPlan mission_plan{};
    // 10000 items so the transfer takes long enough that a mid-flight cancel is reliable
    // even on a fast loopback link where the user callback queue lags behind.
    for (unsigned i = 0; i < 10000; ++i) {
        mission_plan.mission_items.push_back(
            add_waypoint(47.3981703270545, 8.54564902186397, 20.0, 3.0, true, -90.0, 0.0, false));
    }
    return mission_plan;
}

TEST(Mission, UploadCancellation)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto mission = Mission{system};
    auto mission_plan = create_mission_plan();

    std::promise<Mission::Result> result_prom{};
    std::future<Mission::Result> result_fut = result_prom.get_future();
    std::atomic<bool> cancel_triggered{false};

    // Use the progress variant so we can cancel the moment the first progress
    // update arrives.  That guarantees cancel() is called while the transfer is
    // still in flight, regardless of how fast the loopback link is.
    LogInfo("Starting mission upload...");
    mission.upload_mission_with_progress_async(
        mission_plan, [&](Mission::Result result, Mission::ProgressData /*progress_data*/) {
            if (result == Mission::Result::Next) {
                // Cancel exactly once, on the first progress update.
                if (!cancel_triggered.exchange(true)) {
                    LogInfo("Upload is in flight, cancelling...");
                    mission.cancel_mission_upload();
                }
                return;
            }
            // Final callback: TransferCancelled expected.
            result_prom.set_value(result);
        });

    // Allow generous time: the cancel result is delivered via the async
    // user-callback queue which can be slow on a loaded CI runner.
    ASSERT_EQ(result_fut.wait_for(std::chrono::seconds(30)), std::future_status::ready);
    EXPECT_EQ(result_fut.get(), Mission::Result::TransferCancelled);
    LogInfo("Mission upload cancelled successfully.");

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(Mission, DownloadCancellation)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    auto mission_raw_server = MissionRawServer{mavsdk_autopilot.server_component()};

    ASSERT_EQ(
        mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000"),
        ConnectionResult::Success);
    ASSERT_EQ(
        mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

    auto maybe_system = mavsdk_groundstation.first_autopilot(10.0);
    ASSERT_TRUE(maybe_system);
    auto system = maybe_system.value();
    ASSERT_TRUE(system->has_autopilot());

    auto mission = Mission{system};
    auto mission_plan = create_mission_plan();

    // Upload synchronously first (separate from the cancellation test).
    LogInfo("Uploading mission first...");
    ASSERT_EQ(mission.upload_mission(mission_plan), Mission::Result::Success);
    LogInfo("Mission uploaded successfully.");

    // Download and cancel once we know the transfer is in flight.
    std::promise<Mission::Result> result_prom{};
    std::future<Mission::Result> result_fut = result_prom.get_future();
    std::atomic<bool> cancel_triggered{false};

    LogInfo("Starting mission download...");
    mission.download_mission_with_progress_async(
        [&](Mission::Result result, Mission::ProgressDataOrMission progress_data) {
            if (result == Mission::Result::Next) {
                // has_progress == true during the item exchange;
                // has_mission == true only at successful completion — don't cancel then.
                if (progress_data.has_progress && !cancel_triggered.exchange(true)) {
                    LogInfo("Download is in flight, cancelling...");
                    mission.cancel_mission_download();
                }
                return;
            }
            // Final callback: TransferCancelled expected.
            result_prom.set_value(result);
        });

    ASSERT_EQ(result_fut.wait_for(std::chrono::seconds(30)), std::future_status::ready);
    EXPECT_EQ(result_fut.get(), Mission::Result::TransferCancelled);
    LogInfo("Mission download cancelled successfully.");

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
