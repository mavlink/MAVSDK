#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

using namespace dronelink;

int test_hello_world()
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    if (ret != DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection: " << int(ret) << std::endl;
        return -1;
    }

    // Wait for device to connect via heartbeat.
    usleep(1500000);

    std::vector<uint64_t> uuids = dl.device_uuids();

    for (auto it = uuids.begin(); it != uuids.end(); ++it) {
        std::cout << "found device with UUID: " << *it << std::endl;
    }

    if (uuids.size() > 1) {
        std::cout << "found more than one device, not sure which one to use." << std::endl;
        return -1;
    } else if (uuids.size() == 0) {
        std::cout << "no device found." << std::endl;
        return -1;
    }

    uint64_t uuid = uuids.at(0);

    dl.device(uuid).example().say_hello();

    return 0;
}

TEST(HelloWorld, SayHello)
{
    ASSERT_EQ(test_hello_world(), 0);
}
