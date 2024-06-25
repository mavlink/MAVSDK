#include <iostream>
#include <functional>
#include <memory>
#include <future>
#include <atomic>
#include <cmath>
#include "integration_test_helper.h"
#include "path_checker.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/mission/mission.h"

using namespace mavsdk;

TEST(SitlTest, MissionTakeoffTransitionAndLand_standard_vtol)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    float mission_altitude_m = 40;

    {
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();

        LogInfo() << "Waiting to discover system...";
        mavsdk.subscribe_on_new_system([&mavsdk, prom]() {
            const auto system = mavsdk.systems().at(0);

            if (system->is_connected()) {
                LogInfo() << "Discovered system";
                prom->set_value();
            }
        });

        ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
        ASSERT_EQ(ret, ConnectionResult::Success);

        auto status = future_result.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        future_result.get();
        // FIXME: This hack is to prevent that the promise is set twice.
        mavsdk.subscribe_on_new_system(nullptr);
    }

    auto system = mavsdk.systems().at(0);
    auto telemetry = std::make_shared<Telemetry>(system);
    auto mission = std::make_shared<Mission>(system);
    auto action = std::make_shared<Action>(system);

    PathChecker pc;

    telemetry->subscribe_position([&pc](Telemetry::Position position) {
        pc.check_current_alitude(position.relative_altitude_m);
    });

    std::vector<Telemetry::VtolState> vtol_states_template = {
        Telemetry::VtolState::Mc,
        Telemetry::VtolState::TransitionToFw,
        Telemetry::VtolState::Fw,
        Telemetry::VtolState::TransitionToMc,
        Telemetry::VtolState::Mc};

    std::vector<Telemetry::VtolState> vtol_states;
    Telemetry::VtolState vtol_state_previous = Telemetry::VtolState::Undefined;

    telemetry->subscribe_vtol_state(
        [&vtol_states, &vtol_state_previous](Telemetry::VtolState vtol_state) {
            if (vtol_state_previous != vtol_state) {
                LogInfo() << vtol_state;
                vtol_state_previous = vtol_state;
                vtol_states.push_back(vtol_state);
            }
        });

    while (!telemetry->health_all_ok()) {
        LogInfo() << "Waiting for system to be ready";
        LogDebug() << "Health: " << telemetry->health();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LogInfo() << "System ready";
    LogInfo() << "Creating and uploading mission";

    Mission::MissionItem new_item{};

    new_item.latitude_deg = 47.398170;
    new_item.longitude_deg = 8.545649;
    new_item.acceptance_radius_m = 1;
    new_item.relative_altitude_m = mission_altitude_m;
    new_item.vehicle_action = Mission::MissionItem::VehicleAction::None;

    Mission::MissionPlan mission_plan{};
    mission_plan.mission_items.push_back(new_item);

    new_item.vehicle_action = Mission::MissionItem::VehicleAction::TransitionToFw;
    mission_plan.mission_items.push_back(new_item);

    new_item.latitude_deg = 47.398170;
    new_item.longitude_deg = 8.550138;
    new_item.vehicle_action = Mission::MissionItem::VehicleAction::None;
    mission_plan.mission_items.push_back(new_item);

    new_item.latitude_deg = 47.398570;
    new_item.vehicle_action = Mission::MissionItem::VehicleAction::TransitionToMc;
    mission_plan.mission_items.push_back(new_item);

    new_item.latitude_deg = 47.398770;
    new_item.vehicle_action = Mission::MissionItem::VehicleAction::Land;
    mission_plan.mission_items.push_back(new_item);

    LogInfo() << "Uploading mission...";
    ASSERT_EQ(mission->upload_mission(mission_plan), Mission::Result::Success);

    LogInfo() << "Downloading mission...";
    const auto download_result = mission->download_mission();
    ASSERT_EQ(download_result.first, Mission::Result::Success);

    EXPECT_EQ(mission_plan.mission_items.size(), download_result.second.mission_items.size());

    if (mission_plan.mission_items.size() == download_result.second.mission_items.size()) {
        for (unsigned i = 0; i < mission_plan.mission_items.size(); ++i) {
            const auto original = mission_plan.mission_items.at(i);
            const auto downloaded = download_result.second.mission_items.at(i);
            EXPECT_EQ(original, downloaded);
        }
    }

    pc.set_min_altitude(0.0f);
    pc.set_next_reach_altitude(mission_altitude_m);
    pc.set_max_altitude(mission_altitude_m);
    pc.set_margin(6);

    LogInfo() << "Arming...";
    const Action::Result arm_result = action->arm();
    ASSERT_EQ(arm_result, Action::Result::Success);
    LogInfo() << "Armed.";

    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    mission->subscribe_mission_progress([](Mission::MissionProgress progress) {
        LogInfo() << "Mission status update: " << progress.current << " / " << progress.total;
    });

    {
        LogInfo() << "Starting mission.";
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission->start_mission_async([prom](Mission::Result result) {
            ASSERT_EQ(result, Mission::Result::Success);
            prom->set_value();
            LogInfo() << "Started mission.";
        });

        auto status = future_result.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        future_result.get();
    }

    while (!mission->is_mission_finished().second) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    while (telemetry->armed()) {
        // Wait until we're done.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(vtol_states_template.at(i), vtol_states.at(i));
    }

    LogInfo() << "Disarmed, exiting.";
}
