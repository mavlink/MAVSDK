#include <iostream>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

using namespace dronecode_sdk;

TEST_F(SitlTest, ActionTakeoffAndKill)
{
    DronecodeSDK dc;
    ASSERT_EQ(dc.add_udp_connection(), ConnectionResult::SUCCESS);

    {
        LogInfo() << "Waiting to discover vehicle";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        dc.register_on_discover([&prom](uint64_t uuid) {
            prom.set_value();
            UNUSED(uuid);
        });
        ASSERT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);

    {
        LogDebug() << "Waiting to be ready...";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        telemetry->health_all_ok_async([&telemetry, &prom](bool all_ok) {
            if (all_ok) {
                // Unregister to prevent fulfilling promise twice.
                telemetry->health_all_ok_async(nullptr);
                prom.set_value();
            }
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    }

    action->set_takeoff_altitude(0.4);

    {
        LogInfo() << "Arming";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        action->arm_async([&prom](Action::Result result) {
            EXPECT_EQ(result, Action::Result::SUCCESS);
            prom.set_value();
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    {
        LogInfo() << "Taking off";
        std::promise<void> prom;
        std::future<void> fut = prom.get_future();
        action->takeoff_async([&prom](Action::Result result) {
            EXPECT_EQ(result, Action::Result::SUCCESS);
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
            EXPECT_EQ(result, Action::Result::SUCCESS);
            prom.set_value();
        });
        EXPECT_EQ(fut.wait_for(std::chrono::seconds(2)), std::future_status::ready);
    }

    EXPECT_TRUE(poll_condition_with_timeout([&telemetry]() { return !telemetry->armed(); },
                                            std::chrono::seconds(2)));

    // The land detector takes some time.
    EXPECT_TRUE(poll_condition_with_timeout([&telemetry]() { return !telemetry->in_air(); },
                                            std::chrono::seconds(2)));
}
