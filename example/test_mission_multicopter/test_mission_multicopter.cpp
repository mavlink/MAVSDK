//
// Multicopter mission test.
//
// Author: Julian Oes <julian@oes.ch>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/action/action.h>
#include <mavsdk/plugins/telemetry/telemetry.h>
#include <iostream>
#include <string>
#include "gtest/gtest.h"
#include "autopilot_tester.h"

using namespace mavsdk;

static std::string connection_url{"udp://"};

static void usage(const std::string& bin_name);

int main(int argc, char** argv)
{
    for (int i = 0; i < argc; ++i) {
        const std::string argv_string(argv[i]);

        if (argv_string == "-h") {
            usage(argv[0]);
        }

        const std::string url_prefix = "--url=";
        if (!argv_string.compare(0, url_prefix.size(), url_prefix)) {
            connection_url = argv_string.substr(url_prefix.size());
        }
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

void usage(const std::string& bin_name)
{
    std::cout << std::endl
              << "Usage : " << bin_name << " [--url=CONNECTION_URL] [gtest arguments]" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl
              << std::endl;
}

TEST(AutopilotTest, TakeoffAndLand)
{
    AutopilotTester tester;
    ASSERT_TRUE(tester.connect(connection_url));
    ASSERT_TRUE(tester.wait_until_ready());
    ASSERT_TRUE(tester.arm());
    ASSERT_TRUE(tester.takeoff());
    // TODO: Replace sleep with check.
    std::this_thread::sleep_for(std::chrono::seconds(5));
    ASSERT_TRUE(tester.land());
    ASSERT_TRUE(tester.wait_until_disarmed());
}
