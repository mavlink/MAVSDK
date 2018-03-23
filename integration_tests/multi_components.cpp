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

    // For both Autopilot and Camera
    ASSERT_EQ(dc.add_udp_connection(), ConnectionResult::SUCCESS);

    // FIXME: As components send Heartbeats at 1Hz,
    // lets wait until atleast 2 of them gets discovered.
    // Without this wait, we MAY NOT get Hearbeat from second component.
    std::cout << "We've nothing to do until we've some system to talk to. Lets sleep!\n";
    sleep_for(seconds(5));
    std::cout << "Done with sleeping\n";

    auto uuids = dc.system_uuids();

    for (auto uuid : uuids) {
        std::cout << "We found a System with UUID: " << uuid << '\n';
        System &system = dc.system(uuid);

        auto is_autopilot = system.is_autopilot();

        auto is_standalone = system.is_standalone();

        auto has_camera = system.has_camera(); // by default checks for camera 1
        if (has_camera) {
            has_camera = system.has_camera(1); // pass camera ID explcitly
            EXPECT_EQ(has_camera, true);
        }

        auto has_gimbal = system.has_gimbal();

        if (is_autopilot && has_camera && !has_gimbal) {
            std::cout << "Its an Autopilot with a Camera." << '\n';
        } else if (is_standalone && has_camera) {
            std::cout << "We found a standalone camera." << '\n';
        } else if (is_autopilot && !has_camera) {
            std::cout << "We found an Autopilot alone." << '\n';
        }
    }

    std::cout << "Done" << '\n';
}
