#include "vehicle.h"

namespace mavsdk {

std::ostream& operator<<(std::ostream& str, const Vehicle& vehicle)
{
    switch (vehicle) {
        case Vehicle::Generic:
            return str << "Generic";
        case Vehicle::FixedWing:
            return str << "FixedWing";
        case Vehicle::Quadrotor:
            return str << "Quadrotor";
        case Vehicle::Coaxial:
            return str << "Coaxial";
        case Vehicle::Helicopter:
            return str << "Helicopter";
        case Vehicle::Airship:
            return str << "Airship";
        case Vehicle::FreeBalloon:
            return str << "FreeBalloon";
        case Vehicle::Rocket:
            return str << "Rocket";
        case Vehicle::GroundRover:
            return str << "GroundRover";
        case Vehicle::SurfaceBoat:
            return str << "SurfaceBoat";
        case Vehicle::Submarine:
            return str << "Submarine";
        case Vehicle::Hexarotor:
            return str << "Hexarotor";
        case Vehicle::Octorotor:
            return str << "Octorotor";
        case Vehicle::Tricopter:
            return str << "Tricopter";
        case Vehicle::FlappingWing:
            return str << "FlappingWing";
        case Vehicle::Kite:
            return str << "Kite";
        case Vehicle::VtolTailsitterDuorotor:
            return str << "VtolTailsitterDuorotor";
        case Vehicle::VtolTailsitterQuadrotor:
            return str << "VtolTailsitterQuadrotor";
        case Vehicle::VtolTiltrotor:
            return str << "VtolTiltrotor";
        case Vehicle::VtolFixedrotor:
            return str << "VtolFixedrotor";
        case Vehicle::VtolTailsitter:
            return str << "VtolTailsitter";
        case Vehicle::VtolTiltwing:
            return str << "VtolTiltwing";
        case Vehicle::Parafoil:
            return str << "Parafoil";
        case Vehicle::Dodecarotor:
            return str << "Dodecarotor";
        case Vehicle::Decarotor:
            return str << "Decarotor";
        case Vehicle::Parachute:
            return str << "Parachute";
        case Vehicle::GenericMultirotor:
            return str << "GenericMultirotor";
        default:
            return str << "Unknown";
    }
}

Vehicle to_vehicle_from_mav_type(MAV_TYPE type)
{
    switch (type) {
        case MAV_TYPE::MAV_TYPE_GENERIC:
            return Vehicle::Generic;
        case MAV_TYPE::MAV_TYPE_FIXED_WING:
            return Vehicle::FixedWing;
        case MAV_TYPE::MAV_TYPE_QUADROTOR:
            return Vehicle::Quadrotor;
        case MAV_TYPE::MAV_TYPE_COAXIAL:
            return Vehicle::Coaxial;
        case MAV_TYPE::MAV_TYPE_HELICOPTER:
            return Vehicle::Helicopter;
        case MAV_TYPE::MAV_TYPE_AIRSHIP:
            return Vehicle::Airship;
        case MAV_TYPE::MAV_TYPE_FREE_BALLOON:
            return Vehicle::FreeBalloon;
        case MAV_TYPE::MAV_TYPE_ROCKET:
            return Vehicle::Rocket;
        case MAV_TYPE::MAV_TYPE_GROUND_ROVER:
            return Vehicle::GroundRover;
        case MAV_TYPE::MAV_TYPE_SURFACE_BOAT:
            return Vehicle::SurfaceBoat;
        case MAV_TYPE::MAV_TYPE_SUBMARINE:
            return Vehicle::Submarine;
        case MAV_TYPE::MAV_TYPE_HEXAROTOR:
            return Vehicle::Hexarotor;
        case MAV_TYPE::MAV_TYPE_OCTOROTOR:
            return Vehicle::Octorotor;
        case MAV_TYPE::MAV_TYPE_TRICOPTER:
            return Vehicle::Tricopter;
        case MAV_TYPE::MAV_TYPE_FLAPPING_WING:
            return Vehicle::FlappingWing;
        case MAV_TYPE::MAV_TYPE_KITE:
            return Vehicle::Kite;
        case MAV_TYPE::MAV_TYPE_VTOL_TAILSITTER_DUOROTOR:
            return Vehicle::VtolTailsitterDuorotor;
        case MAV_TYPE::MAV_TYPE_VTOL_TAILSITTER_QUADROTOR:
            return Vehicle::VtolTailsitterQuadrotor;
        case MAV_TYPE::MAV_TYPE_VTOL_TILTROTOR:
            return Vehicle::VtolTiltrotor;
        case MAV_TYPE::MAV_TYPE_VTOL_FIXEDROTOR:
            return Vehicle::VtolFixedrotor;
        case MAV_TYPE::MAV_TYPE_VTOL_TAILSITTER:
            return Vehicle::VtolTailsitter;
        case MAV_TYPE::MAV_TYPE_VTOL_TILTWING:
            return Vehicle::VtolTiltwing;
        case MAV_TYPE::MAV_TYPE_PARAFOIL:
            return Vehicle::Parafoil;
        case MAV_TYPE::MAV_TYPE_DODECAROTOR:
            return Vehicle::Dodecarotor;
        case MAV_TYPE::MAV_TYPE_DECAROTOR:
            return Vehicle::Decarotor;
        case MAV_TYPE::MAV_TYPE_PARACHUTE:
            return Vehicle::Parachute;
        case MAV_TYPE::MAV_TYPE_GENERIC_MULTIROTOR:
            return Vehicle::GenericMultirotor;
        default:
            return Vehicle::Unknown;
    }
}

} // namespace mavsdk
