#include <iostream>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "plugins/info/info.h"

using namespace dronecore;

static void on_discover(uint64_t uuid);
static bool _discovered_system = false;

TEST_F(SitlTest, Info)
{
    DroneCore dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    dc.register_on_discover(std::bind(&on_discover, std::placeholders::_1));

    while (!_discovered_system) {
        std::cout << "waiting for system to appear..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


    System &system = dc.system();
    auto info = std::make_shared<Info>(system);

    for (unsigned i = 0; i < 3; ++i) {
        Info::Version version = info->get_version();

        std::cout << "Flight version: "
                  << version.flight_sw_major << "."
                  << version.flight_sw_minor << "."
                  << version.flight_sw_patch << " ("
                  << std::string(version.flight_sw_git_hash) << ")" << std::endl;
        std::cout << "Flight vendor version: "
                  << version.flight_sw_vendor_major << "."
                  << version.flight_sw_vendor_minor << "."
                  << version.flight_sw_vendor_patch << std::endl;
        std::cout << "OS version: "
                  << version.os_sw_major << "."
                  << version.os_sw_minor << "."
                  << version.os_sw_patch << " ("
                  << std::string(version.os_sw_git_hash) << ")" << std::endl;

        EXPECT_NE(version.flight_sw_major, 0);

        // FIXME: This is currently 0.
        //EXPECT_NE(version.os_sw_major, 0);


        Info::Product product = info->get_product();

        std::cout << "Vendor: " << product.vendor_name << std::endl;
        std::cout << "Product: " << product.product_name << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void on_discover(uint64_t uuid)
{
    std::cout << "Found system with UUID: " << uuid << std::endl;
    _discovered_system = true;
}
