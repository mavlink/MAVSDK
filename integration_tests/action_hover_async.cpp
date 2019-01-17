#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecode_sdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

#include <atomic>

using namespace dronecode_sdk;
using namespace std::placeholders; // for `_1`

void receive_result(Action::Result result);
void receive_health_all_ok(bool all_ok);
void receive_in_air(bool in_air);

static std::atomic<bool> _all_ok{false};
static std::atomic<bool> _in_air{false};

TEST_F(SitlTest, ActionHoverAsync)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // TODO: this test is pretty dumb, should be improved with more checks.
    System &system = dc.system();

    auto telemetry = std::make_shared<Telemetry>(system);
    telemetry->health_all_ok_async(std::bind(&receive_health_all_ok, _1));
    telemetry->in_air_async(std::bind(&receive_in_air, _1));

    while (!_all_ok) {
        LogDebug() << "Waiting to be ready...";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    auto action = std::make_shared<Action>(system);
    LogDebug() << "Arming";
    action->arm_async(std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(2));

    LogDebug() << "Setting takeoff altitude";
    action->set_takeoff_altitude(5.0f);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    LogDebug() << "Taking off";
    action->takeoff_async(std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(5));

    LogDebug() << "Landing off";
    action->land_async(std::bind(&receive_result, _1));

    while (_in_air) {
        LogDebug() << "Waiting to be landed...";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    action->disarm_async(std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void receive_result(Action::Result result)
{
    if (result != Action::Result::SUCCESS) {
        LogWarn() << "Result not successful: " << Action::result_str(result);
    }
    EXPECT_EQ(result, Action::Result::SUCCESS);
}

void receive_health_all_ok(bool all_ok)
{
    if (all_ok && !_all_ok) {
        LogDebug() << "Ready, let's go.";
        _all_ok = true;
    }
}

void receive_in_air(bool in_air)
{
    _in_air = in_air;
}
