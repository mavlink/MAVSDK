#include <iostream>
#include <cmath>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

using namespace dronelink;

int test_offboard_velocity()
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    if (ret != DroneLink::ConnectionResult::SUCCESS) {
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

    Action::Result action_ret = dl.device(uuid).action().arm();
    if (action_ret != Action::Result::SUCCESS) {
        std::cout << "failed to arm" <<  std::endl;
    }

    usleep(500000);

    action_ret = dl.device(uuid).action().takeoff();
    if (action_ret != Action::Result::SUCCESS) {
        std::cout << "failed to takeoff" << std::endl;
    }

    usleep(5000000);

    // Send it once before starting offboard, otherwise it will be rejected.
    dl.device(uuid).offboard().set_velocity({0.0f, 0.0f, 0.0f, 0.0f});

    Offboard::Result offboard_result = dl.device(uuid).offboard().start();

    if (offboard_result != Offboard::Result::SUCCESS) {
        std::cout << "failed to start offboard" << std::endl;
        return 1;
    }

    for (unsigned i = 0; i < 1000; ++i) {
        float vx = 5.0f * sinf(i * 0.01f);
        std::cout << "vx: " << vx << std::endl;
        dl.device(uuid).offboard().set_velocity({vx, 0.0f, 0.0f, 90.0f});
        usleep(10000);
    }

    offboard_result = dl.device(uuid).offboard().stop();

    if (offboard_result != Offboard::Result::SUCCESS) {
        std::cout << "failed to stop offboard" << std::endl;
        return 1;
    }

    action_ret = dl.device(uuid).action().land();
    if (action_ret != Action::Result::SUCCESS) {
        std::cout << "failed to land" << std::endl;
    }

    return 0;
}

TEST(Offboard, Velocity)
{
    ASSERT_EQ(test_offboard_velocity(), 0);
}

