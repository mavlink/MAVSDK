#include <iostream>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

using namespace dronecode_sdk;

// static void connect(DronecodeSDK);
static void takeoff(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry);
static void takeoff_and_transition_to_fixedwing();
static void land_and_disarm(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry);

TEST_F(SitlTest, ActionTransitionSync_standard_vtol)
{
    takeoff_and_transition_to_fixedwing();
}

void takeoff_and_transition_to_fixedwing()
{
    // Init & connect
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_TRUE(dc.is_connected());

    System &system = dc.system();
    auto action = std::make_shared<Action>(system);
    auto telemetry = std::make_shared<Telemetry>(system);

    // We need to takeoff first, otherwise we can't actually transition
    LogInfo() << "Taking off";
    takeoff(action, telemetry);

    LogInfo() << "Transitioning to fixedwing";
    Action::Result transition_result = action->transition_to_fixedwing();
    EXPECT_EQ(transition_result, Action::Result::SUCCESS);

    // Wait a little before the transition back to multicopter,
    // so we can actually see it fly
    std::this_thread::sleep_for(std::chrono::seconds(5));

    LogInfo() << "Transitioning to multicopter";
    transition_result = action->transition_to_multicopter();
    EXPECT_EQ(transition_result, Action::Result::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Return safely to launch position so the next test
    // can start with a clean slate
    land_and_disarm(action, telemetry);
}

void land_and_disarm(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry)
{
    action->return_to_launch();

    // Wait until the vtol is disarmed.
    while (telemetry->armed()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LogInfo() << "Disarmed, exiting.";
}

void takeoff(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry)
{
    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = action->arm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    float altitude_m = 10.0f;
    action->set_takeoff_altitude(altitude_m);

    action_ret = action->takeoff();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    const int wait_time_s = 10;
    std::this_thread::sleep_for(std::chrono::seconds(wait_time_s));

    EXPECT_GT(telemetry->position().relative_altitude_m, altitude_m - 0.25f);
    EXPECT_LT(telemetry->position().relative_altitude_m, altitude_m + 0.25f);
}
