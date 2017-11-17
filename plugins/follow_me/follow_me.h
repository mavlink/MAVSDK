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
     * @brief FollowMe Configuration.
     * Reference: https://docs.px4.io/en/advanced_config/parameter_reference.html#follow-target
     */
    struct Config {
        /**
         * @brief Side to follow target from.
         */
        enum class FollowDirection {
            FRONT_RIGHT, /**< @brief Follow from front right. */
            BEHIND, /**< @brief Follow from behind. */
            FRONT, /**< @brief Follow from front. */
            FRONT_LEFT, /**< @brief Follow from front left. */
            NONE
        };
        // Default Configuration
        static const float DEF_HEIGHT_M; /**< @brief Default height (in meters) above home. */
        static const float DEF_FOLLOW_DIST_M; /**< @brief Default Follow distance (in meters) to target. */
        static const FollowDirection DEF_FOLLOW_DIR; /**< @brief Default side to follow target from. */
        static const float DEF_RESPONSIVENSS; /**< @brief Default responsiveness to target movement. */

        // Min & Max ranges as per PX4 FollowMe Configuration.
        static const float MIN_HEIGHT_M; /**< @brief Min follow target height, in meters. */
        static const float MIN_FOLLOW_DIST_M;  /**< @brief Min distance to follow target from, in meters. */
        static const float MIN_RESPONSIVENESS; /**< @brief Min responsiveness to target movement. Range (0.0 to 1.0) */
        static const float MAX_RESPONSIVENESS; /**< @brief Max responsiveness to target movement. Range (0.0 to 1.0) */

        float min_height_m = DEF_HEIGHT_M; /**< @brief Default follow target height in meters. */
        float follow_dist_m = DEF_FOLLOW_DIST_M; /**< @brief Default distance to follow target from. */
        FollowDirection follow_dir =
            FollowDirection::BEHIND; /**< @brief Follow target from behind by default. */
        float responsiveness = DEF_RESPONSIVENSS; /**< @brief Default responsiveness to target movement. */

        /**
         * @brief Human-readable string of enum `FollowDirection`.
         * @param dir follow direction
         * @return std::string representing given direction
         */
        static std::string to_str(FollowDirection dir);
    };

    /**
     * @brief Gets current FollowMe configuration.
     * @return current FollowMe configuration.
     * @sa set_config(const Config &)
     */
    const Config &get_config() const;

    /**
     * @brief Applies FollowMe configuration by sending them to device.
     * @param config FollowMe configuration to be applied.
     * @return true if configuration is applied successfully, false if config values are out-of-range.
     *         In case of failure, last configuration is preserved.
     * @sa get_config()
     */
    bool set_config(const Config &config);

#ifdef FOLLOW_ME_TESTING
    /**
     * @brief Follow info which will be emitted periodically to the Vehicle
     */
    struct FollowInfo {
        double lat = 47.3977418; /**< @brief Latitude */
        double lon = 8.5455938; /**< @brief Longitude */
        double alt = 488.04; /**< @brief Altitude */

        float vx_ms = 0.f; /**< @brief X Velocity in NED frame in m/s */
        float vy_ms = 0.f; /**< @brief Y Velocity in NED frame in m/s */
        float vz_ms = 0.f; /**< @brief Z Velocity in NED frame in m/s */

        float ax_ms2 = 0.f; /**< @brief X Acceleration  in NED frame in m/s2 or NAN */
        float ay_ms2 = 0.f; /**< @brief Y Acceleration  in NED frame in m/s2 or NAN */
        float az_ms2 = 0.f; /**< @brief Z Acceleration  in NED frame in m/s2 or NAN */

        float pos_std_dev[3] = {}; /**< @brief epv, epv values */
    };

    /**
     * @brief Gets current follow information
     * @return read-only reference to current follow info
     * @sa set_follow_info(const FollowInfo&)
     */
    const FollowInfo &get_follow_info() const;

    /**
     * @brief Sets follow information
     * @param follow info to be set
     * @sa get_follow_info()
     */
    void set_follow_info(const FollowInfo &info);
#endif

    /**
     * @brief Return English string for FollowMe error codes.
     *
     * @param result FollowMe::Result code
     * @return Returns std::string describing error code
     */
    static std::string result_str(Result result);

    /**
     * @brief Starts FollowMe mode
     * @return Result::SUCCESS if succeeded, error otherwise. See FollowMe::Result for error codes.
     * @sa stop()
     */
    FollowMe::Result start() const;

    /**
     * @brief Stops FollowMe mode
     * @return Result::SUCCESS if succeeded, error otherwise. See FollowMe::Result for error codes.
     * @sa start()
     */
    FollowMe::Result stop() const;

    // Non-copyable
    FollowMe(const FollowMe &) = delete;
    const FollowMe &operator=(const FollowMe &) = delete;

private:
    // Underlying implementation, set at instantiation
    FollowMeImpl *_impl;
};

} // namespace dronecore
