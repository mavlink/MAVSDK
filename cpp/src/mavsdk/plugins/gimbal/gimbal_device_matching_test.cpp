#include "gimbal_device_matching.hpp"
#include <gtest/gtest.h>

using namespace mavsdk;

// Component IDs from MAV_COMPONENT for readability.
static constexpr uint8_t comp_autopilot = 1;
static constexpr uint8_t comp_gimbal = 154;
static constexpr uint8_t comp_gimbal2 = 171;

// Same component acts as manager and device (e.g. a smart gimbal). The manager advertises a
// non-MAVLink device id in 1-6, and the device messages carry that same id.
TEST(GimbalDeviceMatching, SameComponentMatchesOnDeviceId)
{
    // Manager compid 154 advertises device id 1; attitude status comes from 154 with id 1.
    EXPECT_TRUE(gimbal_device_message_matches(comp_gimbal, 1, comp_gimbal, 1));

    // Wrong device id from the same component does not match.
    EXPECT_FALSE(gimbal_device_message_matches(comp_gimbal, 1, comp_gimbal, 2));

    // Right device id but from a different component does not match.
    EXPECT_FALSE(gimbal_device_message_matches(comp_gimbal, 1, comp_gimbal2, 1));
}

// Separate device component: the manager (e.g. the autopilot) advertises the device's own
// component ID, and the device streams messages with gimbal_device_id 0.
TEST(GimbalDeviceMatching, SeparateComponentMatchesOnCompid)
{
    // Autopilot manager advertises device compid 154; device streams from 154 with id 0.
    EXPECT_TRUE(gimbal_device_message_matches(comp_autopilot, comp_gimbal, comp_gimbal, 0));

    // A message from a different device component (id 0) must not be attributed to this gimbal.
    EXPECT_FALSE(gimbal_device_message_matches(comp_autopilot, comp_gimbal, comp_gimbal2, 0));
}

// Two managers each with their own gimbal device must stay distinct.
TEST(GimbalDeviceMatching, DistinctGimbalsDoNotCrossMatch)
{
    // Same-component gimbal 1 vs a device message that belongs to a separate-component gimbal.
    EXPECT_FALSE(gimbal_device_message_matches(comp_autopilot, comp_gimbal, comp_gimbal, 1));

    // Separate-component gimbal (advertising compid 171) vs a same-component device message.
    EXPECT_FALSE(gimbal_device_message_matches(comp_autopilot, comp_gimbal2, comp_gimbal2, 1));
}
