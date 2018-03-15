#include <iostream>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"

using namespace std::placeholders; // for _1
using namespace dronecore;

static bool _discovered_system = false;
static uint64_t _uuid = 0;
static void on_discover(uint64_t uuid);

static bool _received_arm_result = false;
static bool _received_takeoff_result = false;
static bool _received_kill_result = false;

static void receive_arm_result(ActionResult result);
static void receive_takeoff_result(ActionResult result);
static void receive_kill_result(ActionResult result);

void receive_arm_result(ActionResult result)
{
    ASSERT_EQ(result, ActionResult::SUCCESS);
    _received_arm_result = true;
}

void receive_takeoff_result(ActionResult result)
{
    ASSERT_EQ(result, ActionResult::SUCCESS);
    _received_takeoff_result = true;
}

void receive_kill_result(ActionResult result)
{
    ASSERT_EQ(result, ActionResult::SUCCESS);
    _received_kill_result = true;
}


void on_discover(uint64_t uuid)
{
    std::cout << "Found system with UUID: " << uuid << std::endl;
    _uuid = uuid;
    _discovered_system = true;
}

TEST_F(SitlTest, ActionTakeoffAndKill)
{
    DroneCore dc;
    ASSERT_EQ(dc.add_udp_connection(), ConnectionResult::SUCCESS);

    dc.register_on_discover(std::bind(&on_discover, _1));
    std::this_thread::sleep_for(std::chrono::seconds(5));
    ASSERT_TRUE(_discovered_system);

    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    action->set_takeoff_altitude(0.5f);

    action->arm_async(std::bind(&receive_arm_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(_received_arm_result);

    action->takeoff_async(std::bind(&receive_takeoff_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(_received_takeoff_result);

    bool reached_alt = false;
    // Wait up to 2.0s (combined 3s) to reach 0.3m.
    for (unsigned i = 0; i < 1000; ++i) {
        if (telemetry->position().relative_altitude_m > 0.3f) {
            reached_alt = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    ASSERT_TRUE(reached_alt);

    // Kill it and hope it doesn't come down upside down, ready to fly again :)
    action->kill_async(std::bind(&receive_kill_result, _1));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ASSERT_TRUE(_received_kill_result);

    // It should be below 0.5m after having been killed
    ASSERT_FALSE(telemetry->armed());

    // The land detector takes some time.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_FALSE(telemetry->in_air());
}
