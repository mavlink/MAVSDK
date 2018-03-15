#include <iostream>
#include "dronecore.h"
#include "integration_test_helper.h"
#include "plugins/logging/logging.h"

using namespace dronecore;

TEST_F(SitlTest, Logging)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();
    auto logging = std::make_shared<Logging>(system);
    Logging::Result log_ret = logging->start_logging();

    if (log_ret == Logging::Result::COMMAND_DENIED) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        logging->stop_logging();
        //ASSERT_EQ(log_ret, Logging::Result::SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        log_ret = logging->start_logging();
    }

    ASSERT_EQ(log_ret, Logging::Result::SUCCESS);


    for (unsigned i = 0; i < 10; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    log_ret = logging->stop_logging();
    ASSERT_EQ(log_ret, Logging::Result::SUCCESS);
}
