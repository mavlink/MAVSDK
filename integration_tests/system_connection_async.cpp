#include <iostream>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecode_sdk.h"

using namespace dronecode_sdk;
using namespace std::placeholders; // for _1

// For now we don't test the timing out because the starting and stopping of
// PX4 SITL is not working as needed.
static constexpr bool ENABLE_TEARDOWN_TEST = false;

static bool _discovered_system = false;
static bool _timeouted_system = false;
static uint64_t _uuid = 0;

void on_discover(uint64_t uuid);
void on_timeout(uint64_t uuid);

TEST_F(SitlTest, SystemConnectionAsync)
{
    DronecodeSDK dc;

    ASSERT_EQ(dc.add_udp_connection(), ConnectionResult::SUCCESS);

    dc.register_on_discover(std::bind(&on_discover, _1));
    dc.register_on_timeout(std::bind(&on_timeout, _1));

    while (!_discovered_system) {
        std::cout << "waiting for system to appear..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Let params stabilize before shutting it down.
    std::this_thread::sleep_for(std::chrono::seconds(3));

    if (ENABLE_TEARDOWN_TEST) {
        // Call gtest to shut down SITL.
        TearDown();

        while (!_timeouted_system) {
            std::cout << "waiting for system to disappear..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void on_discover(uint64_t uuid)
{
    std::cout << "Found system with UUID: " << uuid << std::endl;
    _discovered_system = true;
    _uuid = uuid;
    // The UUID should not be 0.
    EXPECT_NE(_uuid, 0);
}

void on_timeout(uint64_t uuid)
{
    std::cout << "Lost system with UUID: " << uuid << std::endl;
    _timeouted_system = true;

    // The UUID should still be the same.
    EXPECT_EQ(_uuid, uuid);
}
