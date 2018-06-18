#pragma once

#include <mutex>
#include "plugin_impl_base.h"
#include "mavlink_include.h"
#include "system.h"
#include "plugins/offboard/offboard.h"

namespace dronecode_sdk {

class OffboardImpl : public PluginImplBase {
public:
    OffboardImpl(System &system);
    ~OffboardImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Offboard::Result start();
    Offboard::Result stop();

    void start_async(Offboard::result_callback_t callback);
    void stop_async(Offboard::result_callback_t callback);

    bool is_active() const;

    void set_position_ned_yaw(Offboard::PositionNEDYaw position_ned_yaw);
    void set_velocity_ned(Offboard::VelocityNEDYaw velocity_ned_yaw);
    void set_velocity_altitude_ned_yaw(Offboard::VelocityAltitudeNEDYaw velocity_altitude_ned_yaw);
    void
    set_position_climbrate_ned_yaw(Offboard::PositionClimbRateNEDYaw position_climbrate_ned_yaw);
    void set_position_climbrate_ned_yawspeed(
        Offboard::PositionClimbRateNEDYawspeed position_climbrate_ned_yawspeed);
    void set_velocity_body(Offboard::VelocityBodyYawspeed velocity_body_yawspeed);

private:
    struct TranslationNED {
        float north_m;
        float east_m;
        float down_m;
        float north_m_s;
        float east_m_s;
        float down_m_s;
        float yaw_deg;
        float yawspeed_deg_s;
    };

    void set_translation_ned(TranslationNED translation_ned);
    void send_translation_ned();
    void send_velocity_ned();
    void send_velocity_body();

    void process_heartbeat(const mavlink_message_t &message);
    void receive_command_result(MAVLinkCommands::Result result,
                                const Offboard::result_callback_t &callback);

    static Offboard::Result offboard_result_from_command_result(MAVLinkCommands::Result result);

    void stop_sending_setpoints();

    mutable std::mutex _mutex{};
    enum class Mode {
        NOT_ACTIVE,
        TRANSLATION_NED,
        VELOCITY_NED,
        VELOCITY_BODY
    } _mode = Mode::NOT_ACTIVE;

    TranslationNED _translation_ned{};
    Offboard::VelocityNEDYaw _velocity_ned_yaw{};
    Offboard::VelocityBodyYawspeed _velocity_body_yawspeed{};

    void *_call_every_cookie = nullptr;

    const float SEND_INTERVAL_S = 0.1f;
};

} // namespace dronecode_sdk
