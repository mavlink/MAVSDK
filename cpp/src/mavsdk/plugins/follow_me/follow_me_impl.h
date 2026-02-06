#pragma once

#include "log.h"
#include "mavlink_include.h"
#include "plugins/follow_me/follow_me.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "timeout_handler.h"

namespace mavsdk {

class FollowMeImpl : public PluginImplBase {
public:
    explicit FollowMeImpl(System& system);
    explicit FollowMeImpl(std::shared_ptr<System> system);
    ~FollowMeImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    FollowMe::Config get_config() const;
    FollowMe::Result set_config(const FollowMe::Config& config);

    FollowMe::Result set_target_location(const FollowMe::TargetLocation& location);
    FollowMe::TargetLocation get_last_location() const;

    bool is_active() const;

    FollowMe::Result start();
    FollowMe::Result stop();

    FollowMeImpl(const FollowMeImpl&) = delete;
    FollowMeImpl& operator=(const FollowMeImpl&) = delete;

private:
    using ConfigVal = unsigned int;
    void process_heartbeat(const mavlink_message_t& message);

    enum class ConfigParameter;

    // Config methods
    bool is_config_ok(const FollowMe::Config& config) const;

    FollowMe::Result to_follow_me_result(MavlinkCommandSender::Result result) const;

    bool is_target_location_set() const;
    void send_target_location();
    void stop_sending_target_location();

    enum class EstimationCapabilities { POS, VEL };

    enum class Mode { NOT_ACTIVE, ACTIVE } _mode = Mode::NOT_ACTIVE;

    enum class ConfigParameter {
        NONE = 0,
        FOLLOW_DIRECTION = 1 << 0,
        MIN_HEIGHT = 1 << 1,
        DISTANCE = 1 << 2,
        RESPONSIVENESS = 1 << 3
    };

    constexpr static const float CONFIG_MIN_HEIGHT_M = 8.0f;
    constexpr static const float CONFIG_MIN_FOLLOW_DIST_M = 2.0f;

    constexpr static const float CONFIG_MIN_RESPONSIVENESS = 0.f;
    constexpr static const float CONFIG_MAX_RESPONSIVENESS = 1.0f;

    constexpr static const float CONFIG_MIN_FOLLOW_ANGLE = -180.0f;
    constexpr static const float CONFIG_MAX_FOLLOW_ANGLE = 180.0f;

    /* <Upstream Enum definitions, to map from the Protobuf generated enums>
     * NOTE : This MUST be up to date with the upstream PX4's enum values!! */

    // Follow Altitude modes set by the parameter FLW_TGT_ALT_M
    enum kFollowAltitudeMode {
        kFollowAltitudeModeConstant,
        kFollowAltitudeModeTerrain,
        kFollowAltitudeModeTrackTarget
    };

    friend ConfigVal operator~(ConfigParameter cfgp) { return ~static_cast<ConfigVal>(cfgp); }
    friend ConfigVal operator|(ConfigVal config_val, ConfigParameter cfgp)
    {
        return (config_val) | static_cast<ConfigVal>(cfgp);
    }
    friend ConfigVal operator|=(ConfigVal& config_val, ConfigParameter cfgp)
    {
        return config_val = config_val | static_cast<ConfigVal>(cfgp);
    }
    friend bool operator!=(ConfigVal config_val, ConfigParameter cfgp)
    {
        return config_val != static_cast<ConfigVal>(cfgp);
    }
    friend bool operator==(ConfigVal config_val, ConfigParameter cfgp)
    {
        return config_val == static_cast<ConfigVal>(cfgp);
    }

    mutable std::mutex _mutex{};
    FollowMe::TargetLocation _target_location{}; // sent to vehicle
    FollowMe::TargetLocation _last_location{}; // sent to vehicle
    CallEveryHandler::Cookie _target_location_cookie{};

    Time _time{};
    uint8_t _estimation_capabilities = 0; // sent to vehicle
    FollowMe::Config _config{}; // has FollowMe configuration settings

    const float SENDER_RATE = 1.0f; // send location updates once in a second

    std::string debug_str = "FollowMe: ";
};

} // namespace mavsdk
