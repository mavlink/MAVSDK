#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"

struct Rates {
    float roll;
    float pitch;
    float yaw;
    float thrust;
};
using namespace dronecode_sdk;
Rates close_loop(float, float, float, std::shared_ptr<Telemetry> telemetry);

TEST_F(SitlTest, OffboardAttitudeRate)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ConnectionResult::SUCCESS, ret);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_TRUE(dc.system().has_autopilot());

    System &system = dc.system();
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);

    while (!telemetry->health_all_ok()) {
        std::cout << "waiting for system to be ready" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_ret = action->arm();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    action_ret = action->takeoff();
    ASSERT_EQ(Action::Result::SUCCESS, action_ret);

    // Taking off and reaching an altitude of 2.5mts
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Send it once before starting offboard, otherwise it will be rejected.
    offboard->set_attitude_rate({0.0f, 0.0f, 0.0f, 0.0f});
    // Printing Pitch, roll and yaw euler angles.
    std::cout << "Roll: " << telemetry->attitude_euler_angle().roll_deg
              << " Pitch: " << telemetry->attitude_euler_angle().pitch_deg
              << " Yaw: " << telemetry->attitude_euler_angle().yaw_deg << std::endl;

    // Starting Offboard Mode
    Offboard::Result offboard_result = offboard->start();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    // Gaining altitude
    offboard->set_attitude_rate({0.0f, 0.0f, 0.0f, 0.5f});
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Performing Acutal tests
    // Testing Roll
    std::cout << "Rolling at 45 degrees/seconds" << std::endl;
    offboard->set_attitude_rate({45.0f, 0.0f, 0.0f, 0.5f});
    for (int i = 0; i < 1000; i++) {
        if (telemetry->attitude_euler_angle().roll_deg <= 45.0f) {
            auto roll_deg = close_loop(45.0f, 0.0f, 0.0f, telemetry);
            offboard->set_attitude_rate(
                {roll_deg.roll, roll_deg.pitch, roll_deg.yaw, roll_deg.thrust});
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            std::cout << "Roll Angle Reached" << std::endl;
            std::cout << "Roll: " << telemetry->attitude_euler_angle().roll_deg
                      << " Pitch: " << telemetry->attitude_euler_angle().pitch_deg
                      << " Yaw: " << telemetry->attitude_euler_angle().yaw_deg << std::endl;
            break;
        }
    }

    // Balancing
    for (int i = 0; i < 1000; i++) {
        if (telemetry->attitude_euler_angle().roll_deg >= 0.2f) {
            auto roll_bal = close_loop(0.0f, 0.0f, 0.0f, telemetry);
            offboard->set_attitude_rate(
                {roll_bal.roll, roll_bal.pitch, roll_bal.yaw, roll_bal.thrust});
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            std::cout << "Balancing Roll" << std::endl;
            std::cout << "Roll: " << telemetry->attitude_euler_angle().roll_deg
                      << " Pitch: " << telemetry->attitude_euler_angle().pitch_deg
                      << " Yaw: " << telemetry->attitude_euler_angle().yaw_deg << std::endl;
            break;
        }
    }

    // Testing Pitch
    std::cout << "Pitching at 45 degrees/second" << std::endl;
    offboard->set_attitude_rate({0.0f, 45.0f, 0.0f, 0.5f});
    for (int i = 0; i < 1000; i++) {
        if (telemetry->attitude_euler_angle().pitch_deg <= 45.0f) {
            auto pitch_deg = close_loop(0.0f, 45.0f, 0.0f, telemetry);
            offboard->set_attitude_rate(
                {pitch_deg.roll, pitch_deg.pitch, pitch_deg.yaw, pitch_deg.thrust});
            std::this_thread::sleep_for(std::chrono::milliseconds(3));
        } else {
            std::cout << "Pitch angle achieved" << std::endl;
            std::cout << "Roll: " << telemetry->attitude_euler_angle().roll_deg
                      << " Pitch: " << telemetry->attitude_euler_angle().pitch_deg
                      << " Yaw: " << telemetry->attitude_euler_angle().yaw_deg << std::endl;
            break;
        }
    }

    // Balancing
    for (int i = 0; i < 1000; i++) {
        if (telemetry->attitude_euler_angle().pitch_deg > 0.2f) {
            auto pitch_bal = close_loop(0.0f, 0.0f, 0.0f, telemetry);
            offboard->set_attitude_rate(
                {pitch_bal.roll, pitch_bal.pitch, pitch_bal.yaw, pitch_bal.thrust});
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            std::cout << "Pitch Balanced" << std::endl;
            std::cout << "Roll: " << telemetry->attitude_euler_angle().roll_deg
                      << " Pitch: " << telemetry->attitude_euler_angle().pitch_deg
                      << " Yaw: " << telemetry->attitude_euler_angle().yaw_deg << std::endl;
            break;
        }
    }

    // Testing Yaw
    std::cout << "Yaw at -60 degrees/second" << std::endl;
    offboard->set_attitude_rate({0.0f, 0.0f, -60.0f, 0.5f});
    for (int i = 0; i < 1000; i++) {
        if (telemetry->attitude_euler_angle().yaw_deg >=
            -60.0f) // Flipping the comparison operator while turning anti-clock wise.
        {
            auto yaw_deg = close_loop(0.0f, 0.0f, -60.0f, telemetry);
            offboard->set_attitude_rate({yaw_deg.roll, yaw_deg.pitch, yaw_deg.yaw, yaw_deg.thrust});
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        } else {
            std::cout << "Yaw angle acheived" << std::endl;
            std::cout << "Roll: " << telemetry->attitude_euler_angle().roll_deg
                      << " Pitch: " << telemetry->attitude_euler_angle().pitch_deg
                      << " Yaw: " << telemetry->attitude_euler_angle().yaw_deg << std::endl;
            break;
        }
    }
    std::cout << "Roll: " << telemetry->attitude_euler_angle().roll_deg
              << " Pitch: " << telemetry->attitude_euler_angle().pitch_deg
              << " Yaw: " << telemetry->attitude_euler_angle().yaw_deg << std::endl;

    // Balancing
    for (int i = 0; i < 1000; i++) {
        if (telemetry->attitude_euler_angle().yaw_deg <= 0.2f) {
            auto yaw_bal = close_loop(0.0f, 0.0f, 0.0f, telemetry);
            offboard->set_attitude_rate({yaw_bal.roll, yaw_bal.pitch, yaw_bal.yaw, yaw_bal.thrust});
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else {
            std::cout << "Balanced Yaw" << std::endl;
            std::cout << "Roll: " << telemetry->attitude_euler_angle().roll_deg
                      << " Pitch: " << telemetry->attitude_euler_angle().pitch_deg
                      << " Yaw: " << telemetry->attitude_euler_angle().yaw_deg << std::endl;
            break;
        }
    }

    // Stopping offboard mode and Landing.
    offboard_result = offboard->stop();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);
    action_ret = action->land();
    std::cout << "Landed! " << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::cout << "Landed" << std::endl;
    std::cout << "Roll: " << telemetry->attitude_euler_angle().roll_deg
              << " Pitch: " << telemetry->attitude_euler_angle().pitch_deg
              << " Yaw: " << telemetry->attitude_euler_angle().yaw_deg << std::endl;
    EXPECT_EQ(action_ret, Action::Result::SUCCESS);
}

// Creating a closing function:
Rates close_loop(float roll_rate,
                 float pitch_rate,
                 float yaw_rate,
                 std::shared_ptr<Telemetry> telemetry)
{
    double roll_control = 6.0 * (double)(roll_rate - telemetry->attitude_euler_angle().roll_deg);
    double pitch_control = 7.2 * (double)(pitch_rate - telemetry->attitude_euler_angle().pitch_deg);
    double yaw_control = 3.80 * (double)(yaw_rate - telemetry->attitude_euler_angle().yaw_deg);
    double thrust_control = 0.1 * (double)(10.5f - telemetry->position().relative_altitude_m);
    if (thrust_control < 0.1) {
        thrust_control = 0.10;
    }
    return {(float)roll_control, (float)pitch_control, (float)yaw_control, (float)(thrust_control)};
}
