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
static void start_offboard(const Offboard& offboard);
static void stop_offboard(const Offboard& offboard);
static void flip_roll(const Offboard& offboard, std::shared_ptr<Telemetry> telemetry);
static void flip_pitch(const Offboard& offboard, std::shared_ptr<Telemetry> telemetry);
static void turn_yaw(const Offboard& offboard);

TEST(SitlTestDisabled, OffboardAttitudeRate)
{
    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};

    ConnectionResult ret = mavsdk.add_any_connection("udpin://0.0.0.0:14540");
    ASSERT_EQ(ConnectionResult::Success, ret);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_TRUE(mavsdk.systems().at(0)->has_autopilot());

    auto system = mavsdk.systems().at(0);
    auto telemetry = std::make_shared<Telemetry>(system);
    auto action = std::make_shared<Action>(system);

    // FIXME: trying new plugin instantiation.
    auto offboard = Offboard{system};

    LogInfo() << "Waiting for system to be ready";
    ASSERT_TRUE(poll_condition_with_timeout(
        [telemetry]() { return telemetry->health_all_ok(); }, std::chrono::seconds(10)));

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

    // FIXME: workaround to prevent a race between set_takeoff_altitude and takeoff.
    std::this_thread::sleep_for(std::chrono::seconds(1));

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

void start_offboard(const Offboard& offboard)
{
    // Send it once before starting offboard, otherwise it will be rejected.
    // Also, turn yaw towards North.
    Offboard::Attitude full_up{};
    full_up.thrust_value = 1.0f;
    offboard.set_attitude(full_up);
    EXPECT_EQ(offboard.start(), Offboard::Result::Success);
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void stop_offboard(const Offboard& offboard)
{
    EXPECT_EQ(offboard.stop(), Offboard::Result::Success);
}

void flip_roll(const Offboard& offboard, std::shared_ptr<Telemetry> telemetry)
{
    while (telemetry->position().relative_altitude_m < 10.0f) {
        // Full speed up to avoid loosing too much altitude during the flip.
        Offboard::AttitudeRate full_up{};
        full_up.thrust_value = 1.0f;
        offboard.set_attitude_rate(full_up);
    }

    Offboard::AttitudeRate roll{};
    roll.roll_deg_s = 360.0f;
    roll.thrust_value = 0.25f;
    offboard.set_attitude_rate(roll);

    // FIXME: This only properly works at 1x speed right now.
    //        For lockstep setups running faster, we would need to use the
    //        speed factor into account which will be available in info soon.
    std::this_thread::sleep_for(std::chrono::milliseconds(750));

    Offboard::Attitude some_up{};
    some_up.thrust_value = 0.8f;
    offboard.set_attitude(some_up);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void flip_pitch(const Offboard& offboard, std::shared_ptr<Telemetry> telemetry)
{
    while (telemetry->position().relative_altitude_m < 10.0f) {
        // Full speed up to avoid loosing too much altitude during the flip.
        Offboard::AttitudeRate full_up{};
        full_up.thrust_value = 1.0f;
        offboard.set_attitude_rate(full_up);
    }

    Offboard::AttitudeRate pitch{};
    pitch.roll_deg_s = 0.0f;
    pitch.pitch_deg_s = 360.0f;
    pitch.yaw_deg_s = 0.0f;
    pitch.thrust_value = 0.25f;
    offboard.set_attitude_rate(pitch);

    // FIXME: This only properly works at 1x speed right now.
    //        For lockstep setups running faster, we would need to use the
    //        speed factor into account which will be available in info soon.
    std::this_thread::sleep_for(std::chrono::milliseconds(750));

    Offboard::Attitude some_up{};
    some_up.thrust_value = 0.8f;
    offboard.set_attitude(some_up);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}

void turn_yaw(const Offboard& offboard)
{
    Offboard::AttitudeRate yaw{};
    yaw.yaw_deg_s = 360.0f;
    yaw.thrust_value = 0.5;
    offboard.set_attitude_rate(yaw);

    // FIXME: This only properly works at 1x speed right now.
    //        For lockstep setups running faster, we would need to use the
    //        speed factor into account which will be available in info soon.
    std::this_thread::sleep_for(std::chrono::milliseconds(750));

    Offboard::Attitude some_up{};
    some_up.thrust_value = 0.8f;
    offboard.set_attitude(some_up);
    std::this_thread::sleep_for(std::chrono::seconds(2));
}
