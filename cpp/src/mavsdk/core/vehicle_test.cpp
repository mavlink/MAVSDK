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

TEST(Vehicle, ToVehicleFromMavTypeExotic)
{
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_COAXIAL), Vehicle::Coaxial);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_AIRSHIP), Vehicle::Airship);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_FREE_BALLOON), Vehicle::FreeBalloon);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_ROCKET), Vehicle::Rocket);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_FLAPPING_WING), Vehicle::FlappingWing);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_KITE), Vehicle::Kite);
    EXPECT_EQ(
        to_vehicle_from_mav_type(MAV_TYPE_VTOL_TAILSITTER_DUOROTOR),
        Vehicle::VtolTailsitterDuorotor);
    EXPECT_EQ(
        to_vehicle_from_mav_type(MAV_TYPE_VTOL_TAILSITTER_QUADROTOR),
        Vehicle::VtolTailsitterQuadrotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_VTOL_FIXEDROTOR), Vehicle::VtolFixedrotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_VTOL_TILTWING), Vehicle::VtolTiltwing);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_PARAFOIL), Vehicle::Parafoil);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_DODECAROTOR), Vehicle::Dodecarotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_DECAROTOR), Vehicle::Decarotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MAV_TYPE_PARACHUTE), Vehicle::Parachute);
}

TEST(Vehicle, StreamOperatorMoreLabels)
{
    std::ostringstream oss;
    oss << Vehicle::Coaxial << "," << Vehicle::Airship << "," << Vehicle::Rocket << ","
        << Vehicle::FlappingWing << "," << Vehicle::Kite << "," << Vehicle::VtolFixedrotor << ","
        << Vehicle::VtolTiltwing << "," << Vehicle::Parafoil << "," << Vehicle::Decarotor << ","
        << Vehicle::GenericMultirotor;
    EXPECT_EQ(
        oss.str(),
        "Coaxial,Airship,Rocket,FlappingWing,Kite,VtolFixedrotor,VtolTiltwing,Parafoil,Decarotor,GenericMultirotor");
}
