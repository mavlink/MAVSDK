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

    const FollowMe::Config &get_config() const;
    bool set_config(const FollowMe::Config &config);

    void register_follow_target_info_callback(FollowMe::follow_target_info_callback_t callback);
    void deregister_follow_target_info_callback();

    FollowMe::Result start();
    FollowMe::Result stop();
private:
    void process_heartbeat(const mavlink_message_t &message);

    void follow_target_info_handler();
    void update_follow_target_info();
    void reset_follow_target_info();
    void send_follow_target_info();

    void load_device_config();
    bool is_config_ok(const FollowMe::Config &config) const;
    void receive_param_min_height(bool success, float min_height_m);
    void receive_param_follow_distance(bool success, float distance);
    void receive_param_follow_direction(bool success, int32_t dir);
    void receive_param_responsiveness(bool success, float rsp);
    FollowMe::Result to_follow_me_result(MavlinkCommands::Result result) const;

    enum class ESTCapabilities {
        POS,
        VEL,
        ACCEL,
        ATT_RATES
    };

    Time _time;
    void *_ce_cookie;
    // required for emitting follow target updates to Vehicle
    FollowMe::FollowTargetInfo _follow_target_info;
    FollowMe::follow_target_info_callback_t _follow_target_info_cb;
    CallEveryHandler _follow_target_info_timer;
    uint8_t _estimatation_capabilities;
    // holds followme configuration
    FollowMe::Config _config;
    bool _followme_mode_active;
};

} // namespace dronecore
