#include <iostream>
#include <unistd.h>
#include "dronelink.h"
#include "integration_test_helper.h"

using namespace dronelink;

TEST_F(SitlTest, Logging)
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);

    sleep(2);

    Logging::Result log_ret = dl.device().logging().start_logging();

    if (log_ret == Logging::Result::COMMAND_DENIED) {
        usleep(10000);
        dl.device().logging().stop_logging();
        //ASSERT_EQ(log_ret, Logging::Result::SUCCESS);
        usleep(10000);
        log_ret = dl.device().logging().start_logging();
    }

    ASSERT_EQ(log_ret, Logging::Result::SUCCESS);


    for (unsigned i = 0; i < 10; ++i) {
        sleep(1);
    }

    log_ret = dl.device().logging().stop_logging();
    ASSERT_EQ(log_ret, Logging::Result::SUCCESS);
}
