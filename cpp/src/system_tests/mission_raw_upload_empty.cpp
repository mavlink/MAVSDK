#include "log.h"
#include "mavsdk.h"
#include "plugins/mission_raw/mission_raw.h"
#include "plugins/mission_raw_server/mission_raw_server.h"
#include <future>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Mission, RawUploadEmpty)
{
    // Regression test for #1962: uploading an empty mission must not crash.

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

    auto mission_raw = MissionRaw{system};

    // Upload empty mission — this used to crash with a core dump.
    std::vector<MissionRaw::MissionItem> empty_items;
    auto result = mission_raw.upload_mission(empty_items);
    LogInfo("Empty mission upload result: {}", to_string(result));

    // The result may vary depending on how the server handles empty missions,
    // but the key assertion is that we don't crash (the original bug).
    EXPECT_TRUE(
        result == MissionRaw::Result::Success || result == MissionRaw::Result::InvalidArgument ||
        result == MissionRaw::Result::NoMissionAvailable);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Now upload a normal mission to verify the server still works.
    MissionRaw::MissionItem waypoint{};
    waypoint.seq = 0;
    waypoint.frame = 6;
    waypoint.command = 16; // MAV_CMD_NAV_WAYPOINT
    waypoint.current = 1;
    waypoint.autocontinue = 1;
    waypoint.param1 = 0;
    waypoint.param2 = 0;
    waypoint.param3 = 0;
    waypoint.param4 = NAN;
    waypoint.x = static_cast<int32_t>(47.398170 * 1e7);
    waypoint.y = static_cast<int32_t>(8.545649 * 1e7);
    waypoint.z = 10.0f;
    waypoint.mission_type = 0;

    std::vector<MissionRaw::MissionItem> items{waypoint};
    EXPECT_EQ(mission_raw.upload_mission(items), MissionRaw::Result::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
