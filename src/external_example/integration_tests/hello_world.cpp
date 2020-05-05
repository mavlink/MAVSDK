#include <iostream>
#include <unistd.h>
#include "mavsdk.h"
#include "plugins/example/example.h"
#include "integration_test_helper.h"

using namespace mavsdk;

TEST_F(SitlTest, ExampleHello)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_TRUE(dc.is_connected());

    System& system = dc.system();
    auto example = std::make_shared<Example>(system);

    // Apparently it can say hello.
    example->say_hello();
}
