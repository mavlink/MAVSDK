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
using namespace std::placeholders; // for `_1`

static void receive_upload_mission_result(Mission::Result result);
static void receive_start_mission_result(Mission::Result result);
static void receive_mission_progress(Mission::MissionProgress progress);

Mission::MissionItem only_set_speed(float speed_m_s);
Mission::MissionItem
add_waypoint(double latitude_deg, double longitude_deg, float relative_altitude_m, float speed_m_s);

float current_speed(std::shared_ptr<Telemetry>& telemetry);

static std::atomic<bool> _mission_sent_ok{false};
static std::atomic<bool> _mission_started_ok{false};
static std::atomic<int> _current_item{0};

const static float speeds[4] = {10.0f, 3.0f, 8.0f, 5.0f};

// Test to check speed set for mission items.

TEST_F(SitlTest, MissionChangeSpeed)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    ASSERT_TRUE(poll_condition_with_timeout(
        [&dc]() { return dc.is_connected(); }, std::chrono::seconds(10)));

    System& system = dc.system();
    ASSERT_TRUE(system.has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto mission = std::make_shared<Mission>(system);
    auto action = std::make_shared<Action>(system);

    while (!telemetry->health_all_ok()) {
        LogInfo() << "Waiting for system to be ready";
        LogDebug() << "Health: " << telemetry->health();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LogInfo() << "System ready, let's start";

    Mission::MissionPlan mission_plan{};

    mission_plan.mission_items.push_back(only_set_speed(speeds[0]));
    mission_plan.mission_items.push_back(
        add_waypoint(47.398262509933957, 8.5456324815750122, 10, speeds[1]));
    mission_plan.mission_items.push_back(
        add_waypoint(47.39824201089737, 8.5447561722784542, 10, speeds[2]));
    mission_plan.mission_items.push_back(
        add_waypoint(47.397733642793433, 8.5447776308767516, 10, speeds[3]));

    mission->upload_mission_async(mission_plan, std::bind(&receive_upload_mission_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(_mission_sent_ok);

    Action::Result result = action->arm();
    ASSERT_EQ(result, Action::Result::Success);

    mission->mission_progress_async(std::bind(&receive_mission_progress, _1));

    mission->start_mission_async(std::bind(&receive_start_mission_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(_mission_started_ok);

    int last_item = -1;
    while (_current_item < static_cast<int>(mission_plan.mission_items.size())) {
        if (last_item != _current_item) {
            // Don't check the first because it's just a speed command and neither the second
            // because we're still taking off.
            if (_current_item >= 2) {
                // Time to accelerate
                std::this_thread::sleep_for(std::chrono::seconds(4));
                const float speed_correct = speeds[_current_item - 1];
                const float speed_actual = current_speed(telemetry);
                const float margin = 1.0f;
                if (speed_actual >= speed_correct + margin ||
                    speed_actual <= speed_correct - margin) {
                    LogWarn() << "Speed should be: " << speed_correct << " m/s, "
                              << "actually: " << speed_actual << " m/s";
                } else {
                    LogWarn() << "Speed should be: " << speed_correct << " m/s, "
                              << "actually: " << speed_actual << " m/s";
                }
                // TODO: enable these again with a better check not susceptible to time.
                // EXPECT_GT(speed_actual, speed_correct - margin);
                // EXPECT_LT(speed_actual, speed_correct + margin);
            }
            last_item = _current_item;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    LogInfo() << "mission done";

    result = action->return_to_launch();
    ASSERT_EQ(result, Action::Result::Success);

    while (!mission->is_mission_finished().second) {
        LogDebug() << "waiting until mission is done";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    while (telemetry->in_air()) {
        LogDebug() << "waiting until landed";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    result = action->disarm();
    ASSERT_EQ(result, Action::Result::Success);
}

void receive_upload_mission_result(Mission::Result result)
{
    EXPECT_EQ(result, Mission::Result::Success);

    if (result == Mission::Result::Success) {
        _mission_sent_ok = true;
    } else {
        LogErr() << "Error: mission send result: " << Mission::result_str(result);
    }
}

void receive_start_mission_result(Mission::Result result)
{
    EXPECT_EQ(result, Mission::Result::Success);

    if (result == Mission::Result::Success) {
        _mission_started_ok = true;
    } else {
        LogErr() << "Error: mission start result: " << Mission::result_str(result);
    }
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
    return new_item;
}

float current_speed(std::shared_ptr<Telemetry>& telemetry)
{
    return std::sqrt(
        telemetry->ground_speed_ned().velocity_north_m_s *
            telemetry->ground_speed_ned().velocity_north_m_s +
        telemetry->ground_speed_ned().velocity_east_m_s *
            telemetry->ground_speed_ned().velocity_east_m_s);
}

void receive_mission_progress(Mission::MissionProgress progress)
{
    LogInfo() << "Mission status update: " << progress.current << " / " << progress.total;
    _current_item = progress.current;
}
