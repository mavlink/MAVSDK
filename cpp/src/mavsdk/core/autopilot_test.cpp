#include "autopilot.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace mavsdk;

TEST(Autopilot, StreamOperator)
{
    {
        std::ostringstream oss;
        oss << Autopilot::Px4;
        EXPECT_EQ(oss.str(), "Px4");
    }
    {
        std::ostringstream oss;
        oss << Autopilot::ArduPilot;
        EXPECT_EQ(oss.str(), "ArduPilot");
    }
    {
        std::ostringstream oss;
        oss << Autopilot::Unknown;
        EXPECT_EQ(oss.str(), "Unknown");
    }
    {
        std::ostringstream oss;
        oss << static_cast<Autopilot>(99);
        EXPECT_EQ(oss.str(), "Unknown");
    }
}
