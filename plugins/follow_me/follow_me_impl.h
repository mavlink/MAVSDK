#pragma once

#include "follow_me.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "device_impl.h"
#include "timeout_handler.h"
#include "global_include.h"

namespace dronecore {

class FollowMeImpl : public PluginImplBase
{
public:
    FollowMeImpl();
    ~FollowMeImpl();

    void init() override;
    void deinit() override;

    FollowMe::Result start();
    FollowMe::Result stop();
private:
    /* private methods */
    /*****************************************************/
    void process_heartbeat(const mavlink_message_t &message);
    FollowMe::Result to_follow_me_result(MavlinkCommands::Result result) const;
    void timeout_occurred();
    void send_gcs_motion_report();
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
#ifdef FOLLOW_ME_TESTING
        constexpr static const double DEF_LATITUDE = 47.3977436;
        constexpr static const double DEF_LONGITUDE = 8.5455864;
        constexpr static const double DEF_ALTITUDE =  488.27;
#endif

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

        std::array<float, 3> pos_std_dev;

        /**
         * @brief MotionReport
         * @param _lat
         * @param _lon
         * @param _alt
         */
#ifdef FOLLOW_ME_TESTING
        MotionReport(double _lat = DEF_LATITUDE, double _lon = DEF_LONGITUDE,
                     double _alt = DEF_ALTITUDE)
#else
        MotionReport(double _lat, double _lon, double _alt)
#endif

        {
            lat_int = _lat * 1e7;
            lon_int = _lon * 1e7;
            alt = static_cast<float>(_alt);
            vx = vy = vz = 0.f;
            afx = afy = afz = 0.f;
            pos_std_dev.fill(0);
        }
    };

    dl_time_t _start_time = steady_time();
    uint8_t _estimatation_capabilities = 0;

    // required for emitting MotionReport updates to Vehicle
    MotionReport _motion_report;
    CallEveryHandler _motion_report_timer;
    void *_timeout_cookie = nullptr;

    bool _followme_mode_active = false;
};

} // namespace dronecore
