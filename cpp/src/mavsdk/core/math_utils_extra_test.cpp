#include "math_utils.hpp"
#include <gtest/gtest.h>
#include <cmath>

using namespace mavsdk;

TEST(MathUtils, ConstrainInsideAndOutside)
{
    EXPECT_EQ(constrain(5, 0, 10), 5);
    EXPECT_EQ(constrain(-3, 0, 10), 0);
    EXPECT_EQ(constrain(15, 0, 10), 10);
    EXPECT_FLOAT_EQ(constrain(0.5f, 0.0f, 1.0f), 0.5f);
    EXPECT_FLOAT_EQ(constrain(-1.0f, 0.0f, 1.0f), 0.0f);
    EXPECT_FLOAT_EQ(constrain(2.0f, 0.0f, 1.0f), 1.0f);
}

TEST(MathUtils, DegRadIdentity)
{
    EXPECT_NEAR(to_deg_from_rad(to_rad_from_deg(0.0)), 0.0, 1e-12);
    EXPECT_NEAR(to_deg_from_rad(to_rad_from_deg(180.0)), 180.0, 1e-9);
    EXPECT_NEAR(to_deg_from_rad(to_rad_from_deg(-90.0)), -90.0, 1e-9);
    EXPECT_NEAR(to_rad_from_deg(180.0), PI, 1e-12);
    EXPECT_NEAR(to_deg_from_rad(PI), 180.0, 1e-12);

    EXPECT_NEAR(to_deg_from_rad(to_rad_from_deg(45.0f)), 45.0f, 1e-5f);
}

TEST(MathUtils, ConstrainUsedByEulerPitchClamp)
{
    // Document pitch asymptote path uses constrain to keep asinf domain valid.
    const float over = constrain(1.5f, -1.0f, 1.0f);
    const float under = constrain(-1.5f, -1.0f, 1.0f);
    EXPECT_FLOAT_EQ(over, 1.0f);
    EXPECT_FLOAT_EQ(under, -1.0f);
    EXPECT_FALSE(std::isnan(asinf(over)));
    EXPECT_FALSE(std::isnan(asinf(under)));
}
