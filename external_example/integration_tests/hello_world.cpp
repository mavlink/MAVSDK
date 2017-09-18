#include <iostream>
#include <unistd.h>
#include "dronecore.h"
#include "integration_test_helper.h"

using namespace dronecore;

TEST_F(SitlTest, ExternalExampleHello)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, DroneCore::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // One vehicle should have connected.
    std::vector<uint8_t> system_ids = dc.device_system_ids();
    EXPECT_EQ(system_ids.size(), 1);

    // Apparently it can say hello.
    dc.device().example().say_hello();
}
