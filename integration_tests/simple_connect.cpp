#include <iostream>
#include "integration_test_helper.h"
#include "global_include.h"
#include "dronecore.h"

using namespace dronecore;

TEST_F(SitlTest, TwoConnections)
{
    dronecore::DroneCore *dc;
    dc = new dronecore::DroneCore();
    std::cout << "started" << std::endl;

    ASSERT_EQ(dc->add_udp_connection(14540), DroneCore::ConnectionResult::SUCCESS);

    ASSERT_EQ(dc->add_udp_connection(14550), DroneCore::ConnectionResult::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::vector<uint8_t> system_ids = dc->device_system_ids();

    for (auto it = system_ids.begin(); it != system_ids.end(); ++it) {
        std::cout << "found device with system ID: " << (int)(*it) << std::endl;
    }

    ASSERT_EQ(system_ids.size(), 1);
    uint64_t system_id = system_ids[0];

    std::cout << "deleting it" << std::endl;
    delete dc;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    dc = new dronecore::DroneCore();

    ASSERT_EQ(dc->add_udp_connection(14540), DroneCore::ConnectionResult::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    system_ids = dc->device_system_ids();

    ASSERT_EQ(system_ids.size(), 1);
    EXPECT_EQ(system_id, system_ids[0]);

    std::cout << "created and started yet again" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    delete dc;
    std::cout << "exiting" << std::endl;
}
