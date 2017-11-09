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
            FRONT_RIGHT = 0, /**< @brief Follow from front right */
            BEHIND = 1, /**< @brief Follow from behind */
            FRONT = 2, /**< @brief Follow from front */
            FRONT_LEFT = 3 /**< @brief Follow from front left */
        };

        // Default configuration
        ///////////////////////////////////////////////////////////
        static constexpr const float DEF_HEIGHT_ABOVE_HOME =
            8.0f; /**< @brief Default follow target altitude */
        static constexpr const float MIN_HEIGHT_ABOVE_HOME = 8.0f; /**< @brief Min follow target altitude */

        static constexpr const float DEF_DIST_WRT_FT = 8.0f; /**< @brief Distance to follow target from */
        static constexpr const float MIN_DIST_WRT_FT =
            1.0f; /**< @brief Min distance to follow target from */

        static constexpr const FollowDirection DEF_FOLLOW_DIR =
            FollowDirection::BEHIND; /**< @brief Side to follow target from */
        static constexpr const float DEF_DYN_FLT_ALG_RSP =
            0.5f; /**< @brief Default Dynamic filtering algorithm responsiveness to target movement */
        static constexpr const float MIN_DYN_FLT_ALG_RSP =
            0.0f; /**< @brief Min Dynamic filtering algorithm responsiveness to target movement */
        static constexpr const float MAX_DYN_FLT_ALG_RSP =
            1.0f; /**< @brief Max Dynamic filtering algorithm responsiveness to target movement */
        ///////////////////////////////////////////////////////////

        // FollowMe Configuration Accessors
        /**
         * @brief min_height_m
         * @return
         */
        float min_height_m() const
        {
            return _min_height_m;
        }
        /**
         * @brief follow_target_distance_m
         * @return
         */
        float follow_target_dist_m() const
        {
            return _follow_target_dist_m;
        }
        /**
         * @brief follow_dir
         * @return
         */
        FollowDirection follow_dir() const
        {
            return _follow_dir;
        }

        /**
         * @brief dynamic_filter_alg_responsiveness
         * @return
         */
        float dynamic_filter_alg_responsiveness() const
        {
            return _dyn_flt_alg_responsiveness;
        }


        // Methods that configure FollowMe behavior
        /**
         * @brief set_min_height
         * @param mht
         * @return
         */
        bool set_min_height_m(float mht)
        {
            if (mht < MIN_HEIGHT_ABOVE_HOME) {
                return false;
            }
            _min_height_m = mht;
            return true;
        }

        /**
         * @brief set_follow_target_dist
         * @param ft_dst
         * @return
         */
        bool set_follow_target_dist_m(float ft_dst)
        {
            if (ft_dst < MIN_DIST_WRT_FT) {
                return false;
            }
            _follow_target_dist_m = ft_dst;
            return true;
        }

        /**
         * @brief set_follow_dir
         * @param dir
         */
        void set_follow_dir(FollowDirection dir)
        {
            _follow_dir = dir;
        }

        /**
         * @brief set_dynamic_filter_algo_rsp_val
         * @param responsiveness
         * @return
         */
        bool set_dynamic_filter_algo_rsp_val(float responsiveness)
        {
            if (responsiveness < 0.f || responsiveness > 1.0f) {
                return false;
            }
            _dyn_flt_alg_responsiveness = responsiveness;
            return true;
        }

    private:
        float _min_height_m = 8.0f; // Default & Minimum follow target altitude in meters
        float _follow_target_dist_m = 8.0f; // Default Distance to follow target from
        FollowDirection _follow_dir = FollowDirection::BEHIND; // Follow target from behind by default
        float _dyn_flt_alg_responsiveness =
            0.5f; // Dynamic filtering algorithm responsiveness to target movement
    };

    /**
     * @brief Gets current FollowMe configuration
     * @return
     */
    Configuration get_config() const;

    /**
     * @brief Sets FollowMe configuration
     * @param config
     */
    void set_config(const Configuration &config);

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
     * @brief Stops FollowMe mode
     * @return Result::SUCCESS if succeeded, error otherwise. See FollowMe::Result for error codes.
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
