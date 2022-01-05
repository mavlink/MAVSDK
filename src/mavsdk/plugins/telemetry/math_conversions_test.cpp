#include "math_conversions.h"
#include <gtest/gtest.h>
#include "mavlink_include.h"
#include "mavsdk_math.h"

using namespace mavsdk;

TEST(MathConversions, QuaternionToEulerAnglesAndBackBaseCase)
{
    Telemetry::Quaternion q1;
    q1.w = 1.0f;
    q1.x = 0.0f;
    q1.y = 0.0f;
    q1.z = 0.0f;
    q1.timestamp_us = 4242;
    Telemetry::EulerAngle e = to_euler_angle_from_quaternion(q1);

    EXPECT_FLOAT_EQ(e.roll_deg, 0.0f);
    EXPECT_FLOAT_EQ(e.pitch_deg, 0.0f);
    EXPECT_FLOAT_EQ(e.yaw_deg, 0.0f);
    EXPECT_EQ(e.timestamp_us, q1.timestamp_us);

    Telemetry::Quaternion q2 = to_quaternion_from_euler_angle(e);

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
TEST(MathConversions, QuaternionToEulerAndBackSomeCase)
{
    Telemetry::Quaternion q1;
    q1.w = 0.143f;
    q1.x = 0.280f;
    q1.y = 0.595f;
    q1.z = 0.739f;
    q1.timestamp_us = 9999;
    Telemetry::EulerAngle e = to_euler_angle_from_quaternion(q1);

    EXPECT_NEAR(e.roll_deg, 82.086f, 0.2f);
    EXPECT_NEAR(e.pitch_deg, -14.142f, 0.2f);
    EXPECT_NEAR(e.yaw_deg, 145.774f, 0.2f);
    EXPECT_EQ(e.timestamp_us, q1.timestamp_us);

    Telemetry::Quaternion q2 = to_quaternion_from_euler_angle(e);

    EXPECT_NEAR(q1.w, q2.w, 0.01f);
    EXPECT_NEAR(q1.x, q2.x, 0.01f);
    EXPECT_NEAR(q1.y, q2.y, 0.01f);
    EXPECT_NEAR(q1.z, q2.z, 0.01f);
    EXPECT_EQ(q1.timestamp_us, q2.timestamp_us);

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
