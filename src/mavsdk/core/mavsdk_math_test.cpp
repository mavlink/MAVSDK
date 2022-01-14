#include "mavsdk_math.h"
#include <gtest/gtest.h>
#include <cmath>

using namespace mavsdk;

#ifndef M_PI_F
#define M_PI_F static_cast<float>(M_PI)
#endif

TEST(MATH, OurPi)
{
    ASSERT_DOUBLE_EQ(PI, M_PI);
}

TEST(Math, RadDegDouble)
{
    ASSERT_DOUBLE_EQ(0.0, to_rad_from_deg(0.0));
    ASSERT_DOUBLE_EQ(M_PI / 2.0, to_rad_from_deg(90.0));
    ASSERT_DOUBLE_EQ(M_PI, to_rad_from_deg(180.0));
    ASSERT_DOUBLE_EQ(-M_PI, to_rad_from_deg(-180.0));
    ASSERT_DOUBLE_EQ(2.0 * M_PI, to_rad_from_deg(360.0));

    ASSERT_DOUBLE_EQ(0.0, to_deg_from_rad(0.0));
    ASSERT_DOUBLE_EQ(90.0, to_deg_from_rad(M_PI / 2.0));
    ASSERT_DOUBLE_EQ(180, to_deg_from_rad(M_PI));
    ASSERT_DOUBLE_EQ(-180, to_deg_from_rad(-M_PI));
    ASSERT_DOUBLE_EQ(360.0, to_deg_from_rad(2.0 * M_PI));
}

TEST(Math, RadDegFloat)
{
    ASSERT_FLOAT_EQ(0.0f, to_rad_from_deg(0.0f));
    ASSERT_FLOAT_EQ(M_PI_F / 2.0f, to_rad_from_deg(90.0f));
    ASSERT_FLOAT_EQ(M_PI_F, to_rad_from_deg(180.0f));
    ASSERT_FLOAT_EQ(-M_PI_F, to_rad_from_deg(-180.0f));
    ASSERT_FLOAT_EQ(2.0f * M_PI_F, to_rad_from_deg(360.0f));

    ASSERT_FLOAT_EQ(0.0f, to_deg_from_rad(0.0f));
    ASSERT_FLOAT_EQ(90.0f, to_deg_from_rad(M_PI_F / 2.0f));
    ASSERT_FLOAT_EQ(180.0f, to_deg_from_rad(M_PI_F));
    ASSERT_FLOAT_EQ(-180.0f, to_deg_from_rad(-M_PI_F));
    ASSERT_FLOAT_EQ(360.0f, to_deg_from_rad(2.0f * M_PI_F));
}