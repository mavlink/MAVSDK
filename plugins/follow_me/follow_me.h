#pragma once

#include <iostream>
#include <cmath>
#include <functional>

namespace dronecore {

class FollowMeImpl;

/**
 * @brief This class enables vehicle tracking of a specified target (typically a ground station carried by a user).
 *
 * The API is used to supply both the position(s) for the target and the relative follow position of the vehicle.
 * Applications must get target position information from the underlying platform (or some other source).
 *
 * See https://docs.px4.io/en/flight_modes/follow_me.html for more information.
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
     * @brief Results of FollowMe operations.
     */
    enum class Result {
        SUCCESS = 0, /**< @brief Request succeeded. */
        NO_DEVICE, /**< @brief No device connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        BUSY, /**< @brief Vehicle busy. */
        COMMAND_DENIED, /**< @brief Command denied. */
        TIMEOUT, /**< @brief Request timeout. */
        NOT_ACTIVE, /**< @brief FollowMe is not activated. */
        UNKNOWN /**< @brief Unknown error. */
    };

    /**
     * @brief FollowMe Configuration.
     * Reference: https://docs.px4.io/en/advanced_config/parameter_reference.html#follow-target
     */
    struct Config {
        /**
         * @brief Relative position of following vehicle.
         */
        enum class FollowDirection {
            FRONT_RIGHT, /**< @brief Follow from front right. */
            BEHIND, /**< @brief Follow from behind. */
            FRONT, /**< @brief Follow from front. */
            FRONT_LEFT, /**< @brief Follow from front left. */
            NONE
        };

        constexpr static const float DEFAULT_HEIGHT_M =
            8.0; /**< @brief Default height above home, in meters. */
        constexpr static const float DEFAULT_FOLLOW_DIST_M =
            8.0; /**< @brief Default follow distance, in meters. */
        constexpr static const FollowDirection DEFAULT_FOLLOW_DIR =
            FollowDirection::BEHIND; /**< @brief Default follow direction. */
        constexpr static const float DEFAULT_RESPONSIVENSS =
            0.5f; /**< @brief Default responsiveness to target. */

        constexpr static const float MIN_HEIGHT_M = 8.0f; /**< @brief Min follow height, in meters. */
        constexpr static const float MIN_FOLLOW_DIST_M =
            1.0f; /**< @brief Min follow distance from, in meters. */
        constexpr static const float MIN_RESPONSIVENESS = 0.f; /**< @brief Min responsiveness. */
        constexpr static const float MAX_RESPONSIVENESS = 1.0f; /**< @brief Max responsiveness. */

        float min_height_m = DEFAULT_HEIGHT_M; /**< @brief Min follow height above home, in meters. */
        float follow_dist_m =
            DEFAULT_FOLLOW_DIST_M; /**< @brief Horizontal distance b/w vehicle & target. */
        FollowDirection follow_direction =
            DEFAULT_FOLLOW_DIR; /**< @brief Relative position of the following vehicle. */
        float responsiveness = DEFAULT_RESPONSIVENSS; /**< @brief Responsiveness: Range (0.0 to 1.0)*/

        /**
         * @brief Human-readable string for enum `FollowDirection`.
         * @param direction Follow direction
         * @return std::string representing given direction
         */
        static std::string to_str(FollowDirection direction);
    };

    /**
     * @brief Gets current FollowMe configuration.
     * @return Current FollowMe configuration.
     * @sa set_config(const Config &)
     */
    const Config &get_config() const;

    /**
     * @brief Applies FollowMe configuration by sending it to device.
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
     * @brief Geographical location of the target in motion.
     */
    struct TargetLocation {
        double latitude_deg; /**< @brief Latitude, in degrees */
        double longitude_deg; /**< @brief Longitude, in degrees */
        double absolute_altitude_m; /**< @brief AMSL, in meters */

        float velocity_x_m_s; /**< @brief X-velocity in m/s */
        float velocity_y_m_s; /**< @brief Y-velocity in m/s */
        float velocity_z_m_s; /**< @brief Z-velocity in m/s */
    };

    /**
     * @brief Sets current location of the moving target
     * @note App can obtain location of the moving target from Location framework of the underlying platform.
     * The following links provide information about location services on different platforms:
     *
     * Android - https://developer.android.com/reference/android/location/Location.html
     *
     * Apple - https://developer.apple.com/documentation/corelocation
     *
     * Windows - https://docs.microsoft.com/en-us/uwp/api/Windows.Devices.Geolocation
     *
     * @param location Current location of the target.
     */
    void set_curr_target_location(const TargetLocation &location);

    /**
     * @brief Returns the most recent location of the target.
     * @param last_location Last location to be filled.
     * @sa set_curr_target_location()
     */
    void get_last_location(TargetLocation &prev_location);

    /**
     * @brief Returns English string for FollowMe error codes.
     *
     * @param result FollowMe::Result code.
     * @return Returns std::string describing error code.
     */
    static std::string result_str(Result result);

    /**
     * @brief Starts FollowMe mode.
     * @return Result::SUCCESS if succeeded, error otherwise.
     * @sa stop(), `FollowMe::Result`
     */
    FollowMe::Result start() const;

    /**
     * @brief Stops FollowMe mode.
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
