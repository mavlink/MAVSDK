#pragma once

#include <sstream>
#include "mavlink_include.h"

namespace mavsdk {

/**
 * @brief Vehicle type
 */
enum class Vehicle {
    Unknown, // Vehicle unknown
    Generic, // Generic micro air vehicle
    FixedWing, // Fixed wing aircraft
    Quadrotor, // Quadrotor
    Coaxial, // Coaxial helicopter
    Helicopter, // Normal helicopter with tail rotor
    Airship, // Airship, controlled
    FreeBalloon, // Free balloon, uncontrolled
    Rocket, // Rocket
    GroundRover, // Ground rover
    SurfaceBoat, // Surface boat
    Submarine, // Submarine
    Hexarotor, // Hexarotor
    Octorotor, // Octorotor
    Tricopter, // Tricopter
    FlappingWing, // Flapping wing
    Kite, // Kite
    VtolTailsitterDuorotor, // Two-rotor Tailsitter VTOL
    VtolTailsitterQuadrotor, // Quad-rotor Tailsitter VTOL
    VtolTiltrotor, // Tiltrotor VTOL
    VtolFixedrotor, // VTOL with separate fixed rotors
    VtolTailsitter, // Tailsitter VTOL
    VtolTiltwing, // Tiltwing VTOL
    Parafoil, // Steerable nonrigid airfoil
    Dodecarotor, // Dodecarotor
    Decarotor, // Decarotor
    Parachute, // Parachute
    GenericMultirotor, // Generic multirotor
};

/**
 * @brief Stream operator to print information about a `Vehicle`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& os, const Vehicle& vehicle);

/**
 * @brief Convert a 'MAV_TYPE' to a `Vehicle`.
 *
 * @return The corresponding `Vehicle`.
 */
Vehicle to_vehicle_from_mav_type(MAV_TYPE type);

} // namespace mavsdk
