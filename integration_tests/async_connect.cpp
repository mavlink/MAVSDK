#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"


bool _discovered_device = false;
bool _timeouted_device = false;

void on_discover(uint64_t uuid);
void on_timeout(uint64_t uuid);

int test_async_connect()
{
    using namespace std::placeholders; // for _1

    dronelink::DroneLink dl;

    dronelink::DroneLink::ConnectionResult ret = dl.add_udp_connection();
    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
        return -1;
    }

    dl.register_on_discover(std::bind(&on_discover, _1));
    dl.register_on_timeout(std::bind(&on_timeout, _1));

    while (!_discovered_device) {
        std::cout << "waiting for device to appear..." << std::endl;
        usleep(1000000);
    }

    //FIXME: commented out because we don't want to stop the simulator.
#if 0
    while (!_timeouted_device) {
        std::cout << "waiting for device to disappear..." << std::endl;
        usleep(1000000);
    }
#endif

    return 0;
}

void on_discover(uint64_t uuid)
{
    std::cout << "Found device with UUID: " << uuid << std::endl;
    _discovered_device = true;
}

void on_timeout(uint64_t uuid)
{
    std::cout << "Lost device with UUID: " << uuid << std::endl;
    _timeouted_device = true;
}

TEST(Connect, Async)
{
    ASSERT_EQ(test_async_connect(), 0);
}
