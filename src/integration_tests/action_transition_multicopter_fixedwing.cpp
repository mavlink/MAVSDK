#include <future>
#include <iostream>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

using namespace mavsdk;

TEST(SitlTest, PX4ActionTransitionSync_standard_vtol)
{
    // Init & connect
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
    auto action = std::make_shared<Action>(system);
    auto telemetry = std::make_shared<Telemetry>(system);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    // We need to takeoff first, otherwise we can't actually transition
    float altitude_m = 10.0f;
    action->set_takeoff_altitude(altitude_m);

    Action::Result action_ret = action->arm();
    ASSERT_EQ(action_ret, Action::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    LogInfo() << "Taking off";
    action_ret = action->takeoff();
    ASSERT_EQ(action_ret, Action::Result::Success);

    LogInfo() << "Waiting until hovering";
    auto prom = std::promise<void>{};
    auto fut = prom.get_future();
    // Wait until hovering.
    Telemetry::FlightModeHandle handle =
        telemetry->subscribe_flight_mode([&telemetry, &prom, &handle](Telemetry::FlightMode mode) {
            if (mode == Telemetry::FlightMode::Hold) {
                telemetry->unsubscribe_flight_mode(handle);
                prom.set_value();
            }
        });

    ASSERT_EQ(fut.wait_for(std::chrono::seconds(20)), std::future_status::ready);

    LogInfo() << "Transitioning to fixedwing";
    Action::Result transition_result = action->transition_to_fixedwing();
    EXPECT_EQ(transition_result, Action::Result::Success);

    // Wait a little before the transition back to multicopter,
    // so we can actually see it fly
    std::this_thread::sleep_for(std::chrono::seconds(5));

    LogInfo() << "Transitioning to multicopter";
    transition_result = action->transition_to_multicopter();
    EXPECT_EQ(transition_result, Action::Result::Success);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Return safely to launch position so the next test
    // can start with a clean slate
    action->return_to_launch();

    // Wait until the vtol is disarmed.
    while (telemetry->armed()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    LogInfo() << "Disarmed, exiting.";
}
