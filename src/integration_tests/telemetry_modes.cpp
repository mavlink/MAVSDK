#include <atomic>
#include <iostream>
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "integration_test_helper.h"

using namespace mavsdk;

void observe_mode(Telemetry::FlightMode flight_mode);
static std::atomic<Telemetry::FlightMode> _flight_mode{Telemetry::FlightMode::Unknown};

TEST_F(SitlTest, TelemetryFlightModes)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    System& system = dc.system();

    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);

    telemetry->subscribe_flight_mode(std::bind(&observe_mode, std::placeholders::_1));

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    EXPECT_EQ(action->arm(), Action::Result::Success);

    EXPECT_EQ(action->takeoff(), Action::Result::Success);

    EXPECT_TRUE(poll_condition_with_timeout(
        []() { return _flight_mode == Telemetry::FlightMode::Takeoff; }, std::chrono::seconds(20)));

    EXPECT_TRUE(poll_condition_with_timeout(
        []() { return _flight_mode == Telemetry::FlightMode::Hold; }, std::chrono::seconds(20)));

    EXPECT_EQ(action->land(), Action::Result::Success);

    EXPECT_TRUE(poll_condition_with_timeout(
        []() { return _flight_mode == Telemetry::FlightMode::Land; }, std::chrono::seconds(20)));

    EXPECT_TRUE(poll_condition_with_timeout(
        [&telemetry]() { return !telemetry->armed(); }, std::chrono::seconds(20)));
}

void observe_mode(Telemetry::FlightMode flight_mode)
{
    std::cout << "Got FlightMode: " << flight_mode << std::endl;
    _flight_mode = flight_mode;
}
