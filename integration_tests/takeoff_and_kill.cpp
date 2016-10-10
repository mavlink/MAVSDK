#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

static bool _discovered_device = false;
static uint64_t _uuid = 0;
static void on_discover(uint64_t uuid);

static bool _received_arm_result = false;
static bool _received_takeoff_result = false;
static bool _received_kill_result = false;

static void receive_arm_result(dronelink::Action::Result result);
static void receive_takeoff_result(dronelink::Action::Result result);
static void receive_kill_result(dronelink::Action::Result result);

void receive_arm_result(dronelink::Action::Result result)
{
    ASSERT_EQ(result, dronelink::Action::Result::SUCCESS);
    _received_arm_result = true;
}

void receive_takeoff_result(dronelink::Action::Result result)
{
    ASSERT_EQ(result, dronelink::Action::Result::SUCCESS);
    _received_takeoff_result = true;
}

void receive_kill_result(dronelink::Action::Result result)
{
    ASSERT_EQ(result, dronelink::Action::Result::SUCCESS);
    _received_kill_result = true;
}


void on_discover(uint64_t uuid)
{
    std::cout << "Found device with UUID: " << uuid << std::endl;
    _uuid = uuid;
    _discovered_device = true;
}

TEST(Action, TakeoffAndKill)
{
    using namespace std::placeholders; // for _1

    dronelink::DroneLink dl;
    ASSERT_EQ(dl.add_udp_connection(), dronelink::DroneLink::ConnectionResult::SUCCESS);

    dl.register_on_discover(std::bind(&on_discover, _1));
    usleep(1500000);
    ASSERT_TRUE(_discovered_device);

    dl.device(_uuid).action().arm_async(std::bind(&receive_arm_result, _1));
    usleep(1000000);
    ASSERT_TRUE(_received_arm_result);

    dl.device(_uuid).action().takeoff_async(std::bind(&receive_takeoff_result, _1));
    usleep(500000);
    ASSERT_TRUE(_received_takeoff_result);

    bool reached_alt = false;
    // Wait up to 0.5s (combined 1s) to reach 0.3m.
    for (unsigned i = 0; i < 500; ++i) {

        if (dl.device(_uuid).telemetry().position().relative_altitude_m > 0.3f) {
            reached_alt = true;
            break;
        }
        usleep(1000);
    }
    ASSERT_TRUE(reached_alt);

    // Kill it and hope it doesn't come down upside down, ready to fly again :)
    dl.device(_uuid).action().kill_async(std::bind(&receive_kill_result, _1));
    usleep(1000000);
    ASSERT_TRUE(_received_kill_result);

    // It should be below 0.5m after having been killed
    ASSERT_FALSE(dl.device(_uuid).telemetry().armed());

    // The land detector takes some time.
    usleep(2000000);
    ASSERT_FALSE(dl.device(_uuid).telemetry().in_air());
}
