#include <iostream>
#include "integration_test_helper.h"
#include "mavsdk.h"

using namespace mavsdk;

// For now we don't test the timing out because the starting and stopping of
// PX4 SITL is not working as needed.
static constexpr bool ENABLE_TEARDOWN_TEST = false;

static bool _discovered_system = false;
static bool _timeouted_system = false;
static uint8_t _sysid = 0;

TEST(SitlTest, SystemConnectionAsync)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ASSERT_EQ(mavsdk.add_any_connection("udpin://0.0.0.0:14540"), ConnectionResult::Success);

    mavsdk.subscribe_on_new_system([&mavsdk]() {
        const auto system = mavsdk.systems().at(0);
        const auto sysid = system->get_system_id();

        if (system->is_connected()) {
            std::cout << "Found system with system ID: " << sysid << '\n';
            _discovered_system = true;
            _sysid = sysid;
            // The sysid should not be 0.
            EXPECT_NE(_sysid, 0);
        } else {
            std::cout << "Lost system with system ID: " << sysid << '\n';
            _timeouted_system = true;

            // The UUID should still be the same.
            EXPECT_EQ(_sysid, sysid);
        }
    });

    while (!_discovered_system) {
        std::cout << "waiting for system to appear..." << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Let params stabilize before shutting it down.
    std::this_thread::sleep_for(std::chrono::seconds(3));

    if (ENABLE_TEARDOWN_TEST) {
        // Call gtest to shut down SITL.
        TearDown();

        while (!_timeouted_system) {
            std::cout << "waiting for system to disappear..." << '\n';
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}
