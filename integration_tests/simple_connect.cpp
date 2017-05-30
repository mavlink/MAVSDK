#include <iostream>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronelink.h"

using namespace dronelink;

TEST_F(SitlTest, TwoConnections)
{
    dronelink::DroneLink *dl;
    dl = new dronelink::DroneLink();
    std::cout << "started" << std::endl;

    ASSERT_EQ(dl->add_udp_connection(14540), DroneLink::ConnectionResult::SUCCESS);

    ASSERT_EQ(dl->add_udp_connection(14550), DroneLink::ConnectionResult::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::vector<uint64_t> uuids = dl->device_uuids();

    for (auto it = uuids.begin(); it != uuids.end(); ++it) {
        std::cout << "found device with UUID: " << *it << std::endl;
    }

    ASSERT_EQ(uuids.size(), 1);
    uint64_t uuid = uuids[0];

    std::cout << "deleting it" << std::endl;
    delete dl;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    dl = new dronelink::DroneLink();

    ASSERT_EQ(dl->add_udp_connection(14540), DroneLink::ConnectionResult::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    uuids = dl->device_uuids();

    ASSERT_EQ(uuids.size(), 1);
    EXPECT_EQ(uuid, uuids[0]);

    std::cout << "created and started yet again" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    delete dl;
    std::cout << "exiting" << std::endl;
}
