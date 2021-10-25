#include <iostream>
#include <functional>
#include <memory>
#include <future>
#include <atomic>
#include <cmath>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/action/action.h"
#include "plugins/custom_action/custom_action.h"
#include "plugins/mission_raw/mission_raw.h"

using namespace mavsdk;
using namespace std::placeholders;
using namespace std::chrono_literals;

static void test_mission(
    std::shared_ptr<Telemetry> telemetry,
    std::shared_ptr<MissionRaw> mission_raw,
    std::shared_ptr<Action> action,
    std::shared_ptr<CustomAction> custom_action);

static MissionRaw::MissionItem add_mission_raw_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    uint16_t sequence,
    uint8_t frame,
    uint16_t command,
    uint8_t current,
    uint8_t autocontinue,
    float param1,
    float param2,
    float param3,
    float param4,
    uint8_t mission_type);

static std::atomic<bool> _mission_finished{false};
static std::atomic<bool> _action_stopped{false};
static std::atomic<bool> _new_action{false};
static std::atomic<bool> _new_actions_check_int{false};
static std::atomic<bool> _in_air{false};

static std::vector<CustomAction::ActionToExecute> _actions;
static std::vector<double> _actions_progress;
static std::vector<CustomAction::Result> _actions_result;
static std::vector<CustomAction::ActionMetadata> _actions_metadata;
static std::vector<std::thread> _progress_threads;

static std::mutex cancel_mtx;
static std::condition_variable cancel_signal;

float _progress_current{0};
float _progress_total{0};

static void new_action_check(std::shared_ptr<CustomAction> custom_action);
static void send_progress_status(
    std::shared_ptr<CustomAction> custom_action, CustomAction::ActionToExecute action_metadata);
static void process_custom_action(
    CustomAction::ActionToExecute action, std::shared_ptr<CustomAction> custom_action);
static void execute_custom_action(
    CustomAction::ActionMetadata action_metadata, std::shared_ptr<CustomAction> custom_action);
static void update_action_progress_from_stage(
    const unsigned& stage_idx, const CustomAction::ActionMetadata& action_metadata);

TEST_F(SitlTest, CustomActionMission)
{
    // Configure MAVSDK GCS instance
    Mavsdk mavsdk_gcs;

    ConnectionResult ret_gcs = mavsdk_gcs.add_udp_connection(14550);
    ASSERT_EQ(ret_gcs, ConnectionResult::Success);

    auto system_to_gcs = std::shared_ptr<System>{nullptr};
    {
        LogInfo() << "Waiting to discover vehicle from the GCS side";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();

        mavsdk_gcs.subscribe_on_new_system([&prom, &mavsdk_gcs, &system_to_gcs]() {
            for (auto& system : mavsdk_gcs.systems()) {
                if (system->has_autopilot() && system->is_connected()) {
                    system_to_gcs = system;
                    // Unsubscribe again as we only want to find one system.
                    mavsdk_gcs.subscribe_on_new_system(nullptr);
                    prom.set_value();
                    break;
                }
            }
        });

        ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    }
    ASSERT_TRUE(system_to_gcs->is_connected());
    ASSERT_TRUE(system_to_gcs->has_autopilot());

    // Telemetry and actions are received and sent from the GCS respectively
    // GCS sends the mission as well
    auto telemetry = std::make_shared<Telemetry>(system_to_gcs);
    auto mission_raw = std::make_shared<MissionRaw>(system_to_gcs);
    auto action = std::make_shared<Action>(system_to_gcs);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Configure MAVSDK mission computer instance
    Mavsdk mavsdk_companion;

    // Change configuration so the instance is treated as a mission computer
    Mavsdk::Configuration config_cc(Mavsdk::Configuration::UsageType::CompanionComputer);
    mavsdk_companion.set_configuration(config_cc);

    ConnectionResult ret_comp = mavsdk_companion.add_udp_connection(14540);
    ASSERT_EQ(ret_comp, ConnectionResult::Success);

    auto system_to_companion = std::shared_ptr<System>{nullptr};
    {
        LogInfo() << "Waiting to discover vehicle from the mission computer side";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();

        mavsdk_companion.subscribe_on_new_system(
            [&prom, &mavsdk_companion, &system_to_companion]() {
                for (auto& system : mavsdk_companion.systems()) {
                    if (system->has_autopilot() && system->is_connected()) {
                        system_to_companion = system;
                        // Unsubscribe again as we only want to find one system.
                        mavsdk_companion.subscribe_on_new_system(nullptr);
                        prom.set_value();
                        break;
                    }
                }
            });

        ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    }
    ASSERT_TRUE(system_to_companion->is_connected());
    ASSERT_TRUE(system_to_companion->has_autopilot());

    // Custom actions are processed and executed in the mission computer
    auto custom_action = std::make_shared<CustomAction>(system_to_companion);

    test_mission(telemetry, mission_raw, action, custom_action);
}

