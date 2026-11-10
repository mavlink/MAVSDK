#include "mavsdk.hpp"
#include "plugins/mission/mission.hpp"
#include "plugins/mission_raw/mission_raw.hpp"
#include "plugins/mission_raw_server/mission_raw_server.hpp"
#include <cmath>
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Mission, UploadReturnToLaunchParameters)
{
    for (const auto autopilot : {Autopilot::ArduPilot, Autopilot::Px4}) {
        SCOPED_TRACE(autopilot);
        Mavsdk groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
        Mavsdk::Configuration autopilot_config{ComponentType::Autopilot};
        autopilot_config.set_autopilot(autopilot);
        Mavsdk vehicle{autopilot_config};
        MissionRawServer mission_server{vehicle.server_component()};

        ASSERT_EQ(
            groundstation.add_any_connection("udpin://127.0.0.1:17000"), ConnectionResult::Success);
        ASSERT_EQ(
            vehicle.add_any_connection("udpout://127.0.0.1:17000"), ConnectionResult::Success);

        const auto system = groundstation.first_autopilot(10.0);
        ASSERT_TRUE(system);
        ASSERT_EQ((*system)->autopilot_type(), autopilot);

        Mission mission{*system};
        Mission::MissionItem waypoint{};
        waypoint.latitude_deg = 47.398170;
        waypoint.longitude_deg = 8.545649;
        waypoint.relative_altitude_m = 20.0f;
        ASSERT_EQ(mission.set_return_to_launch_after_mission(true), Mission::Result::Success);
        ASSERT_EQ(mission.upload_mission({{waypoint}}), Mission::Result::Success);

        const auto [result, items] = MissionRaw{*system}.download_mission();
        ASSERT_EQ(result, MissionRaw::Result::Success);
        ASSERT_EQ(items.size(), 2);
        const auto& rtl = items.back();
        EXPECT_EQ(rtl.command, MAV_CMD_NAV_RETURN_TO_LAUNCH);
        for (const float param : {rtl.param1, rtl.param2, rtl.param3, rtl.param4}) {
            if (autopilot == Autopilot::ArduPilot) {
                EXPECT_EQ(param, 0.0f);
            } else {
                EXPECT_TRUE(std::isnan(param));
            }
        }
    }
}
