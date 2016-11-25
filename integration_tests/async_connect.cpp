#include <iostream>
#include <unistd.h>
#include "integration_test_helper.h"
#include "dronelink.h"

using namespace dronelink;
using namespace std::placeholders; // for _1

static bool _discovered_device = false;
static bool _timeouted_device = false;
static uint64_t _uuid = 0;

void on_discover(uint64_t uuid);
void on_timeout(uint64_t uuid);

TEST(Connect, Async)
{
    DroneLink dl;

    ASSERT_EQ(dl.add_udp_connection(), DroneLink::ConnectionResult::SUCCESS);

    dl.register_on_discover(std::bind(&on_discover, _1));
    dl.register_on_timeout(std::bind(&on_timeout, _1));

    sitl::start();

    while (!_discovered_device) {
        std::cout << "waiting for device to appear..." << std::endl;
        sleep(1);
    }

    // Let params stabilize before shutting it down.
    sleep(3);
    sitl::stop();

    while (!_timeouted_device) {
        std::cout << "waiting for device to disappear..." << std::endl;
        sleep(1);
    }
}

void on_discover(uint64_t uuid)
{
    std::cout << "Found device with UUID: " << uuid << std::endl;
    _discovered_device = true;
    _uuid = uuid;
    // The UUID should not be 0.
    EXPECT_NE(_uuid, 0);
}

void on_timeout(uint64_t uuid)
{
    std::cout << "Lost device with UUID: " << uuid << std::endl;
    _timeouted_device = true;

    // The UUID should still be the same.
    EXPECT_EQ(_uuid, uuid);
}
