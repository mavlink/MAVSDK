#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecore.h"

using namespace dronecore;


TEST_F(SitlTest, OffboardVelocityNED)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(DroneCore::ConnectionResult::SUCCESS, ret);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = device.action().arm();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    action_ret = device.action().takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Send it once before starting offboard, otherwise it will be rejected.
    device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f});

    Offboard::Result offboard_result = device.offboard().start();

    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 90.0f});
    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        const float step_size = 0.01f;
        const float one_cycle = 2.0f * M_PI_F;
        const unsigned steps = 2 * unsigned(one_cycle / step_size);

        for (unsigned i = 0; i < steps; ++i) {
            float vx = 5.0f * sinf(i * step_size);
            //std::cout << "vx: " << vx << std::endl;
            device.offboard().set_velocity_ned({vx, 0.0f, 0.0f, 90.0f});
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 270.0f});
    std::this_thread::sleep_for(std::chrono::seconds(2));

    device.offboard().set_velocity_ned({0.0f, 0.0f, -2.0f, 180.0f});
    std::this_thread::sleep_for(std::chrono::seconds(4));

    device.offboard().set_velocity_ned({0.0f, 0.0f, 0.0f, 90.0f});
    std::this_thread::sleep_for(std::chrono::seconds(4));

    device.offboard().set_velocity_ned({0.0f, 0.0f, 1.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(4));

    offboard_result = device.offboard().stop();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    action_ret = device.action().land();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
}


TEST_F(SitlTest, OffboardVelocityBody)
{
    DroneCore dc;

    DroneCore::ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(DroneCore::ConnectionResult::SUCCESS, ret);

    // Wait for device to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Device &device = dc.device();

    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = device.action().arm();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    action_ret = device.action().takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Send it once before starting offboard, otherwise it will be rejected.
    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});

    Offboard::Result offboard_result = device.offboard().start();

    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    // Yaw clockwise and climb
    device.offboard().set_velocity_body({0.0f, 0.0f, -1.0f, 60.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Yaw anti-clockwise
    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, -60.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Wait for a bit
    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Fly a circle
    device.offboard().set_velocity_body({5.0f, 0.0f, 0.0f, 60.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Wait for a bit
    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Fly a circle sideways
    device.offboard().set_velocity_body({0.0f, -5.0f, 0.0f, 60.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Wait for a bit
    device.offboard().set_velocity_body({0.0f, 0.0f, 0.0f, 0.0f});
    std::this_thread::sleep_for(std::chrono::seconds(5));

    offboard_result = device.offboard().stop();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    action_ret = device.action().land();
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
}
