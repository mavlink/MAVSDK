#include <iostream>
#include <future>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

using namespace mavsdk;

TEST(SitlTest, ActionTakeoffAndKill)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ASSERT_EQ(mavsdk.add_any_connection("udpin://0.0.0.0:14540"), ConnectionResult::Success);

    {
        LogInfo() << "Waiting to discover vehicle";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        mavsdk.subscribe_on_new_system([&mavsdk, &prom]() {
            const auto system = mavsdk.systems().at(0);

            if (system->is_connected()) {
                // Unregister to prevent fulfilling promise twice.
                mavsdk.subscribe_on_new_system(nullptr);
                prom.set_value();
            }
        });

        ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    }

    auto system = mavsdk.systems().at(0);
    ASSERT_TRUE(system->has_autopilot());
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);

    {
        LogDebug() << "Waiting to be ready...";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        Telemetry::HealthAllOkHandle handle =
            telemetry->subscribe_health_all_ok([&telemetry, &prom, &handle](bool all_ok) {
                if (all_ok) {
                    // Unregister to prevent fulfilling promise twice.
                    telemetry->unsubscribe_health_all_ok(handle);
                    prom.set_value();
                }
            });
        ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    }

    // FIXME: We only want to takeoff to an altitude to maybe 0.5 meters, however,
    //        that's currently broken in PX4, so we use a higher altitude.
    //        Also see: https://github.com/PX4/Firmware/issues/12471
    action->set_takeoff_altitude(1.5f);

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

    EXPECT_TRUE(poll_condition_with_timeout(
        [&telemetry]() { return telemetry->position().relative_altitude_m > 0.2f; },
        std::chrono::seconds(8)));

    // Kill it and hope it doesn't come down upside down, ready to fly again :)
    {
        LogInfo() << "Kill it";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        action->kill_async([&prom](Action::Result result) {
            EXPECT_EQ(result, Action::Result::Success);
            prom.set_value();
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    EXPECT_TRUE(poll_condition_with_timeout(
        [&telemetry]() { return !telemetry->armed(); }, std::chrono::seconds(2)));

    // The land detector takes some time.
    EXPECT_TRUE(poll_condition_with_timeout(
        [&telemetry]() { return !telemetry->in_air(); }, std::chrono::seconds(2)));
}
