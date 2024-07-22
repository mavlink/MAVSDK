#include <atomic>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>

#include "mavsdk.h"
#include "mavsdk_math.h"
#include "unused.h"
#include "integration_test_helper.h"
#include "plugins/action/action.h"
#include "plugins/gimbal/gimbal.h"
#include "plugins/offboard/offboard.h"
#include "plugins/telemetry/telemetry.h"

using namespace mavsdk;

void gimbal_pattern(std::shared_ptr<Gimbal> gimbal);
void send_gimbal_roi_location(
    std::shared_ptr<Gimbal> gimbal, double latitude_deg, double longitude_deg, float altitude_m);
void receive_gimbal_result(Gimbal::Result result);
void receive_gimbal_attitude(Gimbal::Attitude attitude);

TEST(SitlTestGimbal, GimbalMove)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);

    // FIXME: This is what it should be, for now though with the typhoon_h480
    //        SITL simulation, the gimbal is hooked up to the autopilot.
    // ASSERT_TRUE(system.has_gimbal());
    ASSERT_TRUE(system->has_autopilot());

    auto gimbal = std::make_shared<Gimbal>(system);

    gimbal->subscribe_attitude(&receive_gimbal_attitude);

    EXPECT_EQ(gimbal->take_control(Gimbal::ControlMode::Primary), Gimbal::Result::Success);

    EXPECT_EQ(gimbal->set_mode(Gimbal::GimbalMode::YawFollow), Gimbal::Result::Success);

    LogInfo() << "Pitch down for a  bit";
    EXPECT_EQ(gimbal->set_pitch_rate_and_yaw_rate(-10.0f, 0.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    LogInfo() << "Pitch up for a bit";
    EXPECT_EQ(gimbal->set_pitch_rate_and_yaw_rate(10.0f, 0.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    LogInfo() << "Yaw right for a bit";
    EXPECT_EQ(gimbal->set_pitch_rate_and_yaw_rate(0.0f, 10.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    LogInfo() << "Yaw left for a bit";
    EXPECT_EQ(gimbal->set_pitch_rate_and_yaw_rate(0.0f, -10.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(2));

    LogInfo() << "Reset back to 0,0";
    EXPECT_EQ(gimbal->set_pitch_and_yaw(0.0f, 0.0f), Gimbal::Result::Success);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_EQ(gimbal->take_control(Gimbal::ControlMode::None), Gimbal::Result::Success);
}

TEST(SitlTestGimbal, GimbalAngles)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);

    // FIXME: This is what it should be, for now though with the typhoon_h480
    //        SITL simulation, the gimbal is hooked up to the autopilot.
    // ASSERT_TRUE(system.has_gimbal());
    ASSERT_TRUE(system->has_autopilot());

    auto gimbal = std::make_shared<Gimbal>(system);

    EXPECT_EQ(gimbal->take_control(Gimbal::ControlMode::Primary), Gimbal::Result::Success);

    EXPECT_EQ(gimbal->set_mode(Gimbal::GimbalMode::YawFollow), Gimbal::Result::Success);

    LogInfo() << "Pitch 45 degrees down";
    EXPECT_EQ(gimbal->set_pitch_and_yaw(-45.0f, 0.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LogInfo() << "Pitch back up";
    EXPECT_EQ(gimbal->set_pitch_and_yaw(0.0f, 0.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LogInfo() << "Pitch 45 degrees up";
    EXPECT_EQ(gimbal->set_pitch_and_yaw(45.0f, 0.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LogInfo() << "Pitch back down";
    EXPECT_EQ(gimbal->set_pitch_and_yaw(0.0f, 0.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    LogInfo() << "Yaw 45 degrees to the right";
    EXPECT_EQ(gimbal->set_pitch_and_yaw(0.0f, 45.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LogInfo() << "Yaw forward";
    EXPECT_EQ(gimbal->set_pitch_and_yaw(0.0f, 0.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LogInfo() << "Yaw 45 degrees to the left";
    EXPECT_EQ(gimbal->set_pitch_and_yaw(0.0f, -45.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    LogInfo() << "Yaw forward";
    EXPECT_EQ(gimbal->set_pitch_and_yaw(0.0f, 0.0f), Gimbal::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_EQ(gimbal->take_control(Gimbal::ControlMode::None), Gimbal::Result::Success);
}

TEST(SitlTestGimbal, GimbalTakeoffAndMove)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    // ASSERT_TRUE(system.has_gimbal());
    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto gimbal = std::make_shared<Gimbal>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    Action::Result action_result = action->arm();
    EXPECT_EQ(action_result, Action::Result::Success);

    action_result = action->takeoff();
    EXPECT_EQ(action_result, Action::Result::Success);

    gimbal->subscribe_attitude(&receive_gimbal_attitude);

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Slowly fly circle.
    Offboard::VelocityBodyYawspeed circle;
    circle.forward_m_s = 0.5f;
    circle.right_m_s = 0.0f;
    circle.down_m_s = 0.0f;
    circle.yawspeed_deg_s = 10.0f;

    EXPECT_EQ(offboard->set_velocity_body(circle), Offboard::Result::Success);
    EXPECT_EQ(offboard->start(), Offboard::Result::Success);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_EQ(gimbal->take_control(Gimbal::ControlMode::Primary), Gimbal::Result::Success);

    // First use gimbal in yaw follow mode.
    EXPECT_EQ(gimbal->set_mode(Gimbal::GimbalMode::YawFollow), Gimbal::Result::Success);

    gimbal_pattern(gimbal);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Now set to lock mode
    EXPECT_EQ(gimbal->set_mode(Gimbal::GimbalMode::YawLock), Gimbal::Result::Success);

    gimbal_pattern(gimbal);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    EXPECT_EQ(gimbal->take_control(Gimbal::ControlMode::None), Gimbal::Result::Success);

    action->land();
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void gimbal_pattern(std::shared_ptr<Gimbal> gimbal)
{
    // Look forward
    gimbal->set_pitch_and_yaw_async(0.0f, 0.0f, &receive_gimbal_result);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Look right
    for (int i = 0; i < 60; ++i) {
        gimbal->set_pitch_and_yaw_async(0.0f, static_cast<float>(i), &receive_gimbal_result);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Look left
    for (int i = 60; i >= -60; --i) {
        gimbal->set_pitch_and_yaw_async(0.0f, static_cast<float>(i), &receive_gimbal_result);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Look up
    for (int i = -60; i <= 0; ++i) {
        gimbal->set_pitch_and_yaw_async(0.0f, static_cast<float>(i), &receive_gimbal_result);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Tilt down
    for (int i = 0; i > -60; --i) {
        gimbal->set_pitch_and_yaw_async(static_cast<float>(i), 0.0f, &receive_gimbal_result);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // And back up
    for (int i = -60; i <= 0; ++i) {
        gimbal->set_pitch_and_yaw_async(static_cast<float>(i), 0.0f, &receive_gimbal_result);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    // Look forward
    gimbal->set_pitch_and_yaw_async(0.0f, 0.0f, &receive_gimbal_result);
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

TEST(SitlTestGimbal, GimbalROIOffboard)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    ASSERT_EQ(mavsdk.systems().size(), 1);
    auto system = mavsdk.systems().at(0);
    // ASSERT_TRUE(system.has_gimbal());
    ASSERT_TRUE(system->has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto gimbal = std::make_shared<Gimbal>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() {
            LogInfo() << "Waiting for system to be ready";
            return telemetry->health_all_ok();
        },
        std::chrono::seconds(10)));

    const Telemetry::Position& position = telemetry->position();

    // set the ROI location: a bit to the north of the vehicle's location
    const double latitude_offset_deg = 3. / 111111.; // this is about 3 m
    send_gimbal_roi_location(
        gimbal,
        position.latitude_deg + latitude_offset_deg,
        position.longitude_deg,
        position.absolute_altitude_m + 1.f);

    Action::Result action_result = action->arm();
    EXPECT_EQ(action_result, Action::Result::Success);

    action_result = action->takeoff();
    EXPECT_EQ(action_result, Action::Result::Success);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    gimbal->subscribe_attitude(&receive_gimbal_attitude);

    // Send it once before starting offboard, otherwise it will be rejected.
    Offboard::VelocityNedYaw still;
    still.north_m_s = 0.0f;
    still.east_m_s = 0.0f;
    still.down_m_s = 0.0f;
    still.yaw_deg = 0.0f;
    offboard->set_velocity_ned(still);
    Offboard::Result offboard_result = offboard->start();
    EXPECT_EQ(offboard_result, Offboard::Result::Success);

    auto fly_straight = [&offboard](int step_count, float max_speed) {
        for (int i = 0; i < step_count; ++i) {
            int k = (i <= step_count / 2) ? i : step_count - i;
            float vy = static_cast<float>(k) / (step_count / 2) * max_speed;

            Offboard::VelocityNedYaw move_east;
            move_east.north_m_s = 0.0f;
            move_east.east_m_s = vy;
            move_east.down_m_s = 0.0f;
            move_east.yaw_deg = 90.0f;
            offboard->set_velocity_ned(move_east);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };

    // fly east for a bit
    fly_straight(300, 2.f);

    // fly in north-south direction (back & forth a few times)
    const float step_size = 0.01f;
    const float one_cycle = 2.0f * static_cast<float>(PI);
    const unsigned steps = static_cast<unsigned>(2.5f * one_cycle / step_size);

    for (unsigned i = 0; i < steps; ++i) {
        float vx = 5.0f * sinf(i * step_size);

        Offboard::VelocityNedYaw move_north;
        move_north.north_m_s = vx;
        move_north.east_m_s = 0.0f;
        move_north.down_m_s = 0.0f;
        move_north.yaw_deg = 90.0f;
        offboard->set_velocity_ned(move_north);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // fly west for a bit
    fly_straight(600, -4.f);

    action->land();
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void send_gimbal_roi_location(
    std::shared_ptr<Gimbal> gimbal, double latitude_deg, double longitude_deg, float altitude_m)
{
    gimbal->set_roi_location_async(latitude_deg, longitude_deg, altitude_m, &receive_gimbal_result);
}

void receive_gimbal_result(Gimbal::Result result)
{
    EXPECT_EQ(result, Gimbal::Result::Success);
}

void receive_gimbal_attitude(Gimbal::Attitude attitude)
{
    std::cout << "Gimbal angle pitch: " << attitude.euler_angle_forward.pitch_deg
              << " deg, yaw: " << attitude.euler_angle_forward.yaw_deg
              << " yaw (relative to forward)\n";
    std::cout << "Gimbal angle pitch: " << attitude.euler_angle_north.pitch_deg
              << " deg, yaw: " << attitude.euler_angle_north.yaw_deg
              << " yaw (relative to North)\n";
}
