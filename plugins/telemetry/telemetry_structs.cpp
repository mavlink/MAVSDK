#include "telemetry_structs.h"

namespace dronecore {

std::string flight_mode_str(FlightMode flight_mode)
{
    switch (flight_mode) {
        case FlightMode::READY:
            return "Ready";
        case FlightMode::TAKEOFF:
            return "Takeoff";
        case FlightMode::HOLD:
            return "Hold";
        case FlightMode::MISSION:
            return "Mission";
        case FlightMode::RETURN_TO_LAUNCH:
            return "Return to launch";
        case FlightMode::LAND:
            return "Land";
        case FlightMode::OFFBOARD:
            return "Offboard";
        case FlightMode::FOLLOW_ME:
            return "FollowMe";
        case FlightMode::UNKNOWN:
        default:
            return "Unknown";
    }
}

bool operator==(const Position &lhs, const Position &rhs)
{
    return lhs.latitude_deg == rhs.latitude_deg
           && lhs.longitude_deg == rhs.longitude_deg
           && lhs.absolute_altitude_m == rhs.absolute_altitude_m
           && lhs.relative_altitude_m == rhs.relative_altitude_m;
}

std::ostream &operator<<(std::ostream &str, Position const &position)
{
    return str << "[lat: " << position.latitude_deg
           << ", lon: " << position.longitude_deg
           << ", abs_alt: " << position.absolute_altitude_m
           << ", rel_alt: " << position.relative_altitude_m
           << "]";
}

bool operator==(const Health &lhs, const Health &rhs)
{
    return lhs.gyrometer_calibration_ok == rhs.gyrometer_calibration_ok
           && lhs.accelerometer_calibration_ok == rhs.accelerometer_calibration_ok
           && lhs.magnetometer_calibration_ok == rhs.magnetometer_calibration_ok
           && lhs.level_calibration_ok == rhs.level_calibration_ok
           && lhs.local_position_ok == rhs.local_position_ok
           && lhs.global_position_ok == rhs.global_position_ok
           && lhs.home_position_ok == rhs.home_position_ok;
}

std::ostream &operator<<(std::ostream &str, Health const &health)
{
    return str << "[gyrometer_calibration_ok: " << health.gyrometer_calibration_ok
           << ", accelerometer_calibration_ok: " << health.accelerometer_calibration_ok
           << ", magnetometer_calibration_ok: " << health.magnetometer_calibration_ok
           << ", level_calibration_ok: " << health.level_calibration_ok
           << ", local_position_ok: " << health.local_position_ok
           << ", global_position_ok: " << health.global_position_ok
           << ", home_position_ok: " << health.home_position_ok << "]";
}

} // namespace dronecore
