#include "flight_mode.hpp"
#include "ardupilot_custom_mode.hpp"
#include "px4_custom_mode.hpp"
#include "autopilot.hpp"
#include <gtest/gtest.h>

using namespace mavsdk;

static uint32_t px4_mode(uint8_t main_mode, uint8_t sub_mode = 0)
{
    px4::px4_custom_mode m{};
    m.main_mode = main_mode;
    m.sub_mode = sub_mode;
    return m.data;
}

TEST(FlightMode, Px4MainModes)
{
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(px4_mode(px4::PX4_CUSTOM_MAIN_MODE_MANUAL)),
        FlightMode::Manual);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(px4_mode(px4::PX4_CUSTOM_MAIN_MODE_ALTCTL)),
        FlightMode::Altctl);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(px4_mode(px4::PX4_CUSTOM_MAIN_MODE_POSCTL)),
        FlightMode::Posctl);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(px4_mode(px4::PX4_CUSTOM_MAIN_MODE_ACRO)), FlightMode::Acro);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(px4_mode(px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD)),
        FlightMode::Offboard);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(px4_mode(px4::PX4_CUSTOM_MAIN_MODE_STABILIZED)),
        FlightMode::Stabilized);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(px4_mode(px4::PX4_CUSTOM_MAIN_MODE_RATTITUDE)),
        FlightMode::Rattitude);
}

TEST(FlightMode, Px4AutoSubModes)
{
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(
            px4_mode(px4::PX4_CUSTOM_MAIN_MODE_AUTO, px4::PX4_CUSTOM_SUB_MODE_AUTO_READY)),
        FlightMode::Ready);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(
            px4_mode(px4::PX4_CUSTOM_MAIN_MODE_AUTO, px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF)),
        FlightMode::Takeoff);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(
            px4_mode(px4::PX4_CUSTOM_MAIN_MODE_AUTO, px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER)),
        FlightMode::Hold);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(
            px4_mode(px4::PX4_CUSTOM_MAIN_MODE_AUTO, px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION)),
        FlightMode::Mission);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(
            px4_mode(px4::PX4_CUSTOM_MAIN_MODE_AUTO, px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL)),
        FlightMode::ReturnToLaunch);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(
            px4_mode(px4::PX4_CUSTOM_MAIN_MODE_AUTO, px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND)),
        FlightMode::Land);
    EXPECT_EQ(
        to_flight_mode_from_px4_mode(
            px4_mode(px4::PX4_CUSTOM_MAIN_MODE_AUTO, px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET)),
        FlightMode::FollowMe);
}

TEST(FlightMode, ArduCopter)
{
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_copter_mode(static_cast<uint32_t>(ardupilot::CopterMode::Auto)),
        FlightMode::Mission);
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_copter_mode(static_cast<uint32_t>(ardupilot::CopterMode::Land)),
        FlightMode::Land);
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_copter_mode(static_cast<uint32_t>(ardupilot::CopterMode::Rtl)),
        FlightMode::ReturnToLaunch);
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_copter_mode(
            static_cast<uint32_t>(ardupilot::CopterMode::Guided)),
        FlightMode::Offboard);
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_copter_mode(
            static_cast<uint32_t>(ardupilot::CopterMode::Unknown)),
        FlightMode::Unknown);
}

TEST(FlightMode, ArduPlane)
{
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_plane_mode(static_cast<uint32_t>(ardupilot::PlaneMode::Fbwa)),
        FlightMode::FBWA);
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_plane_mode(static_cast<uint32_t>(ardupilot::PlaneMode::Fbwb)),
        FlightMode::FBWB);
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_plane_mode(static_cast<uint32_t>(ardupilot::PlaneMode::Takeoff)),
        FlightMode::Takeoff);
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_plane_mode(static_cast<uint32_t>(ardupilot::PlaneMode::Auto)),
        FlightMode::Mission);
}

TEST(FlightMode, ArduRover)
{
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_rover_mode(static_cast<uint32_t>(ardupilot::RoverMode::Hold)),
        FlightMode::Hold);
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_rover_mode(static_cast<uint32_t>(ardupilot::RoverMode::RTL)),
        FlightMode::ReturnToLaunch);
    EXPECT_EQ(
        to_flight_mode_from_ardupilot_rover_mode(static_cast<uint32_t>(ardupilot::RoverMode::Manual)),
        FlightMode::Manual);
}

TEST(FlightMode, CustomModeDispatch)
{
    // Unknown autopilot path uses PX4 decoder.
    EXPECT_EQ(
        to_flight_mode_from_custom_mode(
            Autopilot::Px4, MAV_TYPE_QUADROTOR, px4_mode(px4::PX4_CUSTOM_MAIN_MODE_MANUAL)),
        FlightMode::Manual);

    EXPECT_EQ(
        to_flight_mode_from_custom_mode(
            Autopilot::ArduPilot,
            MAV_TYPE_QUADROTOR,
            static_cast<uint32_t>(ardupilot::CopterMode::Land)),
        FlightMode::Land);

    EXPECT_EQ(
        to_flight_mode_from_custom_mode(
            Autopilot::ArduPilot,
            MAV_TYPE_FIXED_WING,
            static_cast<uint32_t>(ardupilot::PlaneMode::Fbwa)),
        FlightMode::FBWA);

    EXPECT_EQ(
        to_flight_mode_from_custom_mode(
            Autopilot::ArduPilot,
            MAV_TYPE_GROUND_ROVER,
            static_cast<uint32_t>(ardupilot::RoverMode::Hold)),
        FlightMode::Hold);
}
