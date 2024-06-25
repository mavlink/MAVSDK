#include <iostream>
#include <functional>
#include <memory>
#include <atomic>
#include <cmath>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/mission/mission.h"

using namespace mavsdk;

Mission::MissionItem only_set_speed(float speed_m_s);
Mission::MissionItem
add_waypoint(double latitude_deg, double longitude_deg, float relative_altitude_m, float speed_m_s);

float current_speed(std::shared_ptr<Telemetry> telemetry);

const static float speeds[4] = {10.0f, 3.0f, 8.0f, 5.0f};

// Test to check speed set for mission items.
TEST(SitlTest, PX4MissionChangeSpeed)
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

    mission_plan.mission_items.push_back(only_set_speed(speeds[0]));
    mission_plan.mission_items.push_back(
        add_waypoint(47.398262509933957, 8.5456324815750122, 10, speeds[1]));
    mission_plan.mission_items.push_back(
        add_waypoint(47.39824201089737, 8.5447561722784542, 10, speeds[2]));
    mission_plan.mission_items.push_back(
        add_waypoint(47.397733642793433, 8.5447776308767516, 10, speeds[3]));

    Mission::Result mission_result = mission->upload_mission(mission_plan);
    ASSERT_EQ(mission_result, Mission::Result::Success);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    Action::Result action_result = action->arm();
    ASSERT_EQ(action_result, Action::Result::Success);

    int last_item = -1;
    mission->subscribe_mission_progress(
        [&mission_plan, &last_item, telemetry](Mission::MissionProgress progress) {
            LogInfo() << "Mission progress: " << progress.current << " / " << progress.total;

            if (progress.current < static_cast<int>(mission_plan.mission_items.size()) &&
                last_item != progress.current) {
                // Don't check the first because it's just a speed command and neither the second
                // because we're still taking off.
                if (progress.current >= 2) {
                    std::thread([progress, telemetry]() {
                        // Time to accelerate
                        std::this_thread::sleep_for(std::chrono::seconds(4));
                        const float speed_correct = speeds[progress.current - 1];
                        const float speed_actual = current_speed(telemetry);
                        const float margin = 1.0f;
                        if (speed_actual >= speed_correct + margin ||
                            speed_actual <= speed_correct - margin) {
                            LogWarn() << "Speed should be: " << speed_correct << " m/s, "
                                      << "actually: " << speed_actual << " m/s";
                        }
                        // TODO: enable these again with a better check not susceptible to time.
                        // EXPECT_GT(speed_actual, speed_correct - margin);
                        // EXPECT_LT(speed_actual, speed_correct + margin);
                    }).detach();
                }
                last_item = progress.current;
            }
        });

    mission_result = mission->start_mission();
    ASSERT_EQ(mission_result, Mission::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

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

Mission::MissionItem only_set_speed(float speed_m_s)
{
    Mission::MissionItem new_item = {};
    new_item.speed_m_s = speed_m_s;
    return new_item;
}

Mission::MissionItem
add_waypoint(double latitude_deg, double longitude_deg, float relative_altitude_m, float speed_m_s)
{
    Mission::MissionItem new_item = {};
    new_item.latitude_deg = latitude_deg;
    new_item.longitude_deg = longitude_deg;
    new_item.relative_altitude_m = relative_altitude_m;
    new_item.speed_m_s = speed_m_s;
    new_item.acceptance_radius_m = 1.0f;
    return new_item;
}

float current_speed(std::shared_ptr<Telemetry> telemetry)
{
    return std::sqrt(
        telemetry->velocity_ned().north_m_s * telemetry->velocity_ned().north_m_s +
        telemetry->velocity_ned().east_m_s * telemetry->velocity_ned().east_m_s);
}
