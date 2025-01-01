#include "mavsdk.h"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Mavsdk, version)
{
    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ASSERT_GT(mavsdk.version().size(), 5);
}

TEST(Mavsdk, Configuration)
{
    Mavsdk::Configuration configuration{ComponentType::Autopilot};

    ASSERT_EQ(configuration.get_mav_type(), MAV_TYPE::MAV_TYPE_GENERIC); // Default
    configuration.set_mav_type(MAV_TYPE::MAV_TYPE_FIXED_WING);
    ASSERT_EQ(configuration.get_mav_type(), MAV_TYPE::MAV_TYPE_FIXED_WING);
}