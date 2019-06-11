#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "dronecode_sdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"

using namespace dronecode_sdk;

static void arm_and_takeoff(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry);
static void disarm_and_land(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry);
static void start_offboard(std::shared_ptr<Offboard> offboard);
static void stop_offboard(std::shared_ptr<Offboard> offboard);
static void flip_roll(std::shared_ptr<Offboard> offboard, std::shared_ptr<Telemetry> telemetry);
static void flip_pitch(std::shared_ptr<Offboard> offboard, std::shared_ptr<Telemetry> telemetry);
static void turn_yaw(std::shared_ptr<Offboard> offboard);

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

    arm_and_takeoff(action, telemetry);

    start_offboard(offboard);

    flip_roll(offboard, telemetry);

    flip_pitch(offboard, telemetry);

    turn_yaw(offboard);

    stop_offboard(offboard);

    disarm_and_land(action, telemetry);
}

void arm_and_takeoff(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry)
{
    ASSERT_EQ(action->arm(), Action::Result::SUCCESS);

    ASSERT_EQ(action->set_takeoff_altitude(5.0f), Action::Result::SUCCESS);

    ASSERT_EQ(action->takeoff(), Action::Result::SUCCESS);

    while (telemetry->position().relative_altitude_m < 4.0f) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void disarm_and_land(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry)
{
    action->land();

    while (telemetry->armed()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void start_offboard(std::shared_ptr<Offboard> offboard)
{
    // Send it once before starting offboard, otherwise it will be rejected.
    offboard->set_attitude_rate({0.0f, 0.0f, 0.0f, 1.0f});
    EXPECT_EQ(offboard->start(), Offboard::Result::SUCCESS);
}

void stop_offboard(std::shared_ptr<Offboard> offboard)
{
    EXPECT_EQ(offboard->stop(), Offboard::Result::SUCCESS);
}

void flip_roll(std::shared_ptr<Offboard> offboard, std::shared_ptr<Telemetry> telemetry)
{
    while (telemetry->position().relative_altitude_m < 6.0f) {
        // Full speed up to avoid loosing too much altitude during the flip.
        offboard->set_attitude_rate({0.0f, 0.0f, 0.0f, 1.0f});
    }

    enum class TurningState { Init, Turned45, Turned315 } turning_state{TurningState::Init};

    while (turning_state != TurningState::Turned315) {
        offboard->set_attitude_rate({360.0f, 0.0f, 0.0f, 0.25f});
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // We can't check for a negative angle from the beginning because we might
        // have a slightly negative angle right in the beginning. Therefore, we make
        // sure that we have started turning.
        switch (turning_state) {
            case TurningState::Init:
                if (telemetry->attitude_euler_angle().roll_deg > 45.0f) {
                    turning_state = TurningState::Turned45;
                }
                break;
            case TurningState::Turned45:
                if (telemetry->attitude_euler_angle().roll_deg > -45.0f &&
                    telemetry->attitude_euler_angle().roll_deg < 0.0f) {
                    turning_state = TurningState::Turned315;
                }
                break;
            case TurningState::Turned315:
                break;
        }
    }

    while (std::abs(telemetry->attitude_euler_angle().roll_deg) > 3.0f) {
        offboard->set_attitude({0.0f, 0.0f, 0.0f, 0.6f});
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void flip_pitch(std::shared_ptr<Offboard> offboard, std::shared_ptr<Telemetry> telemetry)
{
    while (telemetry->position().relative_altitude_m < 10.0f) {
        // Full speed up to avoid loosing too much altitude during the flip.
        offboard->set_attitude({0.0f, 0.0f, 0.0f, 1.0f});
    }

    enum class TurningState {
        Init,
        Turned45,
        Turned240,
        Turned315
    } turning_state{TurningState::Init};

    while (turning_state != TurningState::Turned315) {
        offboard->set_attitude_rate({0.0f, 360.0f, 0.0f, 0.25f});
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // We can't check for a negative angle from the beginning because we might
        // have a slightly negative angle right in the beginning. Therefore, we make
        // sure that we have started turning.
        // Euler angles are impractical for this because pitch only increases to 90
        // degrees before it goes down to 0 and eventually -90.
        switch (turning_state) {
            case TurningState::Init:
                if (telemetry->attitude_euler_angle().pitch_deg > 45.0f) {
                    turning_state = TurningState::Turned45;
                }
                break;
            case TurningState::Turned45:
                if (telemetry->attitude_euler_angle().pitch_deg < -60.0f) {
                    turning_state = TurningState::Turned240;
                }
                break;
            case TurningState::Turned240:
                if (telemetry->attitude_euler_angle().pitch_deg < 0.0f &&
                    telemetry->attitude_euler_angle().pitch_deg > -45.0f) {
                    turning_state = TurningState::Turned315;
                }
                break;
            case TurningState::Turned315:
                break;
        }
    }

    while (true) {
        offboard->set_attitude({0.0f, 0.0f, 0.0f, 0.6f});
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if (std::abs(telemetry->attitude_euler_angle().pitch_deg) < 3.0f) {
            break;
        }
    }
}

void turn_yaw(std::shared_ptr<Offboard> offboard)
{
    for (int i = 0; i < 100; ++i) {
        offboard->set_attitude_rate({0.0f, 0.0f, 360.0f, 0.5f});
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (int i = 0; i < 100; ++i) {
        offboard->set_attitude({0.0f, 0.0f, 0.0f, 0.5f});
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
