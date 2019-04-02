#include <iostream>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/info/info.h"

using namespace dronecode_sdk;


TEST_F(SitlTest, Info)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_TRUE(dc.is_connected());

    System &system = dc.system();
    auto info = std::make_shared<Info>(system);

    // FIXME: we need to wait some time until Info has determined the version.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    for (unsigned i = 0; i < 3; ++i) {
        std::pair<Info::Result, Info::Version> version_result = info->get_version();

        EXPECT_EQ(version_result.first, Info::Result::SUCCESS);

        if (version_result.first == Info::Result::SUCCESS) {
            std::cout << "Flight version: " << version_result.second.flight_sw_major << "."
                      << version_result.second.flight_sw_minor << "."
                      << version_result.second.flight_sw_patch << " ("
                      << std::string(version_result.second.flight_sw_git_hash) << ")" << std::endl;
            std::cout << "Flight vendor version: " << version_result.second.flight_sw_vendor_major
                      << "." << version_result.second.flight_sw_vendor_minor << "."
                      << version_result.second.flight_sw_vendor_patch << std::endl;
            std::cout << "OS version: " << version_result.second.os_sw_major << "."
                      << version_result.second.os_sw_minor << "."
                      << version_result.second.os_sw_patch << " ("
                      << std::string(version_result.second.os_sw_git_hash) << ")" << std::endl;

            EXPECT_NE(version_result.second.flight_sw_major, 0);

            // FIXME: This is currently 0.
            // EXPECT_NE(version_result.second.os_sw_major, 0);

        } else {
            LogWarn() << "Version request result: " << Info::result_str(version_result.first);
        }

        std::pair<Info::Result, Info::Product> product_result = info->get_product();
        EXPECT_EQ(product_result.first, Info::Result::SUCCESS);

        if (product_result.first == Info::Result::SUCCESS) {
            std::cout << "Vendor: " << product_result.second.vendor_name << std::endl;
            std::cout << "Product: " << product_result.second.product_name << std::endl;
        } else {
            LogWarn() << "Product request result: " << Info::result_str(product_result.first);
        }

        std::pair<Info::Result, Info::Identification> identification_result =
            info->get_identification();

        EXPECT_EQ(identification_result.first, Info::Result::SUCCESS);

        if (identification_result.first == Info::Result::SUCCESS) {
            std::cout << "Hardware UID: " << identification_result.second.hardware_uid;
            for (unsigned j = 0; j < sizeof(identification_result.second.hardware_uid); ++j) {
                std::cout << std::hex << std::setfill('0') << std::setw(2)
                          << int(identification_result.second.hardware_uid[j]);
            }
            std::cout << std::endl;
        } else {
            LogWarn() << "Identification request result: "
                      << Info::result_str(identification_result.first);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
