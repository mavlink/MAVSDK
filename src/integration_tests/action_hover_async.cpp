#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

#include <future>

using namespace mavsdk;

TEST(SitlTest, ActionHoverAsync)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    auto system = std::shared_ptr<System>{nullptr};
    {
        LogInfo() << "Waiting to discover vehicle";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();

        mavsdk.subscribe_on_new_system([&prom, &mavsdk, &system]() {
            if (mavsdk.systems().size() == 1) {
                system = mavsdk.systems().at(0);
                ASSERT_TRUE(system->has_autopilot());

                // Unregister to prevent fulfilling promise twice.
                mavsdk.subscribe_on_new_system(nullptr);
                prom.set_value();
            }
        });

        ASSERT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    }

    ASSERT_TRUE(system->is_connected());

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

    // TODO: adapt this time based on altitude
    std::this_thread::sleep_for(std::chrono::seconds(5));

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
        Telemetry::InAirHandle handle =
            telemetry->subscribe_in_air([&telemetry, &prom, &handle](bool in_air) {
                if (!in_air) {
                    // Unregister to prevent fulfilling promise twice.
                    telemetry->unsubscribe_in_air(handle);
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
