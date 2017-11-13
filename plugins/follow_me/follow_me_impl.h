#pragma once

#include "follow_me.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "device_impl.h"
#include "timeout_handler.h"
#include "global_include.h"

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

class FollowMeImpl : public PluginImplBase
{
public:
    FollowMeImpl();
    ~FollowMeImpl();

    void init() override;
    void deinit() override;

    FollowMe::Configuration get_config() const;
    void set_config(const FollowMe::Configuration &cfg);

    FollowMe::Result start();
    FollowMe::Result stop();
private:
    /* private methods */
    /*****************************************************/
    void process_heartbeat(const mavlink_message_t &message);
    FollowMe::Result to_follow_me_result(MavlinkCommands::Result result) const;
    void timeout_occurred();
    void send_gcs_motion_report();

    void receive_param_min_height(bool success, float min_height_m);
    void receive_param_follow_target_dist(bool success, float ft_dist_m);
    void receive_param_follow_dir(bool success, int32_t dir);
    void receive_param_dyn_fltr_alg_rsp(bool success, float rsp);
    /*****************************************************/

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
     * @brief Motion report info which will be emitted periodically to the Vehicle
     */
    struct MotionReport {
        // GCS Position
        int32_t lat_int;
        int32_t lon_int;
        float alt;
        // Velocity in NED frame in meter / s
        float vx;
        float vy;
        float vz;
        // Acceleration in NED frame in meter/s^2 or NAN
        float afx;
        float afy;
        float afz;
        // for eph, epv
        float pos_std_dev[3];

        /**
         * @brief MotionReport
         * @param _lat
         * @param _lon
         * @param _alt
         */
#ifdef FOLLOW_ME_TESTING
        // Initial GCS position is set to PX4 Home
        constexpr static const double DEF_LATITUDE = 47.3977436;
        constexpr static const double DEF_LONGITUDE = 8.5455864;
        constexpr static const double DEF_ALTITUDE =  488.27;
        // Constructor
        MotionReport(double _lat = DEF_LATITUDE, double _lon = DEF_LONGITUDE,
                     double _alt = DEF_ALTITUDE): vx(0.f), vy(0.f), vz(0.f), afx(0.f), afy(0.f), afz(0.f), pos_std_dev{}
#else
        MotionReport(double _lat, double _lon, double _alt): vx(0.f), vy(0.f), vz(0.f), afx(0.f), afy(0.f),
            afz(0.f), pos_std_dev {}
#endif
        {
            lat_int = _lat * 1e7;
            lon_int = _lon * 1e7;
            alt = static_cast<float>(_alt);
        }
    };

    Time _time;
    void *_timeout_cookie;
    // required for emitting MotionReport updates to Vehicle
    MotionReport _motion_report;
    CallEveryHandler _motion_report_timer;
    uint8_t _estimatation_capabilities;
    // holds followme configuration
    FollowMe::Configuration _config;
    bool _followme_mode_active;
};

} // namespace dronecore
