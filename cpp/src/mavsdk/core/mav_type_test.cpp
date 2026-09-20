#include "mav_type.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace mavsdk;

TEST(MavType, ToString)
{
    EXPECT_EQ(to_string(MavType::Generic), "Generic");
    EXPECT_EQ(to_string(MavType::Gcs), "Gcs");
    EXPECT_EQ(to_string(MavType::Radio), "Radio");
    EXPECT_EQ(to_string(static_cast<MavType>(250)), "Unknown");
}

TEST(MavType, StreamOperator)
{
    std::ostringstream oss;
    oss << MavType::OnboardController;
    EXPECT_EQ(oss.str(), "OnboardController");
}
