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
    FollowMe::Result stop() const;
#ifdef FOLLOW_ME_TESTING
    FollowMe::Result start(const FollowMe::GCSPosition &gcs_pos);
    void set_gcs_position(const FollowMe::GCSPosition &gcs_pos);
#endif

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

        float pos_std_dev[3];
#ifdef FOLLOW_ME_TESTING
        /* modifiers */
        void set_position(const FollowMe::GCSPosition &gcs_pos);
#endif
    };

    // required for emitting MotionReport updates to Vehicle
    MotionReport _motion_report {};
    dl_time_t _start_time;
    uint8_t _estimatation_capabilities;

    TimeoutHandler _motion_report_timer;
    void *_timeout_cookie = nullptr;

    bool _followme_mode_active;
};

} // namespace dronecore
