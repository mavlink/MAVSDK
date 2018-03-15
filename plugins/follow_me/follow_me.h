#pragma once

#include <string>
#include <memory>
#include <iostream>
#include <cmath>
#include <functional>
#include "plugin_base.h"

namespace dronecore {

class FollowMeImpl;
class System;

/**
 * @brief This class enables vehicle tracking of a specified target (typically a ground station carried by a user).
 *
 * The API is used to supply both the position(s) for the target and the relative follow position of the vehicle.
 * Applications must get target position information from the underlying platform (or some other source).
 *
 * @sa [Follow Me Mode](https://docs.px4.io/en/flight_modes/follow_me.html) (PX4 User Guide)
 *
 */
class FollowMe : public PluginBase
{
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto follow_me = std::make_shared<FollowMe>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit FollowMe(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~FollowMe();

    /**
     * @brief Results of FollowMe operations.
     * @sa result_str()
     */
    enum class Result {
        SUCCESS = 0, /**< @brief Request succeeded. */
        NO_SYSTEM, /**< @brief No system connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        BUSY, /**< @brief Vehicle busy. */
        COMMAND_DENIED, /**< @brief Command denied. */
        TIMEOUT, /**< @brief Request timeout. */
        NOT_ACTIVE, /**< @brief FollowMe is not activated. */
        SET_CONFIG_FAILED, /**< @brief Failed to set FollowMe configuration. */
        UNKNOWN /**< @brief Unknown error. */
    };

    /**
     * @brief FollowMe Configuration.
     * @sa get_config(), set_config()
     * @sa [Parameter Reference](https://docs.px4.io/en/advanced_config/parameter_reference.html#follow-target) (PX4 User Guide)
     */
    struct Config {
        /**
         * @brief Relative position of following vehicle.
         * @sa to_str()
         */
        enum class FollowDirection {
            FRONT_RIGHT, /**< @brief Follow from front right. */
            BEHIND, /**< @brief Follow from behind. */
            FRONT, /**< @brief Follow from front. */
            FRONT_LEFT, /**< @brief Follow from front left. */
            NONE
        };

        constexpr static const float MIN_HEIGHT_M = 8.0f; /**< @brief Min follow height, in meters. */
        constexpr static const float MIN_FOLLOW_DIST_M =
            1.0f; /**< @brief Min follow distance, in meters. */
        constexpr static const float MIN_RESPONSIVENESS = 0.f; /**< @brief Min responsiveness. */
        constexpr static const float MAX_RESPONSIVENESS = 1.0f; /**< @brief Max responsiveness. */

        float min_height_m = 8.0f; /**< @brief Min follow height, in meters. */
        float follow_distance_m = 8.0f; /**< @brief Horizontal follow distance to target, in meters. */
        FollowDirection follow_direction =
            FollowDirection::BEHIND; /**< @brief Relative position of the following vehicle. */
        float responsiveness = 0.5f; /**< @brief Responsiveness, Range (0.0-1.0) */

        /**
         * @brief Human-readable string for enum #FollowDirection.
         * @param[in] direction Follow direction
         * @return std::string representing given direction
         */
        static std::string to_str(FollowDirection direction);
    };

    /**
     * @brief Gets current FollowMe configuration.
     * @return Current FollowMe configuration.
     * @sa set_config()
     */
    const Config &get_config() const;

    /**
     * @brief Applies FollowMe configuration by sending it to system.
     * @param[in] config FollowMe configuration to be applied.
     * @return FollowMe::Result::SUCCESS if configuration is applied successfully,
     *         FollowMe::Result::SET_CONFIG_FAILED on failure.
     *         In case of failure, last configuration is preserved.
     * @sa get_config()
     */
    Result set_config(const Config &config);

    /**
     * @brief Checks whether FollowMe is active.
     * @return `true` if FollowMe is active, `false` otherwise.
     */
    bool is_active() const;

    /**
     * @brief Type for geographical location of the target being followed.
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
     * @brief Sets location of the moving target.
     *
     * An app can obtain the location of the moving target from Location framework of the underlying platform.
     *
     * @note
     * The following links provide information about location services on different platforms:
     * - Android - https://developer.android.com/reference/android/location/Location.html
     * - Apple - https://developer.apple.com/documentation/corelocation
     * - Windows - https://docs.microsoft.com/en-us/uwp/api/Windows.Devices.Geolocation
     *
     * @param[in] location Location of the moving target.
     * @sa get_last_location()
     */
    void set_target_location(const TargetLocation &location);

    /**
     * @brief Returns the last location of the target.
     * @return Last location of the target.
     * @sa set_target_location()
     */
    const TargetLocation &get_last_location() const;

    /**
     * @brief Returns English string for FollowMe error codes
     *
     * @param[in] result #Result code.
     * @return Returns std::string describing error code.
     */
    static std::string result_str(Result result);

    /**
     * @brief Starts FollowMe mode.
     * @return FollowMe::Result::SUCCESS if succeeded, error otherwise.
     */
    FollowMe::Result start() const;

    /**
     * @brief Stops FollowMe mode.
     * @return FollowMe::Result::SUCCESS if succeeded, error otherwise. See FollowMe::Result for error codes.
     */
    FollowMe::Result stop() const;


    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    FollowMe(const FollowMe &) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const FollowMe &operator=(const FollowMe &) = delete;

private:
    // Underlying implementation, set at instantiation
    std::unique_ptr<FollowMeImpl> _impl;
};

} // namespace dronecore
