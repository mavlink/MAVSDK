#include "log.h"
#include "mavsdk.h"
#include "example_plan.h"
#include "plugins/mission_raw/mission_raw.h"
#include "plugins/mission_raw_server/mission_raw_server.h"
#include <string>
#include <fstream>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;
TEST(Mission, RawUpload)
{
    Mavsdk mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};

    Mavsdk mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};

    // Register server plugin before connections so capabilities (MISSION_INT)
    // are available before the first AUTOPILOT_VERSION exchange.
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

    // We take an example mission plan, write it to a temp file and then import it.
    auto constexpr path = "/tmp/example.plan";
    std::ofstream out(path);
    out << plan;
    out.close();

    auto mission_raw = MissionRaw{system};
    auto result_pair = mission_raw.import_qgroundcontrol_mission(path);
    ASSERT_EQ(result_pair.first, MissionRaw::Result::Success);

    EXPECT_EQ(
        mission_raw.upload_mission(result_pair.second.mission_items), MissionRaw::Result::Success);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
