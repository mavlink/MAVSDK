#pragma once

#include <iostream>
#include <cmath>

/**
  IMPORTANT NOTE:
    Macro FOLLOW_ME_TESTING is used to test FollowMe plugin.
    In real scenario, GCS (DroneCore Application) doesn't provide poistion,
    but instead, current location of the device is captured by platform-specific Location Framework.

  */
#ifndef FOLLOW_ME_TESTING
#define FOLLOW_ME_TESTING
#endif

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

#ifdef FOLLOW_ME_TESTING
    static const double DEF_LAT; /**< @brief Default latitude of GCS */
    static const double DEF_LONG; /**< @brief Default longitude of GCS */
    static const double DEF_ALT; /**< @brief Default altitude of GCS */

    /**
     * @brief GCS Position
     */
    struct GCSPosition {
        double lat = DEF_LAT; /**< @brief Latitude of GCS device */
        double lon = DEF_LONG; /**< @brief Longitude of GCS device */
        double alt = DEF_ALT; /**< @brief Altitude of GCS device */
        /**
         * @brief Constructor of GCSPosition
         * @param _lat latitude set by application
         * @param _lon longtitude set by application
         * @param _alt altitude set by application
         */
        GCSPosition(double _lat, double _lon, double _alt) : lat(_lat), lon(_lon), alt(_alt) {}
    };
#endif

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

#ifdef FOLLOW_ME_TESTING
    /**
     * @brief Starts FollowMe mode with position
     * @return Result::SUCCESS if succeeded, error otherwise. See FollowMe::Result for error codes.
     */
    FollowMe::Result start(const GCSPosition &gcs_pos);
#endif

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
