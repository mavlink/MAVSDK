#include <iostream>
#include "integration_test_helper.h"
#include "dronecore.h"

using namespace dronecore;


//static void connect(DroneCore);
static void takeoff(Device &);
static void takeoff_and_transition_to_fixedwing();
static void land_and_disarm(Device &);


TEST_F(SitlTest, ActionSimpleTransition)
{
    takeoff_and_transition_to_fixedwing();
}

void takeoff_and_transition_to_fixedwing()
{
    // Init & connect
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, DroneCore::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_TRUE(dc.is_connected());

    Device &device = dc.device();

    // We need to takeoff first, otherwise we can't actually transition
    LogInfo() << "Taking off";
    takeoff(device);

    LogInfo() << "Transitioning to fixedwing";
    Action::Result transition_result = device.action().transition_to_fixedwing();
    EXPECT_EQ(transition_result, Action::Result::SUCCESS);

    // Wait a little before the transition back to multicopter,
    // so we can actually see it fly
    std::this_thread::sleep_for(std::chrono::seconds(5));

    LogInfo() << "Transitioning to multicopter";
    transition_result = device.action().transition_to_multicopter();
    EXPECT_EQ(transition_result, Action::Result::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Return safely to launch position so the next test
    // can start with a clean slate
    land_and_disarm(device);
}

void land_and_disarm(Device &device)
{
    device.action().return_to_launch();

    // Wait until the vtol is disarmed.
    while (device.telemetry().armed()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LogInfo() << "Disarmed, exiting.";
}

void takeoff(Device &device)
{
    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = device.action().arm();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    float altitude_m = 10.0f;
    device.action().set_takeoff_altitude(altitude_m);

    action_ret = device.action().takeoff();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
    const int wait_time_s = 10;
    std::this_thread::sleep_for(std::chrono::seconds(wait_time_s));


    EXPECT_GT(device.telemetry().position().relative_altitude_m, altitude_m - 0.25f);
    EXPECT_LT(device.telemetry().position().relative_altitude_m, altitude_m + 0.25f);
}

