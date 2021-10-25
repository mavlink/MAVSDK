#include "integration_test_helper.h"
#include "global_include.h"
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/custom_action/custom_action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/param/param.h"

#include <future>

using namespace mavsdk;
using namespace std::placeholders;
using namespace std::chrono_literals;

static std::atomic<double> _action_progress{0};
static std::atomic<CustomAction::Result> _action_result;
static std::atomic<bool> in_air{false};
static std::atomic<bool> on_ground{false};

static std::pair<Param::Result, float> get_com_disarm_land{};

static void send_progress_status(std::shared_ptr<CustomAction> custom_action);
static void process_custom_action(
    CustomAction::ActionToExecute action,
    std::shared_ptr<CustomAction> custom_action,
    std::shared_ptr<Param> param);
static void execute_stages(
    CustomAction::ActionMetadata action_metadata,
    std::shared_ptr<CustomAction> custom_action,
    std::shared_ptr<Param> param);

TEST_F(SitlTest, CustomAction)
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
    // The GCS is also capable of sending the custom action commands
    auto telemetry = std::make_shared<Telemetry>(system_to_gcs);
    auto action = std::make_shared<Action>(system_to_gcs);
    auto custom_action_gcs = std::make_shared<CustomAction>(system_to_gcs);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Configure MAVSDK mission computer instance
    Mavsdk mavsdk_companion;

    // Change configuration so the instance is treated as a mission computer
    mavsdk::Mavsdk::Configuration configuration(1, 193, true);
    mavsdk_companion.set_configuration(configuration);

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
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(system_to_companion->is_connected());
    ASSERT_TRUE(system_to_companion->has_autopilot());

    // Custom actions are processed and executed in the mission computer
    auto custom_action_comp = std::make_shared<CustomAction>(system_to_companion);
    auto param = std::make_shared<Param>(system_to_companion);

    // Get COM_DISARM_LAND current value.
    get_com_disarm_land = param->get_param_float("COM_DISARM_LAND");
    ASSERT_EQ(get_com_disarm_land.first, Param::Result::Success);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Start tests
    {
        LogDebug() << "Waiting to be ready...";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        telemetry->subscribe_health_all_ok([&telemetry, &prom](bool all_ok) {
            if (all_ok) {
                // Unregister to prevent fulfilling promise twice.
                telemetry->subscribe_health_all_ok(nullptr);
                prom.set_value();
            }
        });
        ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    }

    // Get the in-air state
    telemetry->subscribe_in_air([](bool in_air_state) { in_air = in_air_state; });

    {
        LogInfo() << "Arming";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        action->arm_async([&prom](Action::Result result) {
            EXPECT_EQ(result, Action::Result::Success);
            prom.set_value();
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    LogInfo() << "Setting takeoff altitude";
    action->set_takeoff_altitude(5.0f);

    {
        LogInfo() << "Taking off";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        action->takeoff_async([&prom](Action::Result result) {
            EXPECT_EQ(result, Action::Result::Success);
            prom.set_value();
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
    {
        LogInfo() << "Sending custom action";
        std::promise<void> prom_res;
        std::future<void> fut_res = prom_res.get_future();

        // Send command to start custom action 0
        CustomAction::ActionToExecute action_to_execute{};
        action_to_execute.id = 0;
        action_to_execute.timeout = 20;

        custom_action_gcs->set_custom_action_async(
            action_to_execute, [&prom_res](CustomAction::Result result) {
                EXPECT_EQ(result, CustomAction::Result::Success);
                prom_res.set_value();
            });

        // Get the custom action to process
        std::promise<CustomAction::ActionToExecute> prom_act;
        std::future<CustomAction::ActionToExecute> fut_act = prom_act.get_future();
        custom_action_comp->subscribe_custom_action(
            [&prom_act, &custom_action_comp](CustomAction::ActionToExecute action_to_exec) {
                custom_action_comp->subscribe_custom_action(nullptr);
                prom_act.set_value(action_to_exec);
            });

        fut_act.wait();
        LogInfo() << "Process custom action";
        CustomAction::ActionToExecute action_exec = fut_act.get();

        // Process the custom action
        process_custom_action(action_exec, custom_action_comp, param);
    }

    {
        LogInfo() << "Landing";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        action->land_async([&prom](Action::Result result) {
            EXPECT_EQ(result, Action::Result::Success);
            prom.set_value();
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    {
        LogInfo() << "Waiting to be landed...";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        telemetry->subscribe_in_air([&telemetry, &prom](bool in_air_state) {
            if (!in_air_state) {
                // Unregister to prevent fulfilling promise twice.
                telemetry->subscribe_in_air(nullptr);
                prom.set_value();
            }
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(20)), std::future_status::ready);
    }

    {
        LogInfo() << "Disarming";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        action->disarm_async([&prom](Action::Result result) {
            EXPECT_EQ(result, Action::Result::Success);
            prom.set_value();
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }
}

void send_progress_status(std::shared_ptr<CustomAction> custom_action)
{
    while (_action_result.load() != CustomAction::Result::Unknown) {
        CustomAction::ActionToExecute action_exec{};
        action_exec.progress = static_cast<int32_t>(_action_progress.load());
        auto action_result = _action_result.load();

        std::promise<void> prom;
        std::future<void> fut = prom.get_future();

        // Send response with the result and the progress
        custom_action->respond_custom_action_async(
            action_exec, action_result, [&prom](CustomAction::Result result) {
                EXPECT_EQ(result, CustomAction::Result::Success);
                prom.set_value();
            });

        std::this_thread::sleep_for(std::chrono::seconds(1));
    };
}

void process_custom_action(
    CustomAction::ActionToExecute action,
    std::shared_ptr<CustomAction> custom_action,
    std::shared_ptr<Param> param)
{
    // Note that this (client) function is not custom action generic, in the sense that
    // it a priori knows the number stages the action being executed is composed of
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
    CustomAction::ActionMetadata action_metadata = fut.get();

    EXPECT_EQ(action_metadata.id, 0);
    EXPECT_EQ(action_metadata.action_name, "Integration test - land and takeoff action");
    EXPECT_EQ(action_metadata.action_description, "Example action of a land and takeoff procedure");
    LogInfo() << "Custom action #" << action_metadata.id << " is \"" << action_metadata.action_name
              << "\"";

    // Start
    _action_result.store(CustomAction::Result::InProgress, std::memory_order_relaxed);
    _action_progress.store(0.0, std::memory_order_relaxed);
    LogInfo() << "Custom action #" << action_metadata.id
              << " current progress: " << _action_progress.load() << "%";

    // Start the progress status report thread
    std::thread status_th(send_progress_status, custom_action);

    // Start the custom action execution
    execute_stages(action_metadata, custom_action, param);
    // EXPECT_DOUBLE_EQ(_action_progress, 100.0);

    status_th.join();
}

void execute_stages(
    CustomAction::ActionMetadata action_metadata,
    std::shared_ptr<CustomAction> custom_action,
    std::shared_ptr<Param> param)
{
    // First stage - Set COM_DISARM_LAND to 0.0
    {
        CustomAction::Result stage_res =
            custom_action->execute_custom_action_stage(action_metadata.stages[0]);
        EXPECT_EQ(stage_res, CustomAction::Result::Success);

        if (stage_res == CustomAction::Result::Success) {
            _action_progress.store(25.0, std::memory_order_relaxed);
            _action_result.store(CustomAction::Result::Success, std::memory_order_relaxed);
            LogInfo() << "Custom action #" << action_metadata.id
                      << " current progress: " << _action_progress.load() << "%";
        }
    }

    // Verify COM_DISARM_LAND was set to 0.0
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::pair<Param::Result, float> verify_get_com_disarm_land =
        param->get_param_float("COM_DISARM_LAND");
    EXPECT_EQ(verify_get_com_disarm_land.first, Param::Result::Success);
    EXPECT_FLOAT_EQ(verify_get_com_disarm_land.second, 0.0f);

    // Second stage - Land
    {
        CustomAction::Result stage_res =
            custom_action->execute_custom_action_stage(action_metadata.stages[1]);
        EXPECT_EQ(stage_res, CustomAction::Result::Success);

        // Wait until it is landed
        while (in_air) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (stage_res == CustomAction::Result::Success) {
            _action_progress.store(50.0, std::memory_order_relaxed);
            _action_result.store(CustomAction::Result::Success, std::memory_order_relaxed);
            LogInfo() << "Custom action #" << action_metadata.id
                      << " current progress: " << _action_progress.load() << "%";
        }
    }

    // Third stage - Set COM_DISARM_LAND to 2.0
    {
        CustomAction::Result stage_res =
            custom_action->execute_custom_action_stage(action_metadata.stages[2]);
        EXPECT_EQ(stage_res, CustomAction::Result::Success);

        if (stage_res == CustomAction::Result::Success) {
            _action_progress.store(75.0, std::memory_order_relaxed);
            _action_result.store(CustomAction::Result::Success, std::memory_order_relaxed);
            LogInfo() << "Custom action #" << action_metadata.id
                      << " current progress: " << _action_progress.load() << "%";
        }
    }

    // Verify COM_DISARM_LAND reset.
    std::this_thread::sleep_for(std::chrono::seconds(1));
    verify_get_com_disarm_land = param->get_param_float("COM_DISARM_LAND");
    EXPECT_EQ(verify_get_com_disarm_land.first, Param::Result::Success);
    EXPECT_FLOAT_EQ(verify_get_com_disarm_land.second, get_com_disarm_land.second);

    // Fourth stage - Takeoff
    {
        CustomAction::Result stage_res =
            custom_action->execute_custom_action_stage(action_metadata.stages[3]);
        EXPECT_EQ(stage_res, CustomAction::Result::Success);

        // Wait until it is in air
        while (!in_air) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (stage_res == CustomAction::Result::Success) {
            _action_progress.store(100.0, std::memory_order_relaxed);
            _action_result.store(CustomAction::Result::Success, std::memory_order_relaxed);
            LogInfo() << "Custom action #" << action_metadata.id
                      << " current progress: " << _action_progress.load() << "%";
        }
    }

    // End
    {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // Used to stop the threads
        _action_result.store(CustomAction::Result::Unknown, std::memory_order_relaxed);

        LogInfo() << "Custom action #" << action_metadata.id << " executed!";
    }
}
