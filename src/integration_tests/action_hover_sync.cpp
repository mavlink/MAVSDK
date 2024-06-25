#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

using namespace mavsdk;

static void takeoff_and_hover_at_altitude(float altitude_m);
void takeoff_and_hover_at_altitude_apm(float altitude_m);

TEST(SitlTest, PX4ActionHoverSyncDefault)
{
    takeoff_and_hover_at_altitude(2.5);
}

TEST(SitlTest, PX4ActionHoverSyncHigher)
{
    takeoff_and_hover_at_altitude(5.0f);
}

TEST(SitlTest, PX4ActionHoverSyncLower)
{
    // TODO: less than 1.0 is currently failing due to a Firmware bug.
    // https://github.com/PX4/Firmware/issues/12471
    takeoff_and_hover_at_altitude(1.5f);
}

TEST(SitlTest, APMActionHoverSyncDefault)
{
    takeoff_and_hover_at_altitude_apm(2.5);
}

TEST(SitlTest, APMActionHoverSyncHigher)
{
    takeoff_and_hover_at_altitude_apm(5.0f);
}

TEST(SitlTest, APMActionHoverSyncLower)
{
    // TODO: less than 1.0 is currently failing due to a Firmware bug.
    // https://github.com/PX4/Firmware/issues/12471
    takeoff_and_hover_at_altitude_apm(1.5f);
}

void takeoff_and_hover_at_altitude(float altitude_m)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    LogInfo() << "Waiting for system connect";
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

    auto systems = mavsdk.systems();

    ASSERT_EQ(systems.size(), 1);

    auto system = systems.at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(*system);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    auto action = std::make_shared<Action>(*system);

    EXPECT_EQ(Action::Result::Success, action->set_takeoff_altitude(altitude_m));
    auto takeoff_altitude_result = action->get_takeoff_altitude();
    EXPECT_EQ(takeoff_altitude_result.first, Action::Result::Success);
    EXPECT_FLOAT_EQ(takeoff_altitude_result.second, altitude_m);

    Action::Result action_ret = action->arm();
    EXPECT_EQ(action_ret, Action::Result::Success);

    action_ret = action->takeoff();
    EXPECT_EQ(action_ret, Action::Result::Success);

    EXPECT_TRUE(poll_condition_with_timeout(
        [telemetry]() { return telemetry->flight_mode() == Telemetry::FlightMode::Takeoff; },
        std::chrono::seconds(10)));

    EXPECT_TRUE(poll_condition_with_timeout(
        [telemetry]() { return telemetry->flight_mode() == Telemetry::FlightMode::Hold; },
        std::chrono::seconds(20)));

    // We need to wait a bit until it stabilizes.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // FIXME: we can't expect too much precision apparently.
    EXPECT_GT(telemetry->position().relative_altitude_m, altitude_m - 0.5f);
    EXPECT_LT(telemetry->position().relative_altitude_m, altitude_m + 0.5f);

    action_ret = action->land();
    EXPECT_EQ(action_ret, Action::Result::Success);

    EXPECT_TRUE(poll_condition_with_timeout(
        [telemetry]() { return !telemetry->in_air(); }, std::chrono::seconds(20)));

    action_ret = action->disarm();
    EXPECT_EQ(action_ret, Action::Result::Success);
}

void takeoff_and_hover_at_altitude_apm(float altitude_m)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    LogInfo() << "Waiting for system connect";
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

    auto systems = mavsdk.systems();

    ASSERT_EQ(systems.size(), 1);

    auto system = systems.at(0);
    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(*system);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    auto action = std::make_shared<Action>(*system);

    EXPECT_EQ(Action::Result::Success, action->set_takeoff_altitude(altitude_m));
    auto takeoff_altitude_result = action->get_takeoff_altitude();
    EXPECT_EQ(takeoff_altitude_result.first, Action::Result::Success);
    EXPECT_FLOAT_EQ(takeoff_altitude_result.second, altitude_m);

    Action::Result action_ret = action->arm();
    EXPECT_EQ(action_ret, Action::Result::Success);

    action_ret = action->takeoff();
    EXPECT_EQ(action_ret, Action::Result::Success);

    // We need to wait a bit until it stabilizes.
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // FIXME: we can't expect too much precision apparently.
    EXPECT_GT(telemetry->position().relative_altitude_m, altitude_m - 0.5f);
    EXPECT_LT(telemetry->position().relative_altitude_m, altitude_m + 0.5f);

    action_ret = action->land();
    EXPECT_EQ(action_ret, Action::Result::Success);

    EXPECT_TRUE(poll_condition_with_timeout(
        [telemetry]() { return !telemetry->in_air(); }, std::chrono::seconds(20)));

    action_ret = action->disarm();
    EXPECT_EQ(action_ret, Action::Result::Success);
}
