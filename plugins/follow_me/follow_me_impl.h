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
    In real scenario, GCS (DroneCore Application) doesn't set Follow info,
    but instead, current location of the device is captured by platform-specific Location Framework.
 */

namespace dronecore {

class FollowMeImpl : public PluginImplBase
{
public:
    FollowMeImpl();
    ~FollowMeImpl();

    void init() override;
    void deinit() override;

    const FollowMe::Config &get_config() const;
    bool set_config(const FollowMe::Config &config);

    const FollowMe::FollowInfo &get_follow_info() const;
    void set_follow_info(const FollowMe::FollowInfo &info);


    FollowMe::Result start();
    FollowMe::Result stop();
private:
    void process_heartbeat(const mavlink_message_t &message);
    void follow_info_handler();
    void send_follow_info();
    bool is_config_ok(const FollowMe::Config &config) const;
    void rcv_param_min_height(bool success, float min_height_m);
    void rcv_param_follow_distance(bool success, float distance);
    void rcv_param_follow_direction(bool success, int32_t dir);
    void rcv_param_responsiveness(bool success, float rsp);
    FollowMe::Result to_follow_me_result(MavlinkCommands::Result result) const;
    void update_follow_info();
#ifdef FOLLOW_ME_TESTING
    void update_fake_follow_info();
    int _spiral_idx;
#endif

    enum class ESTCapabilities {
        POS,
        VEL,
        ACCEL,
        ATT_RATES
    };

    Time _time;
    void *_ce_cookie;
    // required for emitting follow target updates to Vehicle
    FollowMe::FollowInfo _follow_info;
    CallEveryHandler _follow_info_timer;
    uint8_t _estimatation_capabilities;
    // holds followme configuration
    FollowMe::Config _config;
    bool _followme_mode_active;
};

} // namespace dronecore
