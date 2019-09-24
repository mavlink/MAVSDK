#include "autopilot_tester.h"
#include <iostream>

using namespace mavsdk;

#define EXPECT_EQ_OR_FAIL(right_, left_) \
    EXPECT_EQ((right_), (left_)); \
    if ((right_) != (left_)) { \
        return false; \
    }

#define EXPECT_TRUE_OR_FAIL(cond_) \
    EXPECT_TRUE((cond_)); \
    if (!(cond_)) { \
        return false; \
    }

bool AutopilotTester::connect(const std::string uri)
{
    ConnectionResult ret = _mavsdk.add_any_connection(uri);
    EXPECT_EQ_OR_FAIL(ret, ConnectionResult::SUCCESS);

    std::cout << "Waiting for system connect" << std::endl;
    EXPECT_TRUE_OR_FAIL(poll_condition_with_timeout(
        [this]() { return _mavsdk.is_connected(); }, std::chrono::seconds(10)));

    auto& system = _mavsdk.system();

    _telemetry.reset(new Telemetry(system));
    _action.reset(new Action(system));
    return true;
}

bool AutopilotTester::wait_until_ready()
{
    std::cout << "Waiting for system to be ready" << std::endl;
    EXPECT_TRUE(poll_condition_with_timeout(
        [this]() { return _telemetry->health_all_ok(); }, std::chrono::seconds(10)));
    return true;
}

bool AutopilotTester::set_takeoff_altitude(const float altitude_m)
{
    EXPECT_EQ(Action::Result::SUCCESS, _action->set_takeoff_altitude(altitude_m));
    const auto result = _action->get_takeoff_altitude();
    EXPECT_EQ(result.first, Action::Result::SUCCESS);
    EXPECT_FLOAT_EQ(result.second, altitude_m);
    return true;
}

bool AutopilotTester::arm()
{
    const auto result = _action->arm();
    EXPECT_EQ(result, Action::Result::SUCCESS);
    return true;
}

bool AutopilotTester::takeoff()
{
    const auto result = _action->takeoff();
    EXPECT_EQ(result, Action::Result::SUCCESS);
    return true;
}

bool AutopilotTester::land()
{
    const auto result = _action->land();
    EXPECT_EQ(result, Action::Result::SUCCESS);
    return true;
}

bool AutopilotTester::wait_until_disarmed()
{
    EXPECT_TRUE(poll_condition_with_timeout(
        [this]() { return !_telemetry->in_air(); }, std::chrono::seconds(10)));
    return true;
}
