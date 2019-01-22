#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/mission/mission.h"
#include "plugins/mission_raw/mission_raw.h"
#include <future>

using namespace dronecode_sdk;

TEST_F(SitlTest, MissionRawMissionChanged)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_TRUE(dc.is_connected());

    System &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto mission = std::make_shared<Mission>(system);
    auto mission_raw = std::make_shared<MissionRaw>(system);

    std::promise<void> prom_changed{};
    std::future<void> fut_changed = prom_changed.get_future();

    LogInfo() << "Subscribe for mission changed notification";
    mission_raw->subscribe_mission_changed([&prom_changed]() { prom_changed.set_value(); });

    // The mission change callback should not trigger yet.
    EXPECT_EQ(fut_changed.wait_for(std::chrono::milliseconds(500)), std::future_status::timeout);

    auto new_item = std::make_shared<MissionItem>();
    new_item->set_position(47.398170327054473, 8.5456490218639658);
    new_item->set_relative_altitude(10.0f);
    new_item->set_speed(5.0f);
    std::vector<std::shared_ptr<MissionItem>> mission_items;
    mission_items.push_back(new_item);

    {
        LogInfo() << "Uploading mission...";
        // We only have the upload_mission function asynchronous for now, so we wrap it using
        // std::future.
        std::promise<void> prom{};
        std::future<void> fut = prom.get_future();
        mission->upload_mission_async(mission_items, [&prom](Mission::Result result) {
            ASSERT_EQ(result, Mission::Result::SUCCESS);
            prom.set_value();
            LogInfo() << "Mission uploaded.";
        });

        auto status = fut.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        fut.get();
    }

    // The mission change callback should have triggered now because we have uploaded a mission.
    EXPECT_EQ(fut_changed.wait_for(std::chrono::milliseconds(500)), std::future_status::ready);

    {
        std::promise<void> prom{};
        std::future<void> fut = prom.get_future();
        LogInfo() << "Download raw mission items.";
        mission_raw->download_mission_async(
            [&prom](MissionRaw::Result result,
                    std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> items) {
                EXPECT_EQ(result, MissionRaw::Result::SUCCESS);
                // TODO: validate items
                UNUSED(items);
                prom.set_value();
            });
        auto status = fut.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
    }
}
