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
     * @brief FollowMe configuration.
     * Reference: https://docs.px4.io/en/advanced_config/parameter_reference.html#follow-target
     */
    class Configuration
    {
    public:
        /**
         * @brief Side to follow target from
         */
        enum class FollowDirection {
            NONE = -1,
            FRONT_RIGHT = 0, /**< @brief Follow from front right. */
            BEHIND = 1, /**< @brief Follow from behind. */
            FRONT = 2, /**< @brief Follow from front. */
            FRONT_LEFT = 3 /**< @brief Follow from front left. */
        };

        // Default configuration
        ///////////////////////////////////////////////////////////
        /**< @brief Default follow target height. */
        static constexpr const float DEF_HEIGHT_ABOVE_HOME = 8.0f;
        /**< @brief Min follow target height. */
        static constexpr const float MIN_HEIGHT_ABOVE_HOME = 8.0f;

        /**< @brief Distance to follow target from. */
        static constexpr const float DEF_DIST_WRT_FT = 8.0f;
        /**< @brief Min distance to follow target from. */
        static constexpr const float MIN_DIST_WRT_FT = 1.0f;

        /**< @brief Side to follow target from. */
        static constexpr const FollowDirection DEF_FOLLOW_DIR = FollowDirection::BEHIND;

        /**< @brief Default dynamic filtering algorithm responsiveness to target movement. */
        static constexpr const float DEF_DYN_FLT_ALG_RSP = 0.5f;
        /**< @brief Min dynamic filtering algorithm responsiveness to target movement. */
        static constexpr const float MIN_DYN_FLT_ALG_RSP = 0.0f;

        /**< @brief Max dynamic filtering algorithm responsiveness to target movement. */
        static constexpr const float MAX_DYN_FLT_ALG_RSP = 1.0f;
        ///////////////////////////////////////////////////////////

        // FollowMe Configuration Accessors.
        /**
         * @brief Gets minimum follow target height, in meters.
         * @return Minimum follow target height.
         * @sa set_min_height_m(float)
         */
        float min_height_m() const;

        /**
         * @brief Gets distance to follow target from, in meters.
         * @return Current distance to follow target from, in meters.
         * @sa set_follow_target_dist_m(float)
         */
        float follow_target_dist_m() const;

        /**
         * @brief Gets Side to follow target from.
         * @return %FollowDirection representing Side to follow target from.
         * @sa set_follow_dir(FollowDirection), to_str(FollowDirection)
         */
        FollowDirection follow_dir() const;

        /**
         * @brief String form of %FollowDirection
         * @param dir enum %FollowDirection
         * @return std::string representation of enum %FollowDirection
         * @sa follow_dir(), set_follow_dir(FollowDirection)
         */
        static std::string to_str(FollowDirection dir);

        /**
         * @brief Gets current dynamic filtering algorithm responsiveness value.
         * @return Current responsiveness value
         * @sa set_responsiveness(float)
         */
        float responsiveness() const;

        // Methods that configure FollowMe behavior.
        /**
         * @brief Sets minimum follow target height, in meters.
         * @param mht: min height in meters relative to home for following a target.
         * @return true (success) if given height >= 8.0 meters, false (failure) otherwise.
         * @sa min_height_m()
         */
        bool set_min_height_m(float mht);

        /**
         * @brief Sets distance in meters to follow target from.
         * @param ft_dst distance in meters to follow the target at.
         * @return true (success) if given distance >= 1.0 meters, false (failure) otherwise.
         * @sa follow_target_dist_m()
         */
        bool set_follow_target_dist_m(float ft_dst);

        /**
         * @brief Sets the side to follow target from.
         * @param dir Side to follow target from.
         * @sa follow_dir(), to_str(FollowDirection)
         */
        void set_follow_dir(FollowDirection dir);

        /**
         * @brief Sets dynamic filtering algorithm responsiveness to target movement.
         * Lower numbers increase the responsiveness to changing long lat but also ignore less noise.
         * @param responsiveness
         * @return true (success) if responsiveness is in range (0.0 to 1.0), false (failure) otherwise.
         * @sa responsiveness()
         */
        bool set_responsiveness(float resp);

    private:
        float _min_height_m = 8.0f; // Default & Minimum follow target height in meters.
        float _follow_target_dist_m = 8.0f; // Default Distance to follow target from.
        FollowDirection _follow_dir = FollowDirection::BEHIND; // Follow target from behind by default.
        float _dyn_flt_alg_responsiveness = 0.5f; // Default responsiveness to target movement.
    };

    /**
     * @brief Gets current FollowMe configuration.
     * @return current FollowMe configuration.
     * @sa set_config(const Configuration &)
     */
    Configuration get_config() const;

    /**
     * @brief Applies FollowMe configuration by sending them to device.
     * @param config FollowMe configuration to be applied.
     * @sa get_config()
     */
    void set_config(const Configuration &config);

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
