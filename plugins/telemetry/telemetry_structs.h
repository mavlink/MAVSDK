#pragma once

#include <ostream>
#include <string>

/**
 * @brief Namespace for all dronecore types.
 */
namespace dronecore {

/**
 * @brief Position type.
 */
struct Position {
    double latitude_deg; /**< @brief Latitude in degrees (range: -90 to +90) */
    double longitude_deg; /**< @brief Longitude in degrees (range: -180 to 180) */
    float absolute_altitude_m; /**< @brief Altitude AMSL (above mean sea level) in metres */
    float relative_altitude_m; /**< @brief Altitude relative to takeoff altitude in metres */
};

bool operator==(const Position &lhs, const Position &rhs);
std::ostream &operator<<(std::ostream &str, Position const &position);

/**
 * @brief Quaternion type.
 *
 * All rotations and axis systems follow the right-hand rule.
 * The Hamilton quaternion product definition is used.
 * A zero-rotation quaternion is represented by (1,0,0,0).
 * The quaternion could also be written as w + xi + yj + zk.
 *
 * For more info see: https://en.wikipedia.org/wiki/Quaternion
 */
struct Quaternion {
    float w; /**< @brief Quaternion entry 0 also denoted as a. */
    float x; /**< @brief Quaternion entry 1 also denoted as b. */
    float y; /**< @brief Quaternion entry 2 also denoted as c. */
    float z; /**< @brief Quaternion entry 3 also denoted as d. */
};

/**
 * @brief Euler angle type.
 *
 * All rotations and axis systems follow the right-hand rule.
 * The Euler angles follow the convention of a 3-2-1 intrinsic Tait-Bryan rotation sequence.
 *
 * For more info see https://en.wikipedia.org/wiki/Euler_angles
 */
struct EulerAngle {
    float roll_deg; /**< @brief Roll angle in degrees, positive is banking to the right. */
    float pitch_deg; /**< @brief Pitch angle in degrees, positive is pitching nose up. */
    float yaw_deg; /**< @brief Yaw angle in degrees, positive is clock-wise seen from above. */
};

/**
 * @brief Ground speed type.
 *
 * The ground speed is represented in the NED (North East Down) frame and in metres/second.
 */
struct GroundSpeedNED {
    float velocity_north_m_s; /**< @brief Velocity in North direction in metres/second. */
    float velocity_east_m_s; /**< @brief Velocity in East direction in metres/second. */
    float velocity_down_m_s; /**< @brief Velocity in Down direction in metres/second. */
};

/**
 * @brief GPS information type.
 */
struct GPSInfo {
    int num_satellites; /**< @brief Number of visible satellites used for solution. */
    int fix_type; /**< @brief Fix type (0: no GPS, 1: no fix, 2: 2D fix, 3: 3D fix, 4: DGPS fix,
                                 5: RTK float, 6: RTK fixed). */
};

/**
 * @brief Battery type.
 */
struct Battery {
    float voltage_v; /**< @brief Voltage in volts. */
    float remaining_percent; /**< @brief Estimated battery percentage remaining (range: 0.0 to 1.0). */
};

/**
 * @brief Flight modes.
 *
 * For more information about flight modes, check out
 * https://docs.px4.io/en/config/flight_mode.html.
 */
enum class FlightMode {
    READY, /**< @brief Armed and ready to take off. */
    TAKEOFF, /**< @brief Taking off. */
    HOLD, /**< @brief Hold mode (hovering in place (or circling for fixed-wing vehicles). */
    MISSION, /**< @brief Mission mode. */
    RETURN_TO_LAUNCH, /**< @brief Returning to launch position (then landing). */
    LAND, /**< @brief Landing. */
    OFFBOARD, /**< @brief Offboard mode. */
    FOLLOW_ME, /**< @brief FollowMe mode. */
    UNKNOWN /**< @brief Mode not known. */
};

/**
 * @brief Get a human readable English string for a flight mode.
 */
std::string flight_mode_str(FlightMode flight_mode);

/**
 * @brief Various health flags.
 */
struct Health {
    bool gyrometer_calibration_ok; /**< @brief true if the gyrometer is calibrated. */
    bool accelerometer_calibration_ok; /**< @brief true if the accelerometer is calibrated. */
    bool magnetometer_calibration_ok; /**< @brief true if the magnetometer is calibrated. */
    bool level_calibration_ok; /**< @brief true if the vehicle has a valid level calibration. */
    bool local_position_ok; /**< @brief true if the local position estimate is good enough to fly in
                                 a position control mode. */
    bool global_position_ok; /**< @brief true if the global position estimate is good enough to fly
                                  in a position controlled mode. */
    bool home_position_ok; /**< @brief true if the home position has been initialized properly. */
};

bool operator==(const Health &lhs, const Health &rhs);
std::ostream &operator<<(std::ostream &str, Health const &health);

/**
 * @brief Remote control status type.
 */
struct RCStatus {
    bool available_once; /**< @brief true if an RC signal has been available once. */
    bool available; /**< @brief true if the RC signal is available now. */
    float signal_strength_percent; /**< @brief Signal strength as a percentage (range: 0 to 100). */
};

} // namespace dronecore
