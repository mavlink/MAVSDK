#include <iostream>
#include "dronelink.h"
#include "integration_test_helper.h"

using namespace dronelink;

TEST_F(SitlTest, Logging)
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    Logging::Result log_ret = dl.device().logging().start_logging();

    if (log_ret == Logging::Result::COMMAND_DENIED) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        dl.device().logging().stop_logging();
        //ASSERT_EQ(log_ret, Logging::Result::SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        log_ret = dl.device().logging().start_logging();
    }

    ASSERT_EQ(log_ret, Logging::Result::SUCCESS);


    for (unsigned i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    log_ret = dl.device().logging().stop_logging();
    ASSERT_EQ(log_ret, Logging::Result::SUCCESS);
}
