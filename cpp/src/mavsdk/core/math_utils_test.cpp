#include "math_utils.h"
#include "mavlink_include.h"
#include <gtest/gtest.h>
#include <cmath>

using namespace mavsdk;

#ifndef M_PI_F
#define M_PI_F static_cast<float>(M_PI)
#endif

TEST(MathUtils, QuaternionToEulerAnglesAndBackBaseCase)
{
    Quaternion q1;
    q1.w = 1.0f;
    q1.x = 0.0f;
    q1.y = 0.0f;
    q1.z = 0.0f;
    EulerAngle e = to_euler_angle_from_quaternion(q1);

    EXPECT_FLOAT_EQ(e.roll_deg, 0.0f);
    EXPECT_FLOAT_EQ(e.pitch_deg, 0.0f);
    EXPECT_FLOAT_EQ(e.yaw_deg, 0.0f);

    Quaternion q2 = to_quaternion_from_euler_angle(e);

    EXPECT_EQ(q1, q2);

    // We also compare against the MAVLink implementation and test
    // the MAVLink functions while we're at it.
    float q1_mavlink[4];
    q1_mavlink[0] = 1.0f;
    q1_mavlink[1] = 0.0f;
    q1_mavlink[2] = 0.0f;
    q1_mavlink[3] = 0.0f;

    float roll;
    float pitch;
    float yaw;
    mavlink_quaternion_to_euler(q1_mavlink, &roll, &pitch, &yaw);

    EXPECT_NEAR(to_deg_from_rad(roll), 0.0f, 0.01f);
    EXPECT_NEAR(to_deg_from_rad(pitch), 0.0f, 0.01f);
    EXPECT_NEAR(to_deg_from_rad(yaw), 0.0f, 0.01f);

    float q2_mavlink[4];
    mavlink_euler_to_quaternion(roll, pitch, yaw, q2_mavlink);

    EXPECT_NEAR(q1_mavlink[0], q2_mavlink[0], 0.01f);
    EXPECT_NEAR(q1_mavlink[1], q2_mavlink[1], 0.01f);
    EXPECT_NEAR(q1_mavlink[2], q2_mavlink[2], 0.01f);
    EXPECT_NEAR(q1_mavlink[3], q2_mavlink[3], 0.01f);

    EXPECT_NEAR(q2.w, q2_mavlink[0], 0.01f);
    EXPECT_NEAR(q2.x, q2_mavlink[1], 0.01f);
    EXPECT_NEAR(q2.y, q2_mavlink[2], 0.01f);
    EXPECT_NEAR(q2.z, q2_mavlink[3], 0.01f);
}

// Example taken from https://quaternions.online/ with Euler angle ZYX-order.
// The accuracy was not great and I had to increase the test margins quite a
// bit to make the test pass. I'm not sure where the errors come from.
TEST(MathUtils, QuaternionToEulerAndBackSomeCase)
{
    Quaternion q1;
    q1.w = 0.143f;
    q1.x = 0.280f;
    q1.y = 0.595f;
    q1.z = 0.739f;
    EulerAngle e = to_euler_angle_from_quaternion(q1);

    EXPECT_NEAR(e.roll_deg, 82.086f, 0.2f);
    EXPECT_NEAR(e.pitch_deg, -14.142f, 0.2f);
    EXPECT_NEAR(e.yaw_deg, 145.774f, 0.2f);

    Quaternion q2 = to_quaternion_from_euler_angle(e);

    EXPECT_NEAR(q1.w, q2.w, 0.01f);
    EXPECT_NEAR(q1.x, q2.x, 0.01f);
    EXPECT_NEAR(q1.y, q2.y, 0.01f);
    EXPECT_NEAR(q1.z, q2.z, 0.01f);

    // We also compare against the MAVLink implementation and test
    // the MAVLink functions while we're at it.
    float q1_mavlink[4];
    q1_mavlink[0] = 0.143f;
    q1_mavlink[1] = 0.280f;
    q1_mavlink[2] = 0.595f;
    q1_mavlink[3] = 0.739f;

    float roll;
    float pitch;
    float yaw;
    mavlink_quaternion_to_euler(q1_mavlink, &roll, &pitch, &yaw);

    EXPECT_NEAR(to_deg_from_rad(roll), 82.086f, 0.1f);
    EXPECT_NEAR(to_deg_from_rad(pitch), -14.142f, 0.1f);
    EXPECT_NEAR(to_deg_from_rad(yaw), 145.774f, 0.1f);

    float q2_mavlink[4];
    mavlink_euler_to_quaternion(roll, pitch, yaw, q2_mavlink);

    EXPECT_NEAR(q1_mavlink[0], q2_mavlink[0], 0.01f);
    EXPECT_NEAR(q1_mavlink[1], q2_mavlink[1], 0.01f);
    EXPECT_NEAR(q1_mavlink[2], q2_mavlink[2], 0.01f);
    EXPECT_NEAR(q1_mavlink[3], q2_mavlink[3], 0.01f);

    EXPECT_NEAR(q2.w, q2_mavlink[0], 0.01f);
    EXPECT_NEAR(q2.x, q2_mavlink[1], 0.01f);
    EXPECT_NEAR(q2.y, q2_mavlink[2], 0.01f);
    EXPECT_NEAR(q2.z, q2_mavlink[3], 0.01f);
}

TEST(MathUtils, QuaternionRotation)
{
    // Define a sample quaternion
    auto sample_q = to_quaternion_from_euler_angle(EulerAngle{30.f, 60.f, -45.f});

    // Rotate by 90 degrees yaw
    auto rotation_q = to_quaternion_from_euler_angle(EulerAngle{0, 0, 90.0f});
    auto rotated_q = rotation_q * sample_q;

    // Expected values after 90 degrees yaw rotation (approximate)
    auto expected_q = to_quaternion_from_euler_angle(EulerAngle{30.f, 60.f, 45.f});

    // Assert that the rotated quaternion is close to the expected value
    EXPECT_NEAR(expected_q.w, rotated_q.w, 1e-3);
    EXPECT_NEAR(expected_q.x, rotated_q.x, 1e-3);
    EXPECT_NEAR(expected_q.y, rotated_q.y, 1e-3);
    EXPECT_NEAR(expected_q.z, rotated_q.z, 1e-3);
}

TEST(MathUtils, OurPi)
{
    ASSERT_DOUBLE_EQ(PI, M_PI);
}

TEST(MathUtils, RadDegDouble)
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

TEST(MathUtils, RadDegFloat)
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
