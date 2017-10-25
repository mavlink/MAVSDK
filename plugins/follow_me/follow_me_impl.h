#pragma once

#include "follow_me.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "device_impl.h"
#include "timeout_handler.h"

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
    void process_heartbeat(const mavlink_message_t &message);
    FollowMe::Result get_result_from_mavcmd_result(MavlinkCommands::Result result) const;
    void timeout_occurred();

    struct motion_report {
        uint32_t timestamp;     // time since boot
        int32_t lat_int;        // X Position in WGS84 frame in 1e7 * meters
        int32_t lon_int;        // Y Position in WGS84 frame in 1e7 * meters
        float alt;              //	Altitude in meters in AMSL altitude, not WGS84 if absolute or relative, above terrain if GLOBAL_TERRAIN_ALT_INT
        float vx;               //	X velocity in NED frame in meter / s
        float vy;               //	Y velocity in NED frame in meter / s
        float vz;               //	Z velocity in NED frame in meter / s
        float afx;              //	X acceleration in NED frame in meter / s^2 or N
        float afy;              //	Y acceleration in NED frame in meter / s^2 or N
        float afz;              //	Z acceleration in NED frame in meter / s^2 or N
        float pos_std_dev[3];   // -1 for unknown
    } _motion_report;
    TimeoutHandler _motion_report_timer;
    void *_timeout_cookie = nullptr;
    std::chrono::system_clock::time_point _start;
    uint8_t _estimatation_capabilities;
    void send_gcs_motion_report();

    bool _followme_mode_active;
};

} // namespace dronecore
