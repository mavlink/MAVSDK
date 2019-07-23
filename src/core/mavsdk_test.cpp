#include "mavsdk.h"
#include <gtest/gtest.h>

using namespace mavsdk;

TEST(Mavsdk, version)
{
    Mavsdk mavsdk;
    ASSERT_GT(mavsdk.version().size(), 5);
}
