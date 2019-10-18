#pragma once

#include <functional>
#include <memory>
#include <string>
#include <array>
#include <limits>

#include "plugin_base.h"

namespace mavsdk {

class MocapImpl;
class System;

/**
 * @brief This class allows users to get vehicle mocap and state information
 * (e.g. battery, GPS, RC connection, flight mode etc.) and set mocap update rates.
 */
class Mocap : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto mocap = std::make_shared<Mocap>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Mocap(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Mocap();

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
    union Quaternion {
        struct {
            float w; /**< @brief Quaternion entry 0 also denoted as a. */
            float x; /**< @brief Quaternion entry 1 also denoted as b. */
            float y; /**< @brief Quaternion entry 2 also denoted as c. */
            float z; /**< @brief Quaternion entry 3 also denoted as d. */
        };

        Quaternion() : a{0.0f} {}; /**< @brief Constructor */
        Quaternion(float val) : a{val} {}; /**< @brief Constructor */
        Quaternion(float w_, float x_, float y_, float z_) :
            w(w_),
            x(x_),
            y(y_),
            z(z_){}; /**< @brief Constructor */

        float operator[](int i) const { return a.at(i); }; /**< @brief Operator [] */
        float& operator[](int i) { return a.at(i); } /**< @brief Operator &[] */

        /**
         * @brief Cast to float C-style float array
         * */
        operator float*() { return a.data(); };
        /**
         * @brief Cast to float array
         * */
        operator std::array<float, 4>&() { return a; };
        /**
         * @brief Cast to float array
         * */
        operator std::array<float, 4>() { return a; };

    private:
        std::array<float, 4> a;
    };

    /**
     * @brief Covariance type
     */
    typedef std::array<float, 21> Covariance;

    /**
     * @brief Body position type
     */
    struct PositionBody {
        float x_m; /**< @brief X position in metres. */
        float y_m; /**< @brief Y position in metres. */
        float z_m; /**< @brief Z position in metres. */
    };

    /**
     * @brief Body angle type
     */
    struct AngleBody {
        float roll_rad; /**< @brief Roll angle in radians. */
        float pitch_rad; /**< @brief Pitch angle in radians. */
        float yaw_rad; /**< @brief Yaw angle in radians. */
    };

    /**
     * @brief Speed type, represented in the Body (X Y Z) frame and in metres/second.
     */
    struct SpeedBody {
        float x_m_s; /**< @brief Velocity in X in metres/second. */
        float y_m_s; /**< @brief Velocity in Y in metres/second. */
        float z_m_s; /**< @brief Velocity in Z in metres/second. */
    };

    /**
     * @brief Angular velocity type
     */
    struct AngularVelocityBody {
        float roll_rad_s; /**< @brief Roll angular velocity in radians/second. */
        float pitch_rad_s; /**< @brief Pitch angular velocity in radians/second. */
        float yaw_rad_s; /**< @brief Yaw angular velocity in radians/second. */
    };

    /**
     * @brief Global position/attitude estimate from a vision source type.
     */
    struct VisionPositionEstimate {
        uint64_t time_usec; /**< @brief  Position frame timestamp UNIX Epoch time. */
        PositionBody position_body; /**< @brief Global Body Position. (m). */
        AngleBody angle_body; /**< @brief Body angle (rad). */
        Covariance pose_covariance; /**< @brief Row-major representation of pose 6x6
                                       ross-covariance matrix upper right triangle. */
        uint8_t reset_counter; /**< @brief Estimate reset counter. */
    };

    /**
     * @brief Motion capture attitude and position type.
     */
    struct AttitudePositionMocap {
        uint64_t time_usec; /**< @brief  Position frame timestamp UNIX Epoch time. */
        Quaternion q; /**< @brief Attitude quaternion (w, x, y, z order,
                         zero-rotation is 1, 0, 0, 0) */
        PositionBody position_body; /**< @brief Body position (NED). */
        Covariance pose_covariance; /**< @brief Row-major representation of pose 6x6
                                       cross-covariance matrix upper right triangle. */
    };

    /**
     * @brief Odometry information with an external interface type.
     */
    struct Odometry {
        /**
         * @brief Mavlink frame id
         */
        enum class MavFrame {
            MOCAP_NED = 14, /**< @brief Odometry local coordinate frame of data given
                               by a motion capture system, Z-down (x: north, y: east, z: down). */
            LOCAL_FRD = 20, /**< @brief  Forward, Right, Down coordinate frame. This is a local
                               frame with Z-down and arbitrary F/R alignment (i.e. not
                               aligned with NED/earth frame). Replacement for
                               MAV_FRAME_MOCAP_NED, MAV_FRAME_VISION_NED, MAV_FRAME_ESTIM_NED. */
        };

        uint64_t time_usec{}; /**< @brief Timestamp (0 to use Backend timestamp). */
        MavFrame frame_id{}; /**< @brief Coordinate frame of reference for the pose data. */
        PositionBody position_body{}; /**< @brief Body position. */
        Quaternion q{}; /**< @brief Quaternion components, w, x, y, z
                         (1 0 0 0 is the null-rotation). */
        SpeedBody speed_body{}; /**< @brief Body linear speed (m/s). */
        AngularVelocityBody angular_velocity_body{}; /**< @brief Body angular speed (rad/s). */
        Covariance pose_covariance{std::numeric_limits<float>::quiet_NaN()}; /**< @brief Row-major
                                       representation of a 6x6 pose cross-covariance matrix upper
                                       right triangle. Leave empty if unknown. */
        Covariance velocity_covariance{
            std::numeric_limits<float>::quiet_NaN()}; /**< @brief Row-major representation of a 6x6
                                                         velocity cross-covariance matrix upper
                                                         right triangle. Leave empty if unknown. */
    };

    /**
     * @brief Results enum for mocap requests.
     */
    enum class Result {
        UNKNOWN = 0, /**< @brief Unknown error. */
        SUCCESS, /**< @brief %Request succeeded. */
        NO_SYSTEM, /**< @brief No system connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        INVALID_REQUEST_DATA /**< @brief Invalid request data */
    };

    /**
     * @brief Get human-readable English string for Mocap::Result.
     *
     * @param result The enum value for which string is needed.
     * @return Human readable string for the Mocap::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Set the vision position.
     *
     * @param vision_position_estimate Position `struct`. Set time_usec to 0 to use internal
     * timestamp.
     */
    Result set_vision_position_estimate(VisionPositionEstimate vision_position_estimate);

    /**
     * @brief Set the motion capture attitude and position.
     *
     * @param attitude_position_mocap Position `struct`. Set time_usec to 0 to use internal
     * timestamp.
     */
    Result set_attitude_position_mocap(AttitudePositionMocap attitude_position_mocap);

    /**
     * @brief Set the Odometry information.
     *
     * @param odometry Odometry `struct`. Set time_usec to 0 to use internal timestamp.
     */
    Result set_odometry(const Odometry& odometry);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Mocap(const Mocap&) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Mocap& operator=(const Mocap&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<MocapImpl> _impl;
};

/**
 * @brief Equal operator to compare two `Mocap::Quaternion` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Mocap::Quaternion& lhs, const Mocap::Quaternion& rhs);

/**
 * @brief Stream operator to print information about a `Mocap::Quaternion`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Mocap::Quaternion const& quaternion);

/**
 * @brief Equal operator to compare two `Mocap::VisionPositionEstimate` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Mocap::VisionPositionEstimate& lhs, const Mocap::VisionPositionEstimate& rhs);

/**
 * @brief Stream operator to print information about a `Mocap::VisionPositionEstimate`.
 *
 * @return A reference to the stream.
 */
std::ostream&
operator<<(std::ostream& str, Mocap::VisionPositionEstimate const& vision_position_estimate);

/**
 * @brief Equal operator to compare two `Mocap::AttitudePositionMocap` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Mocap::AttitudePositionMocap& lhs, const Mocap::AttitudePositionMocap& rhs);

/**
 * @brief Stream operator to print information about a `Mocap::AttitudePositionMocap`.
 *
 * @return A reference to the stream.
 */
std::ostream&
operator<<(std::ostream& str, Mocap::AttitudePositionMocap const& attitude_position_mocap);

/**
 * @brief Equal operator to compare two `Mocap::Odometry` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Mocap::Odometry& lhs, const Mocap::Odometry& rhs);

/**
 * @brief Stream operator to print information about a `Mocap::Odometry`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Mocap::Odometry const& odometry);

/**
 * @brief Equal operator to compare two `Mocap::PositionBody` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Mocap::PositionBody& lhs, const Mocap::PositionBody& rhs);

/**
 * @brief Non Equal operator to compare two `Mocap::PositionBody` objects.
 *
 * @return `true` if items are not equal.
 */
bool operator!=(const Mocap::PositionBody& lhs, const Mocap::PositionBody& rhs);

/**
 * @brief Stream operator to print information about a `Mocap::PositionBody`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Mocap::PositionBody const& position_body);

/**
 * @brief Equal operator to compare two `Mocap::AngleBody` objects.
 *
 * @return `true` if items are not equal.
 */
bool operator==(const Mocap::AngleBody& lhs, const Mocap::AngleBody& rhs);

/**
 * @brief Non Equal operator to compare two `Mocap::AngleBody` objects.
 *
 * @return `true` if items are not equal.
 */
bool operator!=(const Mocap::AngleBody& lhs, const Mocap::AngleBody& rhs);

/**
 * @brief Stream operator to print information about a `Mocap::AngleBody`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Mocap::AngleBody const& angle_body);

/**
 * @brief Equal operator to compare two `Mocap::SpeedBody` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Mocap::SpeedBody& lhs, const Mocap::SpeedBody& rhs);

/**
 * @brief Non Equal operator to compare two `Mocap::SpeedBody` objects.
 *
 * @return `true` if items are not equal.
 */
bool operator!=(const Mocap::SpeedBody& lhs, const Mocap::SpeedBody& rhs);

/**
 * @brief Stream operator to print information about a `Mocap::SpeedBody`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Mocap::SpeedBody const& speed_body);

/**
 * @brief Equal operator to compare two `Mocap::Covariance` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Mocap::Covariance& lhs, const Mocap::Covariance& rhs);

/**
 * @brief Non Equal operator to compare two `Mocap::Covariance` objects.
 *
 * @return `true` if items are not equal.
 */
bool operator!=(const Mocap::Covariance& lhs, const Mocap::Covariance& rhs);

/**
 * @brief Stream operator to print information about a `Mocap::SpeedBody`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Mocap::Covariance const& covariance);

/**
 * @brief Equal operator to compare two `Telemetry::AngularVelocityBody` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Mocap::AngularVelocityBody& lhs, const Mocap::AngularVelocityBody& rhs);

/**
 * @brief Stream operator to print information about a `Telemetry::AngularVelocityBody`.
 *
 * @return A reference to the stream.
 */
std::ostream&
operator<<(std::ostream& str, Mocap::AngularVelocityBody const& angular_velocity_body);

} // namespace mavsdk
