#include "mav_type.hpp"
#include "mavlink_include.hpp"

namespace mavsdk {

// MavType is a copy of MAV_TYPE, so that the public headers do not need the MAVLink C
// headers. Make sure the two stay in sync.
static_assert(static_cast<int>(MavType::Generic) == MAV_TYPE_GENERIC);
static_assert(static_cast<int>(MavType::FixedWing) == MAV_TYPE_FIXED_WING);
static_assert(static_cast<int>(MavType::Quadrotor) == MAV_TYPE_QUADROTOR);
static_assert(static_cast<int>(MavType::Coaxial) == MAV_TYPE_COAXIAL);
static_assert(static_cast<int>(MavType::Helicopter) == MAV_TYPE_HELICOPTER);
static_assert(static_cast<int>(MavType::AntennaTracker) == MAV_TYPE_ANTENNA_TRACKER);
static_assert(static_cast<int>(MavType::Gcs) == MAV_TYPE_GCS);
static_assert(static_cast<int>(MavType::Airship) == MAV_TYPE_AIRSHIP);
static_assert(static_cast<int>(MavType::FreeBalloon) == MAV_TYPE_FREE_BALLOON);
static_assert(static_cast<int>(MavType::Rocket) == MAV_TYPE_ROCKET);
static_assert(static_cast<int>(MavType::GroundRover) == MAV_TYPE_GROUND_ROVER);
static_assert(static_cast<int>(MavType::SurfaceBoat) == MAV_TYPE_SURFACE_BOAT);
static_assert(static_cast<int>(MavType::Submarine) == MAV_TYPE_SUBMARINE);
static_assert(static_cast<int>(MavType::Hexarotor) == MAV_TYPE_HEXAROTOR);
static_assert(static_cast<int>(MavType::Octorotor) == MAV_TYPE_OCTOROTOR);
static_assert(static_cast<int>(MavType::Tricopter) == MAV_TYPE_TRICOPTER);
static_assert(static_cast<int>(MavType::FlappingWing) == MAV_TYPE_FLAPPING_WING);
static_assert(static_cast<int>(MavType::Kite) == MAV_TYPE_KITE);
static_assert(static_cast<int>(MavType::OnboardController) == MAV_TYPE_ONBOARD_CONTROLLER);
static_assert(static_cast<int>(MavType::VtolTailsitterDuorotor) == MAV_TYPE_VTOL_TAILSITTER_DUOROTOR);
static_assert(static_cast<int>(MavType::VtolTailsitterQuadrotor) == MAV_TYPE_VTOL_TAILSITTER_QUADROTOR);
static_assert(static_cast<int>(MavType::VtolTiltrotor) == MAV_TYPE_VTOL_TILTROTOR);
static_assert(static_cast<int>(MavType::VtolFixedrotor) == MAV_TYPE_VTOL_FIXEDROTOR);
static_assert(static_cast<int>(MavType::VtolTailsitter) == MAV_TYPE_VTOL_TAILSITTER);
static_assert(static_cast<int>(MavType::VtolTiltwing) == MAV_TYPE_VTOL_TILTWING);
static_assert(static_cast<int>(MavType::VtolReserved5) == MAV_TYPE_VTOL_RESERVED5);
static_assert(static_cast<int>(MavType::Gimbal) == MAV_TYPE_GIMBAL);
static_assert(static_cast<int>(MavType::Adsb) == MAV_TYPE_ADSB);
static_assert(static_cast<int>(MavType::Parafoil) == MAV_TYPE_PARAFOIL);
static_assert(static_cast<int>(MavType::Dodecarotor) == MAV_TYPE_DODECAROTOR);
static_assert(static_cast<int>(MavType::Camera) == MAV_TYPE_CAMERA);
static_assert(static_cast<int>(MavType::ChargingStation) == MAV_TYPE_CHARGING_STATION);
static_assert(static_cast<int>(MavType::Flarm) == MAV_TYPE_FLARM);
static_assert(static_cast<int>(MavType::Servo) == MAV_TYPE_SERVO);
static_assert(static_cast<int>(MavType::Odid) == MAV_TYPE_ODID);
static_assert(static_cast<int>(MavType::Decarotor) == MAV_TYPE_DECAROTOR);
static_assert(static_cast<int>(MavType::Battery) == MAV_TYPE_BATTERY);
static_assert(static_cast<int>(MavType::Parachute) == MAV_TYPE_PARACHUTE);
static_assert(static_cast<int>(MavType::Log) == MAV_TYPE_LOG);
static_assert(static_cast<int>(MavType::Osd) == MAV_TYPE_OSD);
static_assert(static_cast<int>(MavType::Imu) == MAV_TYPE_IMU);
static_assert(static_cast<int>(MavType::Gps) == MAV_TYPE_GPS);
static_assert(static_cast<int>(MavType::Winch) == MAV_TYPE_WINCH);
static_assert(static_cast<int>(MavType::GenericMultirotor) == MAV_TYPE_GENERIC_MULTIROTOR);
static_assert(static_cast<int>(MavType::Illuminator) == MAV_TYPE_ILLUMINATOR);
static_assert(static_cast<int>(MavType::SpacecraftOrbiter) == MAV_TYPE_SPACECRAFT_ORBITER);
static_assert(static_cast<int>(MavType::GroundQuadruped) == MAV_TYPE_GROUND_QUADRUPED);
static_assert(static_cast<int>(MavType::VtolGyrodyne) == MAV_TYPE_VTOL_GYRODYNE);
static_assert(static_cast<int>(MavType::Gripper) == MAV_TYPE_GRIPPER);
static_assert(static_cast<int>(MavType::Radio) == MAV_TYPE_RADIO);
// If this fails, MAV_TYPE got new values which need to be added to MavType.
static_assert(MAV_TYPE_ENUM_END == 50);

std::string_view to_string(MavType mav_type)
{
    switch (mav_type) {
        case MavType::Generic:
            return "Generic";
        case MavType::FixedWing:
            return "FixedWing";
        case MavType::Quadrotor:
            return "Quadrotor";
        case MavType::Coaxial:
            return "Coaxial";
        case MavType::Helicopter:
            return "Helicopter";
        case MavType::AntennaTracker:
            return "AntennaTracker";
        case MavType::Gcs:
            return "Gcs";
        case MavType::Airship:
            return "Airship";
        case MavType::FreeBalloon:
            return "FreeBalloon";
        case MavType::Rocket:
            return "Rocket";
        case MavType::GroundRover:
            return "GroundRover";
        case MavType::SurfaceBoat:
            return "SurfaceBoat";
        case MavType::Submarine:
            return "Submarine";
        case MavType::Hexarotor:
            return "Hexarotor";
        case MavType::Octorotor:
            return "Octorotor";
        case MavType::Tricopter:
            return "Tricopter";
        case MavType::FlappingWing:
            return "FlappingWing";
        case MavType::Kite:
            return "Kite";
        case MavType::OnboardController:
            return "OnboardController";
        case MavType::VtolTailsitterDuorotor:
            return "VtolTailsitterDuorotor";
        case MavType::VtolTailsitterQuadrotor:
            return "VtolTailsitterQuadrotor";
        case MavType::VtolTiltrotor:
            return "VtolTiltrotor";
        case MavType::VtolFixedrotor:
            return "VtolFixedrotor";
        case MavType::VtolTailsitter:
            return "VtolTailsitter";
        case MavType::VtolTiltwing:
            return "VtolTiltwing";
        case MavType::VtolReserved5:
            return "VtolReserved5";
        case MavType::Gimbal:
            return "Gimbal";
        case MavType::Adsb:
            return "Adsb";
        case MavType::Parafoil:
            return "Parafoil";
        case MavType::Dodecarotor:
            return "Dodecarotor";
        case MavType::Camera:
            return "Camera";
        case MavType::ChargingStation:
            return "ChargingStation";
        case MavType::Flarm:
            return "Flarm";
        case MavType::Servo:
            return "Servo";
        case MavType::Odid:
            return "Odid";
        case MavType::Decarotor:
            return "Decarotor";
        case MavType::Battery:
            return "Battery";
        case MavType::Parachute:
            return "Parachute";
        case MavType::Log:
            return "Log";
        case MavType::Osd:
            return "Osd";
        case MavType::Imu:
            return "Imu";
        case MavType::Gps:
            return "Gps";
        case MavType::Winch:
            return "Winch";
        case MavType::GenericMultirotor:
            return "GenericMultirotor";
        case MavType::Illuminator:
            return "Illuminator";
        case MavType::SpacecraftOrbiter:
            return "SpacecraftOrbiter";
        case MavType::GroundQuadruped:
            return "GroundQuadruped";
        case MavType::VtolGyrodyne:
            return "VtolGyrodyne";
        case MavType::Gripper:
            return "Gripper";
        case MavType::Radio:
            return "Radio";
    }
    return "Unknown";
}

std::ostream& operator<<(std::ostream& str, const MavType& mav_type)
{
    return str << to_string(mav_type);
}

} // namespace mavsdk
