#include "mavsdk.h"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Mavsdk, version)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ASSERT_GT(mavsdk.version().size(), 5);
}
