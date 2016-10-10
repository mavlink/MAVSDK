#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

int test_simple_connect()
{
    dronelink::DroneLink *dl;
    dl = new dronelink::DroneLink();
    std::cout << "started" << std::endl;

    dronelink::DroneLink::ConnectionResult ret = dl->add_udp_connection(14540, 0);
    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
    }

    ret = dl->add_udp_connection(14550, 0);
    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
    }

    usleep(2000000);

    std::vector<uint64_t> uuids = dl->device_uuids();

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

    std::cout << "deleting it" << std::endl;
    delete dl;

    usleep(1000000);

    dl = new dronelink::DroneLink();

    ret = dl->add_udp_connection(14540, 0);
    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection" << std::endl;
    }
    std::cout << "created and started yet again" << std::endl;
    usleep(1000000);
    delete dl;
    std::cout << "exiting" << std::endl;
    return 0;
}

TEST(Connect, Simple)
{
    ASSERT_EQ(test_simple_connect(), 0);
}

