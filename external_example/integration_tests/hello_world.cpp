#include <iostream>
#include <unistd.h>
#include "dronelink.h"
#include "integration_test_helper.h"

using namespace dronelink;

TEST(HelloWorld, SayHello)
{
    sitl::start();

    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // One vehicle should have connected.
    std::vector<uint64_t> uuids = dl.device_uuids();
    EXPECT_EQ(uuids.size(), 1);

    // Apparently it can say hello.
    dl.device().example().say_hello();

    sitl::stop();
}