void test_mission(
    std::shared_ptr<Telemetry> telemetry,
    std::shared_ptr<MissionRaw> mission_raw,
    std::shared_ptr<Action> action,
    std::shared_ptr<CustomAction> custom_action)
{
    while (!telemetry->health_all_ok()) {
        LogInfo() << "Waiting for system to be ready";
        LogDebug() << "Health: " << telemetry->health();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LogInfo() << "System ready";

    // Get the home position so the waypoint mission items are set with respect
    // to the home position instead of being hardcoded.
    auto home = telemetry->home();
    LogInfo() << "Vehicle home " << home;

    LogInfo() << "Creating and uploading mission";
    std::vector<MissionRaw::MissionItem> mission_raw_items;

    // Normal waypoint
    mission_raw_items.push_back(add_mission_raw_item(
        home.latitude_deg + 0.000419627,
        home.longitude_deg - 0.000041622,
        10.0f,
        0,
        6, // MAV_FRAME_GLOBAL_RELATIVE_ALT_INT
        16, // MAV_CMD_NAV_WAYPOINT
        1,
        1,
        1.0, // Hold
        1.0, // Acceptance Radius
        1.0, // Pass Radius
        NAN, // Yaw
        0 // MAV_MISSION_TYPE_MISSION
        ));

    // Custom action waypoint to turn-on air quality sensor
    mission_raw_items.push_back(add_mission_raw_item(
        NAN,
        NAN,
        NAN,
        1,
        2, // MAV_FRAME_MISSION
        31000, // MAV_CMD_WAYPOINT_USER_1
        0,
        1,
        1.0, // Action ID
        0.0, // Action execution control
        2.0, // Action timeout (in seconds)
        NAN,
        0 // MAV_MISSION_TYPE_MISSION
        ));

    // Normal waypoint
    mission_raw_items.push_back(add_mission_raw_item(
        home.latitude_deg + 0.000490638,
        home.longitude_deg - 0.000071389,
        10.0f,
        2,
        6, // MAV_FRAME_GLOBAL_RELATIVE_ALT_INT
        16, // MAV_CMD_NAV_WAYPOINT
        0,
        1,
        0.0, // Hold
        1.0, // Acceptance Radius
        1.0, // Pass Radius
        NAN, // Yaw
        0 // MAV_MISSION_TYPE_MISSION
        ));

    // Custom action waypoint to do a payload delivery
    // using a winch
    mission_raw_items.push_back(add_mission_raw_item(
        NAN,
        NAN,
        NAN,
        3,
        2, // MAV_FRAME_MISSION
        31000, // MAV_CMD_WAYPOINT_USER_1
        0,
        1,
        3.0, // Action ID
        0.0, // Action execution control
        16.0, // Action timeout (in seconds)
        NAN,
        0 // MAV_MISSION_TYPE_MISSION
        ));

    // Normal waypoint (Hold WP for delivery)
    mission_raw_items.push_back(add_mission_raw_item(
        home.latitude_deg + 0.000490638,
        home.longitude_deg - 0.000071389,
        10.0f,
        4,
        6, // MAV_FRAME_GLOBAL_RELATIVE_ALT_INT
        16, // MAV_CMD_NAV_WAYPOINT
        0,
        1,
        16.0, // Hold
        1.0, // Acceptance Radius
        1.0, // Pass Radius
        NAN, // Yaw
        0 // MAV_MISSION_TYPE_MISSION
        ));

    // Normal waypoint
    mission_raw_items.push_back(add_mission_raw_item(
        home.latitude_deg + 0.000419627,
        home.longitude_deg - 0.000041622,
        10.0f,
        5,
        6, // MAV_FRAME_GLOBAL_RELATIVE_ALT_INT
        16, // MAV_CMD_NAV_WAYPOINT
        0,
        1,
        1.0, // Hold
        1.0, // Acceptance Radius
        1.0, // Pass Radius
        NAN, // Yaw
        0 // MAV_MISSION_TYPE_MISSION
        ));

    // Custom action waypoint to turn-off air quality sensor
    mission_raw_items.push_back(add_mission_raw_item(
        NAN,
        NAN,
        NAN,
        6,
        2, // MAV_FRAME_MISSION
        31000, // MAV_CMD_WAYPOINT_USER_1
        0,
        1,
        2.0, // Action ID
        0.0, // Action execution control
        2.0, // Action timeout (in seconds)
        NAN,
        0 // MAV_MISSION_TYPE_MISSION
        ));

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

    // Get the in-air state
    telemetry->subscribe_in_air([](bool in_air) { _in_air = in_air; });

    LogInfo() << "Arming...";
    const Action::Result arm_result = action->arm();
    EXPECT_EQ(arm_result, Action::Result::Success);
    LogInfo() << "Armed.";

    // Before starting the mission, we want to be sure to subscribe to the mission progress.
    mission_raw->subscribe_mission_progress([&mission_raw](MissionRaw::MissionProgress progress) {
        if (!_mission_finished) {
            LogInfo() << "Mission status update: " << progress.current << " / " << progress.total;
        }

        _progress_current = static_cast<float>(progress.current);
        _progress_total = static_cast<float>(progress.total);

        if (progress.current == progress.total && !_mission_finished) {
            _mission_finished = true;
        }
    });

    // Subscribe to the cancelation message
    custom_action->subscribe_custom_action_cancellation(
        [](bool canceled) { _action_stopped.store(canceled, std::memory_order_relaxed); });

    {
        LogInfo() << "Starting mission.";
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission_raw->start_mission_async([prom](MissionRaw::Result result) {
            ASSERT_EQ(result, MissionRaw::Result::Success);
            prom->set_value();
            LogInfo() << "Started mission.";
        });

        auto status = future_result.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        future_result.get();
    }

    auto new_actions_check_th = std::thread(new_action_check, custom_action);

    {
        // Get the custom action to process
        custom_action->subscribe_custom_action([](CustomAction::ActionToExecute action_to_exec) {
            if (_actions.empty() ||
                (!_actions.empty() && _actions.back().id != action_to_exec.id)) {
                _actions.push_back(action_to_exec);
                _new_action.store(true, std::memory_order_relaxed);
                LogInfo() << "New action received: " << action_to_exec.id;
            }
        });
    }

    while (!_mission_finished) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    _new_actions_check_int.store(true, std::memory_order_relaxed);
    new_actions_check_th.join();

    {
        LogInfo() << "Return-to-launch";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        action->return_to_launch_async([&prom](Action::Result result) {
            EXPECT_EQ(result, Action::Result::Success);
            prom.set_value();
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    while (telemetry->armed()) {
        // Wait until we're done.
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LogInfo() << "Disarmed.";

    {
        LogInfo() << "Clearing mission.";
        auto prom = std::make_shared<std::promise<void>>();
        auto future_result = prom->get_future();
        mission_raw->clear_mission_async([prom](MissionRaw::Result result) {
            ASSERT_EQ(result, MissionRaw::Result::Success);
            prom->set_value();
            LogInfo() << "Cleared mission, exiting.";
        });

        auto status = future_result.wait_for(std::chrono::seconds(2));
        ASSERT_EQ(status, std::future_status::ready);
        future_result.get();
    }
}

MissionRaw::MissionItem add_mission_raw_item(
    double latitude_deg,
    double longitude_deg,
    float relative_altitude_m,
    uint16_t sequence,
    uint8_t frame,
    uint16_t command,
    uint8_t current,
    uint8_t autocontinue,
    float param1,
    float param2,
    float param3,
    float param4,
    uint8_t mission_type)
{
    MissionRaw::MissionItem new_raw_item_nav{};
    new_raw_item_nav.seq = sequence;
    new_raw_item_nav.frame = frame;
    new_raw_item_nav.command = command;
    new_raw_item_nav.current = current;
    new_raw_item_nav.autocontinue = autocontinue;
    new_raw_item_nav.param1 = param1;
    new_raw_item_nav.param2 = param2;
    new_raw_item_nav.param3 = param3;
    new_raw_item_nav.param4 = param4;
    new_raw_item_nav.x = int32_t(std::round(latitude_deg * 1e7));
    new_raw_item_nav.y = int32_t(std::round(longitude_deg * 1e7));
    new_raw_item_nav.z = relative_altitude_m;
    new_raw_item_nav.mission_type = mission_type;

    return new_raw_item_nav;
}

void send_progress_status(
    std::shared_ptr<CustomAction> custom_action, CustomAction::ActionToExecute action)
{
    while (!_action_stopped.load() && _actions_result.back() != CustomAction::Result::Unknown) {
        CustomAction::ActionToExecute action_exec{};
        action_exec.id = action.id;
        action_exec.progress = _actions_progress.back();
        auto action_result = _actions_result.back();

        std::promise<void> prom;
        std::future<void> fut = prom.get_future();

        // Send response with the result and the progress
        custom_action->respond_custom_action_async(
            action_exec, action_result, [&prom](CustomAction::Result result) {
                EXPECT_EQ(result, CustomAction::Result::Success);
                prom.set_value();
            });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    };
}

void new_action_check(std::shared_ptr<CustomAction> custom_action)
{
    while (!_new_actions_check_int) {
        if (_new_action.load()) {
            process_custom_action(_actions.back(), custom_action);
            _new_action.store(false, std::memory_order_relaxed);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void process_custom_action(
    CustomAction::ActionToExecute action, std::shared_ptr<CustomAction> custom_action)
{
    LogInfo() << "Custom action #" << action.id << " being processed";

    // Get the custom action metadata
    std::promise<CustomAction::ActionMetadata> prom;
    std::future<CustomAction::ActionMetadata> fut = prom.get_future();
    custom_action->custom_action_metadata_async(
        action,
        "src/integration_tests/test_data/custom_action.json",
        [&prom](CustomAction::Result result, CustomAction::ActionMetadata action_metadata) {
            prom.set_value(action_metadata);
            EXPECT_EQ(result, CustomAction::Result::Success);
        });

    _actions_metadata.push_back(fut.get());

    // Start
    _actions_result.push_back(CustomAction::Result::InProgress);
    _actions_progress.push_back(0.0);
    LogInfo() << "Custom action #" << _actions_metadata.back().id
              << " current progress: " << _actions_progress.back() << "%";

    // Start the progress status report thread
    _progress_threads.push_back(std::thread(send_progress_status, custom_action, _actions.back()));

    // Start the custom action execution
    // For the purpose of the test, we are storing all the actions but only processing the last one.
    // This means that only one action at a time can be processed
    execute_custom_action(_actions_metadata.back(), custom_action);

    EXPECT_DOUBLE_EQ(_actions_progress.back(), 100.0);
    EXPECT_EQ(_actions_result.back(), CustomAction::Result::Success);

    // Used to stop the progress status thread
    _actions_result.back() = CustomAction::Result::Unknown;

    _progress_threads.back().join();
}

void update_action_progress_from_stage(
    const unsigned& stage_idx, const CustomAction::ActionMetadata& action_metadata)
{
    if (!_action_stopped.load()) {
        _actions_progress.back() = (stage_idx + 1.0) / action_metadata.stages.size() * 100.0;

        if (_actions_progress.back() != 100.0) {
            _actions_result.back() = CustomAction::Result::InProgress;
            LogInfo() << "Custom action #" << _actions_metadata.back().id
                      << " current progress: " << _actions_progress.back() << "%";
        } else {
            _actions_result.back() = CustomAction::Result::Success;
        }
    }
}

void execute_custom_action(
    CustomAction::ActionMetadata action_metadata, std::shared_ptr<CustomAction> custom_action)
{
    if (!action_metadata.stages.empty()) {
        for (unsigned i = 0; i < action_metadata.stages.size(); i++) {
            CustomAction::Result stage_res = CustomAction::Result::Unknown;

            if (!_action_stopped.load()) {
                LogInfo() << "Executing stage " << i << "of action #"
                          << _actions_metadata.back().id;
                stage_res = custom_action->execute_custom_action_stage(action_metadata.stages[i]);
                EXPECT_EQ(stage_res, CustomAction::Result::Success);
            }

            if (action_metadata.stages[i].state_transition_condition ==
                CustomAction::Stage::StateTransitionCondition::OnResultSuccess) {
                if (stage_res == CustomAction::Result::Success) {
                    update_action_progress_from_stage(i, action_metadata);
                }
            } else if (
                action_metadata.stages[i].state_transition_condition ==
                CustomAction::Stage::StateTransitionCondition::OnTimeout) {
                auto wait_time = action_metadata.stages[i].timeout * 1s;
                std::unique_lock<std::mutex> lock(cancel_mtx);
                cancel_signal.wait_for(lock, wait_time, []() { return _action_stopped.load(); });

                update_action_progress_from_stage(i, action_metadata);
            } else if (
                action_metadata.stages[i].state_transition_condition ==
                CustomAction::Stage::StateTransitionCondition::OnLandingComplete) {
                // Wait for the vehicle to be landed
                while (!_action_stopped.load() && _in_air) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }

                update_action_progress_from_stage(i, action_metadata);
            } else if (
                action_metadata.stages[i].state_transition_condition ==
                CustomAction::Stage::StateTransitionCondition::OnTakeoffComplete) {
                // Wait for the vehicle to be in-air
                while (!_action_stopped.load() && !_in_air) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }

                update_action_progress_from_stage(i, action_metadata);
            }
        }

    } else if (action_metadata.global_script != "") {
        if (!_action_stopped.load()) {
            CustomAction::Result result = CustomAction::Result::Unknown;

            if (action_metadata.action_complete_condition ==
                CustomAction::ActionMetadata::ActionCompleteCondition::OnResultSuccess) {
                if (!_action_stopped.load()) {
                    result = custom_action->execute_custom_action_global_script(
                        action_metadata.global_script);
                    EXPECT_EQ(result, CustomAction::Result::Success);
                }
            } else if (
                action_metadata.action_complete_condition ==
                CustomAction::ActionMetadata::ActionCompleteCondition::OnTimeout) {
                std::promise<CustomAction::Result> prom;
                std::future<CustomAction::Result> fut = prom.get_future();
                custom_action->execute_custom_action_global_script_async(
                    action_metadata.global_script,
                    [&prom](CustomAction::Result script_result) { prom.set_value(script_result); });

                std::chrono::seconds timeout(static_cast<long int>(action_metadata.global_timeout));
                EXPECT_EQ(fut.wait_for(timeout), std::future_status::ready);

                result = fut.get();
                EXPECT_EQ(result, CustomAction::Result::Success);
            }

            _actions_result.back() = result;

            if (result == CustomAction::Result::Success) {
                _actions_progress.back() = 100.0;
            }
        }
    }

    // We wait for half a second to make sure that the ACCEPTED ACKs are sent
    // to the FMU and don't get lost
    auto wait_time = 500ms;
    std::unique_lock<std::mutex> lock(cancel_mtx);
    cancel_signal.wait_for(lock, wait_time, []() { return _action_stopped.load(); });

    if (_action_stopped.load()) {
        LogWarn() << "Custom action #" << _actions_metadata.back().id << " canceled!";
        _action_stopped.store(false, std::memory_order_relaxed);
    } else if (!_action_stopped.load() && _actions_progress.back() == 100) {
        LogInfo() << "Custom action #" << _actions_metadata.back().id << " executed!";
    }
}
