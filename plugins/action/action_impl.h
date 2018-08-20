#pragma once

#include <cstdint>

#include "plugins/action/action.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"

namespace dronecode_sdk {

class ActionImpl : public PluginImplBase {
public:
    ActionImpl(System &system);
    ~ActionImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Action::Result arm() const;
    Action::Result disarm() const;
    Action::Result kill() const;
    Action::Result reboot() const;
    Action::Result takeoff() const;
    Action::Result land() const;
    Action::Result return_to_launch() const;
    Action::Result
    goto_location(double latitude_deg, double longitude_deg, float altitude_amsl_m, float yaw_deg);
    Action::Result transition_to_fixedwing() const;
    Action::Result transition_to_multicopter() const;

    void arm_async(const Action::result_callback_t &callback);
    void disarm_async(const Action::result_callback_t &callback);
    void kill_async(const Action::result_callback_t &callback);
    void takeoff_async(const Action::result_callback_t &callback);
    void land_async(const Action::result_callback_t &callback);
    void return_to_launch_async(const Action::result_callback_t &callback);
    void transition_to_fixedwing_async(const Action::result_callback_t &callback);
    void transition_to_multicopter_async(const Action::result_callback_t &callback);

    Action::Result set_takeoff_altitude(float relative_altitude_m);
    std::pair<Action::Result, float> get_takeoff_altitude() const;

    Action::Result set_max_speed(float speed_m_s);
    std::pair<Action::Result, float> get_max_speed() const;

    Action::Result set_return_to_launch_return_altitude(float relative_altitude_m);
    std::pair<Action::Result, float> get_return_to_launch_return_altitude() const;

private:
    void loiter_before_takeoff_async(const Action::result_callback_t &callback);
    void loiter_before_arm_async(const Action::result_callback_t &callback);

    void takeoff_async_continued(MAVLinkCommands::Result previous_result,
                                 const Action::result_callback_t &callback);
    void arm_async_continued(MAVLinkCommands::Result previous_result,
                             const Action::result_callback_t &callback);

    Action::Result arming_allowed() const;
    Action::Result disarming_allowed() const;
    Action::Result taking_off_allowed() const;

    void process_extended_sys_state(const mavlink_message_t &message);

    static Action::Result action_result_from_command_result(MAVLinkCommands::Result result);

    static void command_result_callback(MAVLinkCommands::Result command_result,
                                        const Action::result_callback_t &callback);

    std::atomic<bool> _in_air_state_known{false};
    std::atomic<bool> _in_air{false};

    std::atomic<bool> _vtol_transition_support_known{false};
    std::atomic<bool> _vtol_transition_possible{false};

    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;
    static constexpr auto TAKEOFF_ALT_PARAM = "MIS_TAKEOFF_ALT";
    static constexpr auto MAX_SPEED_PARAM = "MPC_XY_CRUISE";
    static constexpr auto RTL_RETURN_ALTITUDE_PARAM = "RTL_RETURN_ALT";
};

} // namespace dronecode_sdk
