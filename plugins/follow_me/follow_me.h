#pragma once

#include <iostream>
#include <cmath>
#include <functional>

namespace dronecore {

class FollowMeImpl;

/**
 * @brief FollowMe plugin is used to make your drone to follow your Ground Control Station (Phone/Tab). <br>
 *
 * Application need to obtain current GCS position from underying platform,
 * and feed to it FollowMe, which makes your drone to move.
 * When GCS keeps moving to a new position, position updates are sent to the drone
 * accordingly; this results in drone movement relative to that of GCS.
 *
 * Please refer https://docs.px4.io/en/flight_modes/follow_me.html for more information.
 *
 */
class FollowMe
{
public:
    /**
     * @brief Constructor (internal use only).
     * @param impl Private internal implementation.
     */
    explicit FollowMe(FollowMeImpl *impl);

    /**
     * @brief Destructor (internal use only).
     */
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
        NOT_ACTIVE, /**< @brief FollowMe is not activated yet */
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
        static const float DEF_HEIGHT_M; /**< @brief Default height above home: 8.0 mts */
        static const float DEF_FOLLOW_DIST_M; /**< @brief Default Follow distance: 8.0 mts */
        static const FollowDirection DEF_FOLLOW_DIR; /**< @brief Default follow direction: BEHIND */
        static const float DEF_RESPONSIVENSS; /**< @brief Default responsiveness to target: 0.5 */

        // Min & Max ranges as per PX4 FollowMe Configuration.
        static const float MIN_HEIGHT_M; /**< @brief Min follow height, in mts. >= 8.0m */
        static const float MIN_FOLLOW_DIST_M;  /**< @brief Min follow distance from, in mts. >= 1.0m */
        static const float MIN_RESPONSIVENESS; /**< @brief Min responsiveness: 0.0 */
        static const float MAX_RESPONSIVENESS; /**< @brief Max responsiveness: 1.0 */

        float min_height_m = DEF_HEIGHT_M; /**< @brief Min follow height above home */
        float follow_dist_m =
            DEF_FOLLOW_DIST_M; /**< @brief Follow horizontal distance b/w Vehicle & GCS. */
        FollowDirection follow_dir = DEF_FOLLOW_DIR; /**< @brief Side to follow target from. */
        float responsiveness = DEF_RESPONSIVENSS; /**< @brief Responsiveness: Range (0.0 to 1.0)*/

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

    /**
     * @brief Checks whether FollowMe is active.
     * @return true if FollowMe is active, false otherwise.
     */
    bool is_active() const;

    /**
     * @brief Represents geographic location of the target in motion
     */
    struct TargetLocation {
        double lattitude_deg; /**< @brief Latitude */
        double longitude_deg; /**< @brief Longitude */
        double absolute_altitude_m; /**< @brief AMSL, in meters */

        float velocity_x_mps; /**< @brief X-velocity in m/s2 */
        float velocity_y_mps; /**< @brief Y-velocity in m/s2 */
        float velocity_z_mps; /**< @brief Z-velocity in m/s2 */
    };

    /**
     * @brief Sets current location of the moving target
     * @note App can obtain location of the moving target from Location framework of the underlying platform. <br>
     * Below is a hint to Apps to avail services available on their underlying platforms. <br>
     * Android - https://developer.android.com/reference/android/location/Location.html <br>
     * Apple - https://developer.apple.com/documentation/corelocation <br>
     * Windows - https://docs.microsoft.com/en-us/uwp/api/Windows.Devices.Geolocation <br>
     * @param pos current location of the target
     * @return Result::SUCCESS when location is set,
     *         Result::NOT_STARTED_YET when you set the location before starting FollowMe.
     */
    Result set_curr_target_location(const TargetLocation &location);

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
