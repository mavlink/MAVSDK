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

    void enable() override;
    void disable() override;

    const FollowMe::Config &get_config() const;
    FollowMe::Result set_config(const FollowMe::Config &config);

    void set_curr_target_location(const FollowMe::TargetLocation &location);
    void get_last_location(FollowMe::TargetLocation &last_location);

    bool is_active() const;

    FollowMe::Result start();
    FollowMe::Result stop();

private:
    void process_heartbeat(const mavlink_message_t &message);

    // Config methods
    void set_default_config();
    bool is_config_ok(const FollowMe::Config &config) const;
    void receive_param_min_height(bool success, float min_height_m);
    void receive_param_follow_distance(bool success, float distance);
    void receive_param_follow_direction(bool success, int32_t direction);
    void receive_param_responsiveness(bool success, float rsp);
    FollowMe::Result to_follow_me_result(MavlinkCommands::Result result) const;

    bool is_current_location_set() const;
    void send_curr_target_location();
    void stop_sending_target_location();

    enum class EstimationCapabilites {
        POS,
        VEL
    };

    enum class Mode {
        NOT_ACTIVE,
        ACTIVE
    } _mode = Mode::NOT_ACTIVE;

    mutable std::mutex _mutex {};
    FollowMe::TargetLocation _curr_target_location; // sent to vehicle
    FollowMe::TargetLocation _last_location; // sent to vehicle
    void *_curr_target_location_cookie = nullptr;

    Time _time {};
    uint8_t _estimatation_capabilities = 0; // sent to vehicle
    FollowMe::Config _config {}; // has FollowMe configuration settings

    const float SENDER_RATE = 1.0f; // send location updates once in a second
};

} // namespace dronecore
