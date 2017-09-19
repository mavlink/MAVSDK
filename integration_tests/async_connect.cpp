#include <iostream>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecore.h"

using namespace dronecore;
using namespace std::placeholders; // for _1

static bool _discovered_device = false;
static bool _timeouted_device = false;
static uint8_t _system_id = 0;

void on_discover(uint8_t system_id);
void on_timeout(uint8_t system_id);

TEST_F(SitlTest, AsyncConnect)
{
    DroneCore dc;

    ASSERT_EQ(dc.add_udp_connection(), DroneCore::ConnectionResult::SUCCESS);

    dc.register_on_discover(std::bind(&on_discover, _1));
    dc.register_on_timeout(std::bind(&on_timeout, _1));

    while (!_discovered_device) {
        std::cout << "waiting for device to appear..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Let params stabilize before shutting it down.
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Call gtest to shut down SITL.
    TearDown();

    while (!_timeouted_device) {
        std::cout << "waiting for device to disappear..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void on_discover(uint8_t system_id)
{
    std::cout << "Found device with system ID: " << (int)system_id << std::endl;
    _discovered_device = true;
    _system_id = system_id;
    // The system ID should not be 0.
    EXPECT_NE(_system_id, 0);
}

void on_timeout(uint8_t system_id)
{
    std::cout << "Lost device with system ID: " << (int)system_id << std::endl;
    _timeouted_device = true;

    // The system ID should still be the same.
    EXPECT_EQ(_system_id, system_id);
}
