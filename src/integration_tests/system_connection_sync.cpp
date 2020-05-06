#include <iostream>
#include "integration_test_helper.h"
#include "global_include.h"
#include "mavsdk.h"

using namespace mavsdk;

TEST(SitlTestMultiple, SystemConnectionMultiple)
{
    mavsdk::Mavsdk* dc;
    dc = new mavsdk::Mavsdk();
    std::cout << "started" << std::endl;

    ASSERT_EQ(dc->add_udp_connection(14540), ConnectionResult::Success);

    ASSERT_EQ(dc->add_udp_connection(14550), ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    std::vector<uint64_t> uuids = dc->system_uuids();

    for (auto it = uuids.begin(); it != uuids.end(); ++it) {
        std::cout << "found system with UUID: " << *it << std::endl;
    }

    ASSERT_EQ(uuids.size(), 1);
    uint64_t uuid = uuids[0];

    std::cout << "deleting it" << std::endl;
    delete dc;

    std::this_thread::sleep_for(std::chrono::seconds(1));

    dc = new mavsdk::Mavsdk();

    ASSERT_EQ(dc->add_udp_connection(14540), ConnectionResult::Success);

    std::this_thread::sleep_for(std::chrono::seconds(3));
    uuids = dc->system_uuids();

    ASSERT_EQ(uuids.size(), 1);
    EXPECT_EQ(uuid, uuids[0]);

    std::cout << "created and started yet again" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    delete dc;
    std::cout << "exiting" << std::endl;
}
