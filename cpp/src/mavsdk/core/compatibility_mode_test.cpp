#include "compatibility_mode.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace mavsdk;

TEST(CompatibilityMode, StreamOperatorKnown)
{
    {
        std::ostringstream oss;
        oss << CompatibilityMode::Auto;
        EXPECT_EQ(oss.str(), "Auto");
    }
    {
        std::ostringstream oss;
        oss << CompatibilityMode::Pure;
        EXPECT_EQ(oss.str(), "Pure");
    }
    {
        std::ostringstream oss;
        oss << CompatibilityMode::Px4;
        EXPECT_EQ(oss.str(), "Px4");
    }
    {
        std::ostringstream oss;
        oss << CompatibilityMode::ArduPilot;
        EXPECT_EQ(oss.str(), "ArduPilot");
    }
}

TEST(CompatibilityMode, StreamOperatorUnknownFallback)
{
    std::ostringstream oss;
    oss << static_cast<CompatibilityMode>(99);
    EXPECT_EQ(oss.str(), "Unknown");
}
