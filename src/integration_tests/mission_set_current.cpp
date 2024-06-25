#include <iostream>
#include <functional>
#include <future>
#include <memory>
#include <atomic>
#include <cmath>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/mission/mission.h"

using namespace mavsdk;

Mission::MissionItem
add_waypoint(double latitude_deg, double longitude_deg, float relative_altitude_m);

// Test to check speed set for mission items.
TEST(SitlTest, PX4MissionSetCurrent)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    ASSERT_TRUE(poll_condition_with_timeout(
        [&mavsdk]() {
            const auto systems = mavsdk.systems();

            if (systems.size() == 0) {
                return false;
            }

            const auto system = mavsdk.systems().at(0);
            return system->is_connected();
        },
        std::chrono::seconds(10)));

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto mission = std::make_shared<Mission>(system);
    auto action = std::make_shared<Action>(system);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    LogInfo() << "System ready, let's start";

    Mission::MissionPlan mission_plan{};

    mission_plan.mission_items.push_back(add_waypoint(47.398262509933957, 8.5456324815750122, 10));
    mission_plan.mission_items.push_back(add_waypoint(47.39824201089737, 8.5447561722784542, 10));
    mission_plan.mission_items.push_back(add_waypoint(47.397733642793433, 8.5447776308767516, 10));

    Mission::Result mission_result = mission->upload_mission(mission_plan);
    ASSERT_EQ(mission_result, Mission::Result::Success);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    Action::Result action_result = action->arm();
    ASSERT_EQ(action_result, Action::Result::Success);

    mission_result = mission->start_mission();
    ASSERT_EQ(mission_result, Mission::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Wait until we reach 2 meters.
    {
        auto prom = std::promise<void>{};
        auto fut = prom.get_future();
        const Telemetry::PositionHandle handle = telemetry->subscribe_position(
            [&prom, &telemetry, &handle](Telemetry::Position position) {
                if (position.relative_altitude_m > 2.0f) {
                    LogInfo() << "Reached 2 meters, jumping ahead to third mission item";
                    telemetry->unsubscribe_position(handle);
                    prom.set_value();
                }
            });
        auto fut_status = fut.wait_for(std::chrono::seconds(10));
        ASSERT_EQ(fut_status, std::future_status::ready);
    }

    const int last_item = mission_plan.mission_items.size() - 1;

    // Then jump to last item.
    {
        auto prom = std::make_shared<std::promise<Mission::Result>>();
        auto fut = prom->get_future();
        mission->set_current_mission_item_async(
            last_item, [prom](Mission::Result result) { prom->set_value(result); });

        auto fut_status = fut.wait_for(std::chrono::seconds(5));
        ASSERT_EQ(fut_status, std::future_status::ready);
        auto fut_result = fut.get();

        EXPECT_EQ(fut_result, Mission::Result::Success);
    }

    // Make sure current is now the last one.
    {
        mission->subscribe_mission_progress([last_item](Mission::MissionProgress progress) {
            EXPECT_GE(progress.current, last_item);
        });
    }

    LogDebug() << "waiting until mission is done";
    while (!mission->is_mission_finished().second) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LogInfo() << "mission done";

    action_result = action->return_to_launch();
    ASSERT_EQ(action_result, Action::Result::Success);

    LogDebug() << "waiting until landed";
    while (telemetry->in_air()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    action_result = action->disarm();
    ASSERT_EQ(action_result, Action::Result::Success);
}

Mission::MissionItem
add_waypoint(double latitude_deg, double longitude_deg, float relative_altitude_m)
{
    Mission::MissionItem new_item = {};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    return new_item;
}
