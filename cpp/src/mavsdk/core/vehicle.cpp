#include "vehicle.hpp"

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

Vehicle to_vehicle_from_mav_type(MavType type)
{
    switch (type) {
        case MavType::Generic:
            return Vehicle::Generic;
        case MavType::FixedWing:
            return Vehicle::FixedWing;
        case MavType::Quadrotor:
            return Vehicle::Quadrotor;
        case MavType::Coaxial:
            return Vehicle::Coaxial;
        case MavType::Helicopter:
            return Vehicle::Helicopter;
        case MavType::Airship:
            return Vehicle::Airship;
        case MavType::FreeBalloon:
            return Vehicle::FreeBalloon;
        case MavType::Rocket:
            return Vehicle::Rocket;
        case MavType::GroundRover:
            return Vehicle::GroundRover;
        case MavType::SurfaceBoat:
            return Vehicle::SurfaceBoat;
        case MavType::Submarine:
            return Vehicle::Submarine;
        case MavType::Hexarotor:
            return Vehicle::Hexarotor;
        case MavType::Octorotor:
            return Vehicle::Octorotor;
        case MavType::Tricopter:
            return Vehicle::Tricopter;
        case MavType::FlappingWing:
            return Vehicle::FlappingWing;
        case MavType::Kite:
            return Vehicle::Kite;
        case MavType::VtolTailsitterDuorotor:
            return Vehicle::VtolTailsitterDuorotor;
        case MavType::VtolTailsitterQuadrotor:
            return Vehicle::VtolTailsitterQuadrotor;
        case MavType::VtolTiltrotor:
            return Vehicle::VtolTiltrotor;
        case MavType::VtolFixedrotor:
            return Vehicle::VtolFixedrotor;
        case MavType::VtolTailsitter:
            return Vehicle::VtolTailsitter;
        case MavType::VtolTiltwing:
            return Vehicle::VtolTiltwing;
        case MavType::Parafoil:
            return Vehicle::Parafoil;
        case MavType::Dodecarotor:
            return Vehicle::Dodecarotor;
        case MavType::Decarotor:
            return Vehicle::Decarotor;
        case MavType::Parachute:
            return Vehicle::Parachute;
        case MavType::GenericMultirotor:
            return Vehicle::GenericMultirotor;
        default:
            return Vehicle::Unknown;
    }
}

} // namespace mavsdk
