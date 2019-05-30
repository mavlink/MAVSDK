/**
  @file Integration test: Handling Multiple components.
  @author Shakthi Prashanth <shakthi.prashanth.m@intel.com>
*/

#include <iostream>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "system.h"

using namespace dronecode_sdk;
using namespace std::this_thread;
using namespace std::chrono;

/**
 * @brief This test detects ALL the components found across systems and tells
 * whether each system is a standalone or the one with a system with componets
 * such as autopilot with camera(s), etc. One way to run this test is as below:
 * /////////////////////////////////////
 * 1. Connect a V4L2 Camera on Ubuntu.
 * 2. Launch Camera Streaming Daemon on Ubuntu and configure UDP port 14550.
 * 3. Launch PX4 SITL.
 * 4. Run SitlTestMultiple.SystemMultipleComponents test.
 * /////////////////////////////////////
 */
TEST(SitlTestMultiple, SystemMultipleComponents)
{
    DronecodeSDK dc;

    // For both Autopilot and Camera
    ASSERT_EQ(dc.add_udp_connection(), ConnectionResult::SUCCESS);

    // FIXME: As components send Heartbeats at 1Hz,
    // lets wait until atleast 2 of them gets discovered.
    // Without this wait, we MAY NOT get Hearbeat from second component.
    std::cout << "We've nothing to do until we've some system to talk to. Lets sleep!\n";
    sleep_for(seconds(3));

    auto uuids = dc.system_uuids();

    for (auto uuid : uuids) {
        std::cout << "We found a System with UUID: " << uuid << '\n';
        System &system = dc.system(uuid);

        auto has_autopilot = system.has_autopilot();
        auto is_standalone = system.is_standalone();
        auto has_camera = system.has_camera(); // Checks whether the system has any camera
        auto has_gimbal = system.has_gimbal();

        if (has_autopilot && has_camera && !has_gimbal) {
            std::cout << "Its an autopilot with camera(s)." << '\n';
        } else if (is_standalone && has_camera) {
            std::cout << "Its a standalone camera." << '\n';
        } else if (has_autopilot && !has_camera) {
            std::cout << "Its an autopilot alone." << '\n';
        }
    }
}
