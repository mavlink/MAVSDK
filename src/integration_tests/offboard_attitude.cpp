#include <iostream>
#include <cmath>
#include "integration_test_helper.h"
#include "mavsdk.h"
#include "plugins/action/action.h"
#include "plugins/telemetry/telemetry.h"
#include "plugins/offboard/offboard.h"

using namespace mavsdk;

static void arm_and_takeoff(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry);
static void disarm_and_land(std::shared_ptr<Action> action, std::shared_ptr<Telemetry> telemetry);
static void start_offboard(std::shared_ptr<Offboard> offboard);
static void stop_offboard(std::shared_ptr<Offboard> offboard);
static void flip_roll(std::shared_ptr<Offboard> offboard, std::shared_ptr<Telemetry> telemetry);
static void flip_pitch(std::shared_ptr<Offboard> offboard, std::shared_ptr<Telemetry> telemetry);
static void turn_yaw(std::shared_ptr<Offboard> offboard);

TEST_F(SitlTest, OffboardAttitudeRate)
{
    Mavsdk dc;

    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ConnectionResult::SUCCESS, ret);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_TRUE(dc.system().has_autopilot());

    System& system = dc.system();
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
    ASSERT_EQ(action->arm(), Action::Result::Success);

    ASSERT_EQ(action->set_takeoff_altitude(5.0f), Action::Result::Success);

    ASSERT_EQ(action->takeoff(), Action::Result::Success);

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
    Offboard::AttitudeRate full_up{};
    full_up.thrust_value = 1.0f;
    offboard->set_attitude_rate(full_up);
    EXPECT_EQ(offboard->start(), Offboard::Result::Success);
}

void stop_offboard(std::shared_ptr<Offboard> offboard)
{
    EXPECT_EQ(offboard->stop(), Offboard::Result::Success);
}

void flip_roll(std::shared_ptr<Offboard> offboard, std::shared_ptr<Telemetry> telemetry)
{
    while (telemetry->position().relative_altitude_m < 6.0f) {
        // Full speed up to avoid loosing too much altitude during the flip.
        Offboard::AttitudeRate full_up{};
        full_up.thrust_value = 1.0f;
        offboard->set_attitude_rate(full_up);
    }

    enum class TurningState { Init, Turned45, Turned315 } turning_state{TurningState::Init};

    while (turning_state != TurningState::Turned315) {
        Offboard::AttitudeRate roll{};
        roll.roll_deg_s = 360.0f;
        roll.thrust_value = 0.25f;
        offboard->set_attitude_rate(roll);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // We can't check for a negative angle from the beginning because we might
        // have a slightly negative angle right in the beginning. Therefore, we make
        // sure that we have started turning.
        switch (turning_state) {
            case TurningState::Init:
                if (telemetry->attitude_euler().roll_deg > 45.0f) {
                    turning_state = TurningState::Turned45;
                }
                break;
            case TurningState::Turned45:
                if (telemetry->attitude_euler().roll_deg > -45.0f &&
                    telemetry->attitude_euler().roll_deg < 0.0f) {
                    turning_state = TurningState::Turned315;
                }
                break;
            case TurningState::Turned315:
                break;
        }
    }

    while (std::abs(telemetry->attitude_euler().roll_deg) > 3.0f) {
        Offboard::Attitude some_up{};
        some_up.thrust_value = 0.6f;
        offboard->set_attitude(some_up);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void flip_pitch(std::shared_ptr<Offboard> offboard, std::shared_ptr<Telemetry> telemetry)
{
    while (telemetry->position().relative_altitude_m < 10.0f) {
        // Full speed up to avoid loosing too much altitude during the flip.
        Offboard::Attitude full_up{};
        full_up.thrust_value = 1.0f;
        offboard->set_attitude(full_up);
    }

    enum class TurningState {
        Init,
        Turned45,
        Turned240,
        Turned315
    } turning_state{TurningState::Init};

    while (turning_state != TurningState::Turned315) {
        Offboard::AttitudeRate pitch{};
        pitch.roll_deg_s = 0.0f;
        pitch.pitch_deg_s = 360.0f;
        pitch.yaw_deg_s = 0.0f;
        pitch.thrust_value = 0.25f;
        offboard->set_attitude_rate(pitch);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // We can't check for a negative angle from the beginning because we might
        // have a slightly negative angle right in the beginning. Therefore, we make
        // sure that we have started turning.
        // Euler angles are impractical for this because pitch only increases to 90
        // degrees before it goes down to 0 and eventually -90.
        switch (turning_state) {
            case TurningState::Init:
                if (telemetry->attitude_euler().pitch_deg > 45.0f) {
                    turning_state = TurningState::Turned45;
                }
                break;
            case TurningState::Turned45:
                if (telemetry->attitude_euler().pitch_deg < -60.0f) {
                    turning_state = TurningState::Turned240;
                }
                break;
            case TurningState::Turned240:
                if (telemetry->attitude_euler().pitch_deg < 0.0f &&
                    telemetry->attitude_euler().pitch_deg > -45.0f) {
                    turning_state = TurningState::Turned315;
                }
                break;
            case TurningState::Turned315:
                break;
        }
    }

    while (true) {
        Offboard::Attitude some_up{};
        some_up.thrust_value = 0.6f;
        offboard->set_attitude(some_up);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        if (std::abs(telemetry->attitude_euler().pitch_deg) < 3.0f) {
            break;
        }
    }
}

void turn_yaw(std::shared_ptr<Offboard> offboard)
{
    for (int i = 0; i < 100; ++i) {
        Offboard::AttitudeRate yaw{};
        yaw.yaw_deg_s = 360.0f;
        yaw.thrust_value = 0.5;
        offboard->set_attitude_rate(yaw);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (int i = 0; i < 100; ++i) {
        Offboard::Attitude stay{};
        stay.thrust_value = 0.5;
        offboard->set_attitude(stay);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
