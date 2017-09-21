#include <iostream>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecore.h"

using namespace dronecore;
using namespace std::placeholders; // for `_1`

void receive_result(Action::Result result);
void receive_health_all_ok(bool all_ok);
void receive_in_air(bool in_air);

static bool _all_ok = false;
static bool _in_air = false;

TEST_F(SitlTest, ActionAsyncHover)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, DroneCore::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // TODO: this test is pretty dumb, should be improved with more checks.
    Device &device = dc.device();

    device.telemetry().health_all_ok_async(std::bind(&receive_health_all_ok, _1));
    device.telemetry().in_air_async(std::bind(&receive_in_air, _1));

    while (!_all_ok) {
        LogInfo() << "Waiting to be ready...";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    LogInfo() << "Arming...";
    device.action().arm_async(std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(2));

    LogInfo() << "Setting takeoff altitude...";
    device.action().set_takeoff_altitude(5.0f);

    LogInfo() << "Taking off...";
    device.action().takeoff_async(std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(5));

    LogInfo() << "Landing...";
    device.action().land_async(std::bind(&receive_result, _1));

    while (_in_air) {
        LogInfo() << "Waiting to be landed...";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    device.action().disarm_async(std::bind(&receive_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(2));
}


void receive_result(Action::Result result)
{
    LogDebug() << "got result: " << unsigned(result);
    EXPECT_EQ(result, Action::Result::SUCCESS);
}

void receive_health_all_ok(bool all_ok)
{
    if (all_ok && !_all_ok) {
        LogDebug() << "we're ready, let's go";
        _all_ok = true;
    }
}

void receive_in_air(bool in_air)
{
    _in_air = in_air;
}
