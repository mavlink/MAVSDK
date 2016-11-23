#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

using namespace dronelink;

static void on_discover(uint64_t uuid);
static bool _discovered_device = false;

TEST(Info, Simple)
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);

    dl.register_on_discover(std::bind(&on_discover, std::placeholders::_1));

    while (!_discovered_device) {
        std::cout << "waiting for device to appear..." << std::endl;
        usleep(1000000);
    }

    for (unsigned i = 0; i < 10; ++i) {
        Info::Version version = dl.device().info().get_version();

        std::cout << "Flight version: "
                  << version.flight_sw_major << "."
                  << version.flight_sw_minor << "."
                  << version.flight_sw_patch << " ("
                  << std::string(version.flight_sw_git_hash) << ")" << std::endl;
        std::cout << "OS version: "
                  << version.os_sw_major << "."
                  << version.os_sw_minor << "."
                  << version.os_sw_patch << " ("
                  << std::string(version.os_sw_git_hash) << ")" << std::endl;
        usleep(1000000);
    }
}

void on_discover(uint64_t uuid)
{
    std::cout << "Found device with UUID: " << uuid << std::endl;
    _discovered_device = true;
}
