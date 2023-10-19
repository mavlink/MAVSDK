#pragma once

#include <cstdint>
#include "mavlink_include.h"
#include "autopilot.h"

namespace mavsdk {

enum class FlightMode {
    Unknown,
    FBWA,
    Autotune,
    Guided,
    Ready,
    Takeoff,
    Hold,
    Mission,
    ReturnToLaunch,
    Land,
    Offboard,
    FollowMe,
    Manual,
    Altctl,
    Posctl,
    Acro,
    Rattitude,
    Stabilized,
};

FlightMode
to_flight_mode_from_custom_mode(Autopilot autopilot, MAV_TYPE mav_type, uint32_t custom_mode);
FlightMode to_flight_mode_from_px4_mode(uint32_t custom_mode);
FlightMode to_flight_mode_from_ardupilot_rover_mode(uint32_t custom_mode);
FlightMode to_flight_mode_from_ardupilot_copter_mode(uint32_t custom_mode);
FlightMode to_flight_mode_from_ardupilot_plane_mode(uint32_t custom_mode);

} // namespace mavsdk
