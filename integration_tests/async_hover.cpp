#include <iostream>
#include <unistd.h>
#include "integration_test_helper.h"
#include "dronelink.h"

using namespace dronelink;
using namespace std::placeholders; // for `_1`

void receive_result(Action::Result result);
void receive_health_all_ok(bool all_ok);
void receive_in_air(bool in_air);

static bool _all_ok = false;
static bool _in_air = false;

TEST_F(SitlTest, ActionAsyncHover)
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    sleep(2);

    // TODO: this test is pretty dumb, should be improved with more checks.
    Device &device = dl.device();

    device.telemetry().health_all_ok_async(std::bind(&receive_health_all_ok, _1));
    device.telemetry().in_air_async(std::bind(&receive_in_air, _1));

    while (!_all_ok) {
        std::cout << "Waiting to be ready..." << std::endl;
        sleep(1);
    }

    device.action().arm_async(std::bind(&receive_result, _1));
    sleep(2);

    device.action().set_takeoff_altitude(5.0f);

    device.action().takeoff_async(std::bind(&receive_result, _1));
    sleep(5);

    device.action().land_async(std::bind(&receive_result, _1));

    while (_in_air) {
        std::cout << "Waiting to be landed..." << std::endl;
        sleep(1);
    }

    device.action().disarm_async(std::bind(&receive_result, _1));
    sleep(2);
}


void receive_result(Action::Result result)
{
    std::cout << "got result: " << unsigned(result) << std::endl;
    EXPECT_EQ(result, Action::Result::SUCCESS);
}

void receive_health_all_ok(bool all_ok)
{
    if (all_ok && !_all_ok) {
        std::cout << "we're ready, let's go" << std::endl;
        _all_ok = true;
    }
}

void receive_in_air(bool in_air)
{
    _in_air = in_air;
}
