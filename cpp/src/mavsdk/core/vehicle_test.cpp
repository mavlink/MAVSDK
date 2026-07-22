#include "vehicle.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace mavsdk;

TEST(Vehicle, ToVehicleFromMavTypeCommon)
{
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_GENERIC), Vehicle::Generic);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_FIXED_WING), Vehicle::FixedWing);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_QUADROTOR), Vehicle::Quadrotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_HELICOPTER), Vehicle::Helicopter);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_GROUND_ROVER), Vehicle::GroundRover);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_SURFACE_BOAT), Vehicle::SurfaceBoat);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_SUBMARINE), Vehicle::Submarine);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_HEXAROTOR), Vehicle::Hexarotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_OCTOROTOR), Vehicle::Octorotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_TRICOPTER), Vehicle::Tricopter);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_VTOL_TILTROTOR), Vehicle::VtolTiltrotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_GENERIC_MULTIROTOR), Vehicle::GenericMultirotor);
}

TEST(Vehicle, ToVehicleFromMavTypeUnknownFallback)
{
    auto bogus = static_cast<MAV_TYPE>(250);
    EXPECT_EQ(to_vehicle_from_mav_type(bogus), Vehicle::Unknown);
}

TEST(Vehicle, StreamOperatorKnownAndUnknown)
{
    {
        std::ostringstream oss;
        oss << Vehicle::Quadrotor;
        EXPECT_EQ(oss.str(), "Quadrotor");
    }
    {
        std::ostringstream oss;
        oss << Vehicle::FixedWing;
        EXPECT_EQ(oss.str(), "FixedWing");
    }
    {
        std::ostringstream oss;
        oss << Vehicle::Unknown;
        EXPECT_EQ(oss.str(), "Unknown");
    }
    {
        std::ostringstream oss;
        oss << static_cast<Vehicle>(250);
        EXPECT_EQ(oss.str(), "Unknown");
    }
}

TEST(Vehicle, VtolAndExoticStream)
{
    std::ostringstream oss;
    oss << Vehicle::VtolTailsitter << "," << Vehicle::Parachute << "," << Vehicle::Dodecarotor;
    EXPECT_EQ(oss.str(), "VtolTailsitter,Parachute,Dodecarotor");
}
