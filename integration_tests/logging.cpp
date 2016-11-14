#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

using namespace dronelink;

void test_logging()
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);

    usleep(2000000);

    Logging::Result log_ret = dl.device().logging().start_logging();
    ASSERT_EQ(log_ret, Logging::Result::SUCCESS);

    while (true) {
        usleep(1000000);
    }

    log_ret = dl.device().logging().stop_logging();
    ASSERT_EQ(log_ret, Logging::Result::SUCCESS);
}

TEST(Logging, StartStop)
{
    test_logging();
}

