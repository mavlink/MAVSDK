#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/mission/mission.h"
#include "plugins/mission_raw/mission_raw.h"
#include <cmath>
#include <future>

using namespace dronecode_sdk;

static constexpr double SOME_LATITUDES[] = {47.398170, 47.398175};
static constexpr double SOME_LONGITUDES[] = {8.545649, 8.545654};
static constexpr float SOME_ALTITUDES[] = {5.0f, 7.5f};
static constexpr float SOME_SPEEDS[] = {4.0f, 5.0f};
static constexpr unsigned NUM_SOME_ITEMS = sizeof(SOME_LATITUDES) / sizeof(SOME_LATITUDES[0]);

static void
validate_items(const std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> &items);

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

    std::vector<std::shared_ptr<MissionItem>> mission_items;

    for (unsigned i = 0; i < NUM_SOME_ITEMS; ++i) {
        auto new_item = std::make_shared<MissionItem>();
        new_item->set_position(SOME_LATITUDES[i], SOME_LONGITUDES[i]);
        new_item->set_relative_altitude(SOME_ALTITUDES[i]);
        new_item->set_speed(SOME_SPEEDS[i]);
        mission_items.push_back(new_item);
    }

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
                validate_items(items);
                prom.set_value();
            });
        auto status = fut.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
    }
}

void validate_items(const std::vector<std::shared_ptr<MissionRaw::MavlinkMissionItemInt>> &items)
{
    for (unsigned i = 0; i < items.size(); ++i) {
        // Even items are waypoints, odd ones are the speed commands.
        if (i % 2 == 0) {
            EXPECT_EQ(items[i]->command, 16); // MAV_CMD_NAV_WAYPOINT
            EXPECT_EQ(items[i]->x, std::round(SOME_LATITUDES[i / 2] * 1e7));
            EXPECT_EQ(items[i]->y, std::round(SOME_LONGITUDES[i / 2] * 1e7));
            EXPECT_EQ(items[i]->z, SOME_ALTITUDES[i / 2]);
        } else {
            EXPECT_EQ(items[i]->command, 178); // MAV_CMD_DO_CHANGE_SPEED
            EXPECT_EQ(items[i]->param2, SOME_SPEEDS[i / 2]);
        }
    }
}
