#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/mission_raw/mission_raw.h"
#include <cmath>
#include <future>

using namespace mavsdk;

static constexpr double SOME_LATITUDES[] = {47.398170, 47.398175};
static constexpr double SOME_LONGITUDES[] = {8.545649, 8.545654};
static constexpr float SOME_ALTITUDES[] = {5.0f, 7.5f};
static constexpr float SOME_SPEEDS[] = {4.0f, 5.0f};
static constexpr unsigned NUM_SOME_ITEMS = sizeof(SOME_LATITUDES) / sizeof(SOME_LATITUDES[0]);

static void validate_items(const std::vector<MissionRaw::MissionItem>& items);

TEST(SitlTest, PX4MissionRawMissionChanged)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->is_connected());
    ASSERT_TRUE(system->has_autopilot());

    auto mission_raw = std::make_shared<MissionRaw>(system);

    std::promise<void> prom_changed{};
    std::future<void> fut_changed = prom_changed.get_future();

    std::atomic<bool> called_once{false};

    LogInfo() << "Subscribe for mission changed notification";
    mission_raw->subscribe_mission_changed([&prom_changed, &called_once](bool) {
        bool flag = false;
        if (called_once.compare_exchange_strong(flag, true)) {
            prom_changed.set_value();
        }
    });

    // The mission change callback should not trigger yet.
    EXPECT_EQ(fut_changed.wait_for(std::chrono::milliseconds(500)), std::future_status::timeout);

    std::vector<MissionRaw::MissionItem> mission_raw_items;

    for (unsigned i = 0; i < NUM_SOME_ITEMS; ++i) {
        MissionRaw::MissionItem new_raw_item_nav{};
        new_raw_item_nav.seq = (i * 2);
        new_raw_item_nav.frame = 6; // MAV_FRAME_GLOBAL_RELATIVE_ALT_INT
        new_raw_item_nav.command = 16; // MAV_CMD_NAV_WAYPOINT
        new_raw_item_nav.current = 0;
        new_raw_item_nav.autocontinue = 1;
        new_raw_item_nav.param1 = 1.0; // Hold
        new_raw_item_nav.param2 = 1.0; // Accept Radius
        new_raw_item_nav.param3 = 1.0; // Pass Radius
        new_raw_item_nav.param4 = NAN; // Yaw
        new_raw_item_nav.x = int32_t(std::round(SOME_LATITUDES[i] * 1e7));
        new_raw_item_nav.y = int32_t(std::round(SOME_LONGITUDES[i] * 1e7));
        new_raw_item_nav.z = SOME_ALTITUDES[i];
        new_raw_item_nav.mission_type = 0; // MAV_MISSION_TYPE_MISSION

        MissionRaw::MissionItem new_raw_item_speed{};
        new_raw_item_speed.seq = (i * 2) + 1;
        new_raw_item_speed.frame = 2; // MAV_FRAME_MISSION
        new_raw_item_speed.command = 178; // MAV_CMD_DO_CHANGE_SPEED
        new_raw_item_speed.current = 0;
        new_raw_item_speed.autocontinue = 1;
        new_raw_item_speed.param1 =
            1.0f; // Speed type (0=Airspeed, 1=Ground Speed, 2=Climb Speed, 3=Descent Speed)
        new_raw_item_speed.param2 = SOME_SPEEDS[i]; // Speed
        new_raw_item_speed.param3 = -1.0f;
        new_raw_item_speed.param4 = 0.0f; // Relative	0: absolute, 1: relative
        new_raw_item_speed.x = 0;
        new_raw_item_speed.y = 0;
        new_raw_item_speed.z = NAN;
        new_raw_item_speed.mission_type = 0; // MAV_MISSION_TYPE_MISSION

        mission_raw_items.push_back(new_raw_item_nav);
        mission_raw_items.push_back(new_raw_item_speed);
    }

    mission_raw_items[0].current = 1;

    {
        LogInfo() << "Uploading mission...";
        // We only have the upload_mission function asynchronous for now, so we wrap it using
        // std::future.
        std::promise<void> prom{};
        std::future<void> fut = prom.get_future();
        mission_raw->upload_mission_async(mission_raw_items, [&prom](MissionRaw::Result result) {
            ASSERT_EQ(result, MissionRaw::Result::Success);
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
            [&prom](MissionRaw::Result result, const std::vector<MissionRaw::MissionItem> items) {
                EXPECT_EQ(result, MissionRaw::Result::Success);
                // TODO: validate items
                validate_items(items);
                prom.set_value();
            });
        auto status = fut.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
    }
}

void validate_items(const std::vector<MissionRaw::MissionItem>& items)
{
    for (unsigned i = 0; i < items.size(); ++i) {
        // Even items are waypoints, odd ones are the speed commands.
        if (i % 2 == 0) {
            EXPECT_EQ(items[i].command, 16); // MAV_CMD_NAV_WAYPOINT
            EXPECT_EQ(items[i].x, std::round(SOME_LATITUDES[i / 2] * 1e7));
            EXPECT_EQ(items[i].y, std::round(SOME_LONGITUDES[i / 2] * 1e7));
            EXPECT_EQ(items[i].z, SOME_ALTITUDES[i / 2]);
        } else {
            EXPECT_EQ(items[i].command, 178); // MAV_CMD_DO_CHANGE_SPEED
            EXPECT_EQ(items[i].param2, SOME_SPEEDS[i / 2]);
        }
    }
}
