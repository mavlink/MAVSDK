#include "vehicle.hpp"
#include <gtest/gtest.h>
#include <sstream>

using namespace mavsdk;

TEST(Vehicle, ToVehicleFromMavTypeCommon)
{
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Generic), Vehicle::Generic);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::FixedWing), Vehicle::FixedWing);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Quadrotor), Vehicle::Quadrotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Helicopter), Vehicle::Helicopter);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::GroundRover), Vehicle::GroundRover);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::SurfaceBoat), Vehicle::SurfaceBoat);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Submarine), Vehicle::Submarine);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Hexarotor), Vehicle::Hexarotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Octorotor), Vehicle::Octorotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Tricopter), Vehicle::Tricopter);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::VtolTiltrotor), Vehicle::VtolTiltrotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::GenericMultirotor), Vehicle::GenericMultirotor);
}

TEST(Vehicle, ToVehicleFromMavTypeUnknownFallback)
{
    auto bogus = static_cast<MavType>(250);
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
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Coaxial), Vehicle::Coaxial);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Airship), Vehicle::Airship);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::FreeBalloon), Vehicle::FreeBalloon);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Rocket), Vehicle::Rocket);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::FlappingWing), Vehicle::FlappingWing);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Kite), Vehicle::Kite);
    EXPECT_EQ(
        to_vehicle_from_mav_type(MavType::VtolTailsitterDuorotor), Vehicle::VtolTailsitterDuorotor);
    EXPECT_EQ(
        to_vehicle_from_mav_type(MavType::VtolTailsitterQuadrotor),
        Vehicle::VtolTailsitterQuadrotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::VtolFixedrotor), Vehicle::VtolFixedrotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::VtolTiltwing), Vehicle::VtolTiltwing);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Parafoil), Vehicle::Parafoil);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Dodecarotor), Vehicle::Dodecarotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Decarotor), Vehicle::Decarotor);
    EXPECT_EQ(to_vehicle_from_mav_type(MavType::Parachute), Vehicle::Parachute);
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
