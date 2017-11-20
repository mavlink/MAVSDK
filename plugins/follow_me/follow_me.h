#pragma once

#include <iostream>
#include <cmath>
#include <functional>

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
        static const float DEF_HEIGHT_M; /**< @brief Default height above home: 8.0 mts */
        static const float DEF_FOLLOW_DIST_M; /**< @brief Default Follow distance: 8.0 mts */
        static const FollowDirection DEF_FOLLOW_DIR; /**< @brief Default follow direction: BEHIND */
        static const float DEF_RESPONSIVENSS; /**< @brief Default responsiveness to target: 0.5 */

        // Min & Max ranges as per PX4 FollowMe Configuration.
        static const float MIN_HEIGHT_M; /**< @brief Min follow height, in mts. >= 8.0m */
        static const float MIN_FOLLOW_DIST_M;  /**< @brief Min follow distance from, in mts. >= 1.0m */
        static const float MIN_RESPONSIVENESS; /**< @brief Min responsiveness: 0.0 */
        static const float MAX_RESPONSIVENESS; /**< @brief Max responsiveness: 1.0 */

        float min_height_m; /**< @brief Min follow height above home */
        float follow_dist_m; /**< @brief Follow horizontal distance b/w Vehicle & GCS. */
        FollowDirection follow_dir; /**< @brief Side to follow target from. */
        float responsiveness; /**< @brief Responsiveness: Range (0.0 to 1.0)*/

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
     * @brief Follow info which will be emitted periodically to the Vehicle.
     */
    struct FollowTargetInfo {
        double lat; /**< @brief Latitude */
        double lon; /**< @brief Longitude */
        double alt; /**< @brief Altitude */

        float vx_ms; /**< @brief X Velocity in NED frame in m/s */
        float vy_ms; /**< @brief Y Velocity in NED frame in m/s */
        float vz_ms; /**< @brief Z Velocity in NED frame in m/s */

        float ax_ms2; /**< @brief X Acceleration  in NED frame in m/s2 or NAN */
        float ay_ms2; /**< @brief Y Acceleration  in NED frame in m/s2 or NAN */
        float az_ms2; /**< @brief Z Acceleration  in NED frame in m/s2 or NAN */

        float pos_std_dev[3];/**< @brief epv, epv values */
    };

    /**
     * @brief Callback that is called by FollowMe plugin to get `FollowTargetinfo`.
     * @param follow target info to be filled by callback
     */
    typedef std::function<void (FollowTargetInfo &)> follow_target_info_callback_t;

    /**
     * @brief Registers follow target information callback. This callback is called periodically
     * by FollowMe plugin. Make sure callback fills proper Follow target info.
     * It is optional to register this callback.
     * If registered, plugin will get follow target info from callback.
     * If not registered, plugin will use platform-specific framework to get follow target info.
     * @param Application callback that fills follow target info
     * @sa deregister_follow_target_info_callback()
     */
    void register_follow_target_info_callback(follow_target_info_callback_t callback);

    /**
     * @brief De-registers follow target info callback, if registered earlier.
     * @sa register_follow_target_info_callback(follow_target_info_callback_t)
     */
    void deregister_follow_target_info_callback();

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
