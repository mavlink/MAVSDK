/**
  @file Integration test: Handling Multiple components.
  @author Shakthi Prashanth <shakthi.prashanth.m@intel.com>
*/

#include <iostream>
#include "integration_test_helper.h"
#include "dronecore.h"
#include "system.h"

using namespace dronecore;
using namespace std::this_thread;
using namespace std::chrono;

/**
 * @brief Note: Steps to verify this test.
 * 0. Connect a V4L2 Camera on Ubuntu.
 * 1. Launch Camera Streaming Daemon on Ubuntu and configure UDP port 14550.
 * 2. Launch PX4 SITL.
 * 3. Run SitlTest.MultiComponentDiscovery test.
 *
 * Below test shall discover both Autopilot and Camera on the system.
 */
TEST_F(SitlTest, MultiComponentDiscovery)
{
    DroneCore dc;

    ASSERT_EQ(dc.add_udp_connection(), ConnectionResult::SUCCESS); // autopilot
    ASSERT_EQ(dc.add_udp_connection(14550), ConnectionResult::SUCCESS); // camera

    // FIXME: As components send Heartbeats at 1Hz,
    // lets wait until atleast 2 of them gets discovered.
    // Without this wait, we MAY NOT get Hearbeat from second component.
    std::cout << "We've nothing to do until we've some system to talk to. Lets sleep!\n";
    sleep_for(seconds(3));
    std::cout << "Done with sleeping\n";

    auto uuids = dc.system_uuids();

    for (auto uuid : uuids) {
        std::cout << "We found a system with UUID: " << uuid << '\n';
        System &system = dc.system(uuid);

        auto has_autopilot = system.has_autopilot();
        EXPECT_EQ(has_autopilot, true);

        auto has_camera = system.has_camera();
        EXPECT_EQ(has_camera, true);

        auto has_gimbal = system.has_gimbal();
        EXPECT_EQ(has_gimbal, false);

        std::cout << "System " << uuid << " has: ";
        if (has_autopilot) {
            std::cout << " Autopilot";
        }
        if (has_camera) {
            std::cout << " + Camera";
        }
        if (has_gimbal) {
            std::cout << " + Gimbal";
        }
        std::cout << std::endl;
    }

    std::cout << "Done" << '\n';
}
