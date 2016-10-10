#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

int test_simple_hover()
{
    dronelink::DroneLink dl;

    dronelink::DroneLink::ConnectionResult ret = dl.add_udp_connection();
    if (ret != dronelink::DroneLink::ConnectionResult::SUCCESS) {
        std::cout << "failed to add connection." << std::endl;
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

    dronelink::Action::Result action_ret = dl.device(uuid).action().arm();
    if (action_ret != dronelink::Action::Result::SUCCESS) {
        std::cout << "failed to arm" <<  std::endl;
    }

    usleep(500000);

    action_ret = dl.device(uuid).action().takeoff();
    if (action_ret != dronelink::Action::Result::SUCCESS) {
        std::cout << "failed to takeoff" << std::endl;
    }

    usleep(5000000);

    action_ret = dl.device(uuid).action().land();
    if (action_ret != dronelink::Action::Result::SUCCESS) {
        std::cout << "failed to land" << std::endl;
    }

    // TODO: we need a check for is_landed() before calling disarm.
    //usleep(1000000);
    //action_ret = dl.device(1).disarm();
    //if (action_ret != dronelink::ConnectionResult::SUCCESS) {
    //    std::cout << "failed to disarm" << std::endl;
    //}

    return 0;
}

TEST(Hover, Simple)
{
    ASSERT_EQ(test_simple_hover(), 0);
}

