#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/mission_raw/mission_raw.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include <future>

using namespace mavsdk;

TEST_F(SitlTest, PX4MissionRawImportAndFly)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->is_connected());
    ASSERT_TRUE(system->has_autopilot());

    auto action = Action{system};
    auto mission_raw = MissionRaw{system};
    auto telemetry = Telemetry{system};

    auto import_result =
        mission_raw.import_qgroundcontrol_mission("./src/integration_tests/triangle.plan");
    ASSERT_EQ(import_result.first, MissionRaw::Result::Success);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [&telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry.health_all_ok();
        },
        std::chrono::seconds(10)));

    ASSERT_EQ(action.arm(), Action::Result::Success);

    ASSERT_EQ(
        mission_raw.upload_mission(import_result.second.mission_items),
        MissionRaw::Result::Success);
    ASSERT_EQ(mission_raw.start_mission(), MissionRaw::Result::Success);

    auto prom = std::promise<void>();
    auto fut = prom.get_future();

    mission_raw.subscribe_mission_progress([&](MissionRaw::MissionProgress progress) {
        LogInfo() << "Progress: " << progress.current << "/" << progress.total;
        if (progress.current == progress.total) {
            mission_raw.subscribe_mission_progress(nullptr);
            prom.set_value();
        }
    });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(120)), std::future_status::ready);
    fut.get();

    ASSERT_EQ(action.return_to_launch(), Action::Result::Success);

    while (telemetry.armed()) {
        LogInfo() << "Waiting for drone to be landed and disarmed.";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
