#include <iostream>
#include <functional>
#include <memory>
#include <atomic>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecore.h"

using namespace std::placeholders; // for `_1`
using namespace dronecore;


void send_new_gimbal_command(Device &device, int i);
void receive_gimbal_result(Gimbal::Result result);
void receive_gimbal_attitude_euler_angles(Telemetry::EulerAngle euler_angle);


TEST_F(SitlTest, GimbalMove)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, DroneCore::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();

    device.telemetry().set_rate_camera_attitude(10.0);

    device.telemetry().camera_attitude_euler_angle_async(
        std::bind(&receive_gimbal_attitude_euler_angles, _1));

    for (int i = 0; i < 500; i += 1) {

        send_new_gimbal_command(device, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

TEST_F(SitlTest, GimbalTakeoffAndMove)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, DroneCore::ConnectionResult::SUCCESS);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        LogInfo() << "waiting for device to be ready";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    device.action().arm();
    device.action().takeoff();

    device.telemetry().set_rate_camera_attitude(10.0);

    device.telemetry().camera_attitude_euler_angle_async(
        std::bind(&receive_gimbal_attitude_euler_angles, _1));

    for (int i = 0; i < 500; i += 1) {

        send_new_gimbal_command(device, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    device.action().land();
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void send_new_gimbal_command(Device &device, int i)
{
    float pitch_deg = 30.0f * cosf(i / 360.0f * 2 * M_PI_F);
    float yaw_deg = 45.0f * sinf(i / 360.0f * 2 * M_PI_F);

    device.gimbal().set_pitch_and_yaw_async(pitch_deg, yaw_deg,
                                            std::bind(&receive_gimbal_result, _1));
}

void receive_gimbal_result(Gimbal::Result result)
{
    EXPECT_EQ(result, Gimbal::Result::SUCCESS);
}

void receive_gimbal_attitude_euler_angles(Telemetry::EulerAngle euler_angle)
{
    LogInfo() << "Received gimbal attitude: "
              << euler_angle.roll_deg << ", "
              << euler_angle.pitch_deg << ", "
              << euler_angle.yaw_deg;
}

