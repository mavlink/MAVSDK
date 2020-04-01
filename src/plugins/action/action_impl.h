#pragma once

#include "mavlink_include.h"
#include "plugins/action/action.h"
#include "plugin_impl_base.h"
#include <atomic>
#include <cstdint>

namespace mavsdk {

class ActionImpl : public PluginImplBase {
public:
    ActionImpl(System& system);
    ~ActionImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Action::Result arm() const;
    Action::Result disarm() const;
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
    Action::Result transition_to_fixedwing() const;
    Action::Result transition_to_multicopter() const;

    void arm_async(const Action::result_callback_t& callback) const;
    void disarm_async(const Action::result_callback_t& callback) const;
    void kill_async(const Action::result_callback_t& callback) const;
    void reboot_async(const Action::result_callback_t& callback) const;
    void shutdown_async(const Action::result_callback_t& callback) const;
    void takeoff_async(const Action::result_callback_t& callback) const;
    void land_async(const Action::result_callback_t& callback) const;
    void return_to_launch_async(const Action::result_callback_t& callback) const;
    void goto_location_async(
        const double latitude_deg,
        const double longitude_deg,
        const float altitude_amsl_m,
        const float yaw_deg,
        const Action::result_callback_t& callback);
    void transition_to_fixedwing_async(const Action::result_callback_t& callback) const;
    void transition_to_multicopter_async(const Action::result_callback_t& callback) const;

    void set_takeoff_altitude_async(
        const float relative_altitude_m, const Action::result_callback_t& callback) const;
    void get_takeoff_altitude_async(const Action::get_takeoff_altitude_callback_t& callback) const;

    Action::Result set_takeoff_altitude(float relative_altitude_m) const;
    std::pair<Action::Result, float> get_takeoff_altitude() const;

    void
    set_maximum_speed_async(const float speed_m_s, const Action::result_callback_t& callback) const;
    void get_maximum_speed_async(const Action::get_maximum_speed_callback_t& callback) const;

    Action::Result set_maximum_speed(float speed_m_s) const;
    std::pair<Action::Result, float> get_maximum_speed() const;

    void set_return_to_launch_altitude_async(
        const float relative_altitude_m, const Action::result_callback_t& callback) const;
    void get_return_to_launch_altitude_async(
        const Action::get_return_to_launch_altitude_callback_t& callback) const;

    Action::Result set_return_to_launch_altitude(const float relative_altitude_m) const;
    std::pair<Action::Result, float> get_return_to_launch_altitude() const;

private:
    Action::Result disarming_allowed() const;
    Action::Result taking_off_allowed() const;

    void process_extended_sys_state(const mavlink_message_t& message);

    static Action::Result action_result_from_command_result(MAVLinkCommands::Result result);

    void command_result_callback(
        MAVLinkCommands::Result command_result, const Action::result_callback_t& callback) const;

    std::atomic<bool> _in_air_state_known{false};
    std::atomic<bool> _in_air{false};

    std::atomic<bool> _vtol_transition_support_known{false};
    std::atomic<bool> _vtol_transition_possible{false};

    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;
    static constexpr auto TAKEOFF_ALT_PARAM = "MIS_TAKEOFF_ALT";
    static constexpr auto MAX_SPEED_PARAM = "MPC_XY_CRUISE";
    static constexpr auto RTL_RETURN_ALTITUDE_PARAM = "RTL_RETURN_ALT";
};

} // namespace mavsdk
