#include <atomic>
#include <cmath>
#include <functional>
#include <iostream>
#include <memory>

#include "dronecode_sdk.h"
#include "integration_test_helper.h"
#include "plugins/action/action.h"
#include "plugins/gimbal/gimbal.h"
#include "plugins/offboard/offboard.h"
#include "plugins/telemetry/telemetry.h"

using namespace dronecode_sdk;

void send_new_gimbal_command(std::shared_ptr<Gimbal> gimbal, int i);
void send_gimbal_roi_location(std::shared_ptr<Gimbal> gimbal,
                              double latitude_deg,
                              double longitude_deg,
                              float altitude_m);
void receive_gimbal_result(Gimbal::Result result);
void receive_gimbal_attitude_euler_angles(Telemetry::EulerAngle euler_angle);

TEST(SitlTestGimbal, GimbalMove)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    System &system = dc.system();
    // FIXME: This is what it should be, for now though with the typhoon_h480
    //        SITL simulation, the gimbal is hooked up to the autopilot.
    // ASSERT_TRUE(system.has_gimbal());
    ASSERT_TRUE(system.has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto gimbal = std::make_shared<Gimbal>(system);

    telemetry->set_rate_camera_attitude(10.0);

    telemetry->camera_attitude_euler_angle_async(&receive_gimbal_attitude_euler_angles);

    for (int i = 0; i < 500; i += 1) {
        send_new_gimbal_command(gimbal, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

TEST(SitlTestGimbal, GimbalTakeoffAndMove)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    System &system = dc.system();
    // ASSERT_TRUE(system.has_gimbal());
    ASSERT_TRUE(system.has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto gimbal = std::make_shared<Gimbal>(system);
    auto action = std::make_shared<Action>(system);

    while (!telemetry->health_all_ok()) {
        LogInfo() << "waiting for system to be ready";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    Action::Result action_result = action->arm();
    EXPECT_EQ(action_result, Action::Result::SUCCESS);

    action_result = action->takeoff();
    EXPECT_EQ(action_result, Action::Result::SUCCESS);

    telemetry->set_rate_camera_attitude(10.0);

    telemetry->camera_attitude_euler_angle_async(&receive_gimbal_attitude_euler_angles);

    for (int i = 0; i < 500; i += 1) {
        send_new_gimbal_command(gimbal, i);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    action->land();
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

TEST(SitlTestGimbal, GimbalROIOffboard)
{
    DronecodeSDK dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));
    System &system = dc.system();
    // ASSERT_TRUE(system.has_gimbal());
    ASSERT_TRUE(system.has_autopilot());

    auto telemetry = std::make_shared<Telemetry>(system);
    auto gimbal = std::make_shared<Gimbal>(system);
    auto action = std::make_shared<Action>(system);
    auto offboard = std::make_shared<Offboard>(system);

    while (!telemetry->health_all_ok()) {
        LogInfo() << "waiting for system to be ready";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    const Telemetry::Position &position = telemetry->position();

    // set the ROI location: a bit to the north of the vehicle's location
    const double latitude_offset_deg = 3. / 111111.; // this is about 3 m
    send_gimbal_roi_location(gimbal,
                             position.latitude_deg + latitude_offset_deg,
                             position.longitude_deg,
                             position.absolute_altitude_m + 1.f);

    Action::Result action_result = action->arm();
    EXPECT_EQ(action_result, Action::Result::SUCCESS);

    action_result = action->takeoff();
    EXPECT_EQ(action_result, Action::Result::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(5));

    telemetry->set_rate_camera_attitude(10.0);

    telemetry->camera_attitude_euler_angle_async(&receive_gimbal_attitude_euler_angles);

    // Send it once before starting offboard, otherwise it will be rejected.
    offboard->set_velocity_ned({0.0f, 0.0f, 0.0f, 0.0f});
    Offboard::Result offboard_result = offboard->start();
    EXPECT_EQ(offboard_result, Offboard::Result::SUCCESS);

    auto fly_straight = [&offboard](int step_count, float max_speed) {
        for (int i = 0; i < step_count; ++i) {
            int k = (i <= step_count / 2) ? i : step_count - i;
            float vy = static_cast<float>(k) / (step_count / 2) * max_speed;
            offboard->set_velocity_ned({0.0f, vy, 0.0f, 90.0f});
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };

    // fly east for a bit
    fly_straight(300, 2.f);

    // fly in north-south direction (back & forth a few times)
    const float step_size = 0.01f;
    const float one_cycle = 2.0f * M_PI_F;
    const unsigned steps = static_cast<unsigned>(2.5f * one_cycle / step_size);

    for (unsigned i = 0; i < steps; ++i) {
        float vx = 5.0f * sinf(i * step_size);
        offboard->set_velocity_ned({vx, 0.0f, 0.0f, 90.0f});
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // fly west for a bit
    fly_straight(600, -4.f);

    action->land();
    std::this_thread::sleep_for(std::chrono::seconds(3));
}

void send_new_gimbal_command(std::shared_ptr<Gimbal> gimbal, int i)
{
    float pitch_deg = 30.0f * cosf(i / 360.0f * 2 * M_PI_F);
    float yaw_deg = 45.0f * sinf(i / 360.0f * 2 * M_PI_F);

    gimbal->set_pitch_and_yaw_async(pitch_deg, yaw_deg, &receive_gimbal_result);
}

void send_gimbal_roi_location(std::shared_ptr<Gimbal> gimbal,
                              double latitude_deg,
                              double longitude_deg,
                              float altitude_m)
{
    gimbal->set_roi_location_async(latitude_deg, longitude_deg, altitude_m, &receive_gimbal_result);
}

void receive_gimbal_result(Gimbal::Result result)
{
    EXPECT_EQ(result, Gimbal::Result::SUCCESS);
}

void receive_gimbal_attitude_euler_angles(Telemetry::EulerAngle euler_angle)
{
    LogInfo() << "Received gimbal attitude: " << euler_angle.roll_deg << ", "
              << euler_angle.pitch_deg << ", " << euler_angle.yaw_deg;
}
