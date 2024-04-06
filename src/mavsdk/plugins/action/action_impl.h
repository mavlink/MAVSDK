#pragma once

#include "mavlink_include.h"
#include "plugins/action/action.h"
#include "plugin_impl_base.h"
#include <atomic>
#include <cstdint>

namespace mavsdk {

class ActionImpl : public PluginImplBase {
public:
    explicit ActionImpl(System& system);
    explicit ActionImpl(std::shared_ptr<System> system_ptr);
    ~ActionImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Action::Result arm() const;
    Action::Result arm_force() const;
    Action::Result disarm() const;
    Action::Result terminate() const;
    Action::Result kill() const;
    Action::Result reboot() const;
    Action::Result shutdown() const;
    Action::Result takeoff() const;
    Action::Result land() const;
    Action::Result return_to_launch() const;
    Action::Result goto_location(
        const double latitude_deg,
        const double longitude_deg,
        const float altitude_amsl_m,
        const float yaw_deg);
    Action::Result do_orbit(
        const float radius_m,
        const float velocity_ms,
        const Action::OrbitYawBehavior yaw_behavior,
        const double latitude_deg,
        const double longitude_deg,
        const double absolute_altitude_m);
    Action::Result hold() const;
    Action::Result set_actuator(const int index, const float value);
    Action::Result transition_to_fixedwing() const;
    Action::Result transition_to_multicopter() const;

    void arm_async(const Action::ResultCallback& callback) const;
    void arm_force_async(const Action::ResultCallback& callback) const;
    void disarm_async(const Action::ResultCallback& callback) const;
    void terminate_async(const Action::ResultCallback& callback) const;
    void kill_async(const Action::ResultCallback& callback) const;
    void reboot_async(const Action::ResultCallback& callback) const;
    void shutdown_async(const Action::ResultCallback& callback) const;
    void takeoff_async(const Action::ResultCallback& callback) const;
    void land_async(const Action::ResultCallback& callback) const;
    void return_to_launch_async(const Action::ResultCallback& callback) const;
    void goto_location_async(
        const double latitude_deg,
        const double longitude_deg,
        const float altitude_amsl_m,
        const float yaw_deg,
        const Action::ResultCallback& callback);
    void do_orbit_async(
        const float radius_m,
        const float velocity_ms,
        const Action::OrbitYawBehavior yaw_behavior,
        const double latitude_deg,
        const double longitude_deg,
        const double absolute_altitude_m,
        const Action::ResultCallback& callback);
    void hold_async(const Action::ResultCallback& callback) const;
    void
    set_actuator_async(const int index, const float value, const Action::ResultCallback& callback);
    void transition_to_fixedwing_async(const Action::ResultCallback& callback) const;
    void transition_to_multicopter_async(const Action::ResultCallback& callback) const;

    void set_takeoff_altitude_async(
        const float relative_altitude_m, const Action::ResultCallback& callback);
    void get_takeoff_altitude_async(const Action::GetTakeoffAltitudeCallback& callback) const;

    Action::Result set_takeoff_altitude(float relative_altitude_m);
    std::pair<Action::Result, float> get_takeoff_altitude() const;

    void
    set_maximum_speed_async(const float speed_m_s, const Action::ResultCallback& callback) const;
    void get_maximum_speed_async(const Action::GetMaximumSpeedCallback& callback) const;

    Action::Result set_maximum_speed(float speed_m_s) const;
    std::pair<Action::Result, float> get_maximum_speed() const;

    void set_return_to_launch_altitude_async(
        const float relative_altitude_m, const Action::ResultCallback& callback) const;
    void get_return_to_launch_altitude_async(
        const Action::GetReturnToLaunchAltitudeCallback& callback) const;

    void set_current_speed_async(float speed_m_s, const Action::ResultCallback& callback);
    Action::Result set_current_speed(float speed_m_s);

    Action::Result set_return_to_launch_altitude(const float relative_altitude_m) const;
    std::pair<Action::Result, float> get_return_to_launch_altitude() const;

private:
    void process_extended_sys_state(const mavlink_message_t& message);

    static Action::Result action_result_from_command_result(MavlinkCommandSender::Result result);

    void command_result_callback(
        MavlinkCommandSender::Result command_result, const Action::ResultCallback& callback) const;

    bool need_hold_before_arm() const;
    bool need_hold_before_arm_px4() const;
    bool need_hold_before_arm_apm() const;

    void takeoff_async_px4(const Action::ResultCallback& callback) const;
    void takeoff_async_apm(const Action::ResultCallback& callback) const;

    Action::Result set_takeoff_altitude_px4(float relative_altitude_m);
    Action::Result set_takeoff_altitude_apm(float relative_altitude_m);

    std::atomic<bool> _vtol_transition_support_known{false};
    std::atomic<bool> _vtol_transition_possible{false};

    float _takeoff_altitude{2.0};

    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;
    static constexpr auto TAKEOFF_ALT_PARAM = "MIS_TAKEOFF_ALT";
    static constexpr auto MAX_SPEED_PARAM = "MPC_XY_CRUISE";
    static constexpr auto RTL_RETURN_ALTITUDE_PARAM = "RTL_RETURN_ALT";
};

} // namespace mavsdk
