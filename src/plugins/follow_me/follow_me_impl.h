#pragma once

#include "plugins/follow_me/follow_me.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "timeout_handler.h"
#include "global_include.h"
#include "log.h"

namespace dronecode_sdk {

class FollowMeImpl : public PluginImplBase {
public:
    FollowMeImpl(System &system);
    ~FollowMeImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    const FollowMe::Config &get_config() const;
    FollowMe::Result set_config(const FollowMe::Config &config);

    void set_target_location(const FollowMe::TargetLocation &location);
    const FollowMe::TargetLocation &get_last_location() const;

    bool is_active() const;

    FollowMe::Result start();
    FollowMe::Result stop();

    FollowMeImpl(const FollowMeImpl &) = delete;
    FollowMeImpl &operator=(const FollowMeImpl &) = delete;

private:
    typedef unsigned int config_val_t;
    void process_heartbeat(const mavlink_message_t &message);

    enum class ConfigParameter;
    // Config methods
    bool is_config_ok(const FollowMe::Config &config) const;
    FollowMe::Result to_follow_me_result(MAVLinkCommands::Result result) const;

    bool is_target_location_set() const;
    void send_target_location();
    void stop_sending_target_location();

    enum class EstimationCapabilites { POS, VEL };

    enum class Mode { NOT_ACTIVE, ACTIVE } _mode = Mode::NOT_ACTIVE;

    enum class ConfigParameter {
        NONE = 0,
        FOLLOW_DIRECTION = 1 << 0,
        MIN_HEIGHT = 1 << 1,
        DISTANCE = 1 << 2,
        RESPONSIVENESS = 1 << 3
    };

    friend config_val_t operator~(ConfigParameter cfgp) { return ~static_cast<config_val_t>(cfgp); }
    friend config_val_t operator|(config_val_t config_val, ConfigParameter cfgp)
    {
        return (config_val) | static_cast<config_val_t>(cfgp);
    }
    friend config_val_t operator|=(config_val_t &config_val, ConfigParameter cfgp)
    {
        return config_val = config_val | static_cast<config_val_t>(cfgp);
    }
    friend bool operator!=(config_val_t config_val, ConfigParameter cfgp)
    {
        return config_val != static_cast<config_val_t>(cfgp);
    }
    friend bool operator==(config_val_t config_val, ConfigParameter cfgp)
    {
        return config_val == static_cast<config_val_t>(cfgp);
    }

    mutable std::mutex _mutex{};
    FollowMe::TargetLocation _target_location{}; // sent to vehicle
    FollowMe::TargetLocation _last_location{}; // sent to vehicle
    void *_target_location_cookie = nullptr;

    Time _time{};
    uint8_t _estimatation_capabilities = 0; // sent to vehicle
    FollowMe::Config _config{}; // has FollowMe configuration settings

    const float SENDER_RATE = 1.0f; // send location updates once in a second

    std::string debug_str = "FollowMe: ";
};

} // namespace dronecode_sdk
