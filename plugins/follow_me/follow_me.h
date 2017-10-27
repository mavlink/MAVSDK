#pragma once

#include <iostream>
#include <cmath>

namespace dronecore {

class FollowMeImpl;

class FollowMe
{
public:
    explicit FollowMe(FollowMeImpl *impl);
    ~FollowMe();

    /**
     * @brief Results of FollowMe operations
     */
    enum class Result {
        SUCCESS = 0, /**< @brief %Request succeeded. */
        NO_DEVICE, /**< @brief No device connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        BUSY, /**< @brief Vehicle busy. */
        COMMAND_DENIED, /**< @brief Command denied. */
        TIMEOUT, /**< @brief %Request timeout. */
        UNKNOWN /**< @brief Unknown error. */
    };

    /**
     * @brief Altitude flag for Follow Target command.
     */
    enum class ESTCapabilities {
        POS = 0,
        VEL = 1,
        ACCEL = 2,
        ATT_RATES = 3
    };

    /**
     * @brief Motion report info which will be emitted peridically to the Vehicle
     */
    struct MotionReport {
        // set to home position used by PX4.
        int32_t lat_int = static_cast<uint32_t>(47.3977419 * 1e7);        // X Position in WGS84 frame in 1e7 * meters
        int32_t lon_int = static_cast<uint32_t>(8.5455938 * 1e7);        // Y Position in WGS84 frame in 1e7 * meters
        float alt = 488.03f;              //	Altitude in meters in AMSL altitude
        // velocity
        float vx = cos(0.13) * 5.0;               //	X velocity in NED frame in meter / s
        float vy = sin(0.13) * 5.0;              //	Y velocity in NED frame in meter / s
        float vz = 0;               //	Z velocity in NED frame in meter / s
        // acceleration
        float afx;              //	X acceleration in NED frame in meter / s^2 or N
        float afy;              //	Y acceleration in NED frame in meter / s^2 or N
        float afz;              //	Z acceleration in NED frame in meter / s^2 or N

        float pos_std_dev[3] = { 0.8f, 0.8f, 0.f };   // -1 for unknown
    };

    /**
     * @brief Return English string for FollowMe error codes
     *
     * @param result FollowMe::Result code
     * @returns returns std::string describing error code
     */
    static std::string result_str(Result result);

    /**
     * @brief Starts FollowMe mode
     * @return Result::SUCCESS if succeeded, error otherwise. See FollowMe::Result for error codes.
     */
    FollowMe::Result start() const;

    /**
     * @brief Starts FollowMe mode with position
     * @return Result::SUCCESS if succeeded, error otherwise. See FollowMe::Result for error codes.
     */
    FollowMe::Result start(const MotionReport &mr);

    /**
     * @brief Stops FollowMe mode
     * @return Result::SUCCESS if succeeded, error otherwise. See FollowMe::Result for error codes.
     */
    FollowMe::Result stop() const;

    /**
     * @brief Sets position of the FollowMe mode
     * @return none
     */
    void set_motion_report(const MotionReport &mr);

    // Non-copyable
    FollowMe(const FollowMe &) = delete;
    const FollowMe &operator=(const FollowMe &) = delete;

private:
    // Underlying implementation, set at instantiation
    FollowMeImpl *_impl;
};

} // namespace dronecore
