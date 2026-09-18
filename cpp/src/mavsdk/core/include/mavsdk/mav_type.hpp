#pragma once

#include <cstdint>
#include <sstream>
#include <string_view>
#include "mavsdk_export.h"

namespace mavsdk {

/**
 * @brief MAVLink type of a component (MAV_TYPE).
 *
 * The values match the MAVLink MAV_TYPE enum, so they can be used where MAVLink
 * expects a MAV_TYPE.
 */
enum class MavType : uint8_t {
    Generic = 0, /**< @brief Generic micro air vehicle */
    FixedWing = 1, /**< @brief Fixed wing aircraft. */
    Quadrotor = 2, /**< @brief Quadrotor */
    Coaxial = 3, /**< @brief Coaxial helicopter */
    Helicopter = 4, /**< @brief Normal helicopter with tail rotor. */
    AntennaTracker = 5, /**< @brief Ground installation */
    Gcs = 6, /**< @brief Operator control unit / ground control station */
    Airship = 7, /**< @brief Airship, controlled */
    FreeBalloon = 8, /**< @brief Free balloon, uncontrolled */
    Rocket = 9, /**< @brief Rocket */
    GroundRover = 10, /**< @brief Ground rover */
    SurfaceBoat = 11, /**< @brief Surface vessel, boat, ship */
    Submarine = 12, /**< @brief Submarine */
    Hexarotor = 13, /**< @brief Hexarotor */
    Octorotor = 14, /**< @brief Octorotor */
    Tricopter = 15, /**< @brief Tricopter */
    FlappingWing = 16, /**< @brief Flapping wing */
    Kite = 17, /**< @brief Kite */
    OnboardController = 18, /**< @brief Onboard companion controller */
    VtolTailsitterDuorotor = 19, /**< @brief Two-rotor Tailsitter VTOL that additionally uses control surfaces in vertical operation. Note, value previously named MAV_TYPE_VTOL_DUOROTOR. */
    VtolTailsitterQuadrotor = 20, /**< @brief Quad-rotor Tailsitter VTOL using a V-shaped quad config in vertical operation. Note: value previously named MAV_TYPE_VTOL_QUADROTOR. */
    VtolTiltrotor = 21, /**< @brief Tiltrotor VTOL. Fuselage and wings stay (nominally) horizontal in all flight phases. It able to tilt (some) rotors to provide thrust in cruise flight. */
    VtolFixedrotor = 22, /**< @brief VTOL with separate fixed rotors for hover and cruise flight. Fuselage and wings stay (nominally) horizontal in all flight phases. */
    VtolTailsitter = 23, /**< @brief Tailsitter VTOL. Fuselage and wings orientation changes depending on flight phase: vertical for hover, horizontal for cruise. Use more specific VTOL MAV_TYPE_VTOL_TAILSITTER_DUOROTOR or MAV_TYPE_VTOL_TAILSITTER_QUADROTOR if appropriate. */
    VtolTiltwing = 24, /**< @brief Tiltwing VTOL. Fuselage stays horizontal in all flight phases. The whole wing, along with any attached engine, can tilt between vertical and horizontal mode. */
    VtolReserved5 = 25, /**< @brief VTOL reserved 5 */
    Gimbal = 26, /**< @brief Gimbal */
    Adsb = 27, /**< @brief ADSB system */
    Parafoil = 28, /**< @brief Steerable, nonrigid airfoil */
    Dodecarotor = 29, /**< @brief Dodecarotor */
    Camera = 30, /**< @brief Camera */
    ChargingStation = 31, /**< @brief Charging station */
    Flarm = 32, /**< @brief FLARM collision avoidance system */
    Servo = 33, /**< @brief Servo */
    Odid = 34, /**< @brief Open Drone ID. See https://mavlink.io/en/services/opendroneid.html. */
    Decarotor = 35, /**< @brief Decarotor */
    Battery = 36, /**< @brief Battery */
    Parachute = 37, /**< @brief Parachute */
    Log = 38, /**< @brief Log */
    Osd = 39, /**< @brief OSD */
    Imu = 40, /**< @brief IMU */
    Gps = 41, /**< @brief GPS */
    Winch = 42, /**< @brief Winch */
    GenericMultirotor = 43, /**< @brief Generic multirotor that does not fit into a specific type or whose type is unknown */
    Illuminator = 44, /**< @brief Illuminator. An illuminator is a light source that is used for lighting up dark areas external to the system: e.g. a torch or searchlight (as opposed to a light source for illuminating the system itself, e.g. an indicator light). */
    SpacecraftOrbiter = 45, /**< @brief Orbiter spacecraft. Includes satellites orbiting terrestrial and extra-terrestrial bodies. Follows NASA Spacecraft Classification. */
    GroundQuadruped = 46, /**< @brief A generic four-legged ground vehicle (e.g., a robot dog). */
    VtolGyrodyne = 47, /**< @brief VTOL hybrid of helicopter and autogyro. It has a main rotor for lift and separate propellers for forward flight. The rotor must be powered for hover but can autorotate in cruise flight. See: https://en.wikipedia.org/wiki/Gyrodyne */
    Gripper = 48, /**< @brief Gripper */
    Radio = 49, /**< @brief Radio */
};

/**
 * @brief Convert a `MavType` to a string.
 *
 * @return A string representation of the `MavType`.
 */
MAVSDK_PUBLIC std::string_view to_string(MavType mav_type);

/**
 * @brief Stream operator to print information about a `MavType`.
 *
 * @return A reference to the stream.
 */
MAVSDK_PUBLIC std::ostream& operator<<(std::ostream& os, const MavType& mav_type);

} // namespace mavsdk
