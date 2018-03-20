#pragma once

#include <cstdint>

#include "action.h"
#include "action_result.h"
#include "mavlink_system.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"

namespace dronecore {

class ActionImpl : public PluginImplBase
{
public:
    ActionImpl(System &system);
    ~ActionImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    ActionResult arm() const;
    ActionResult disarm() const;
    ActionResult kill() const;
    ActionResult takeoff() const;
    ActionResult land() const;
    ActionResult return_to_launch() const;
    ActionResult transition_to_fixedwing() const;
    ActionResult transition_to_multicopter() const;

    void arm_async(const Action::result_callback_t &callback);
    void disarm_async(const Action::result_callback_t &callback);
    void kill_async(const Action::result_callback_t &callback);
    void takeoff_async(const Action::result_callback_t &callback);
    void land_async(const Action::result_callback_t &callback);
    void return_to_launch_async(const Action::result_callback_t &callback);
    void transition_to_fixedwing_async(const Action::result_callback_t &callback);
    void transition_to_multicopter_async(const Action::result_callback_t &callback);

    void set_takeoff_altitude(float relative_altitude_m);
    float get_takeoff_altitude_m() const;

    void set_max_speed(float speed_m_s);
    float get_max_speed_m_s() const;

private:
    void loiter_before_takeoff_async(const Action::result_callback_t &callback);
    void loiter_before_arm_async(const Action::result_callback_t &callback);

    void takeoff_async_continued(MAVLinkCommands::Result previous_result,
                                 const Action::result_callback_t &callback);
    void arm_async_continued(MAVLinkCommands::Result previous_result,
                             const Action::result_callback_t &callback);

    ActionResult arming_allowed() const;
    ActionResult disarming_allowed() const;
    ActionResult taking_off_allowed() const;

    void process_extended_sys_state(const mavlink_message_t &message);

    void receive_max_speed_result(bool success, float new_speed_m_s);

    void receive_takeoff_alt_param(bool success, float new_relative_altitude_m);

    static ActionResult action_result_from_command_result(MAVLinkCommands::Result result);

    static void command_result_callback(MAVLinkCommands::Result command_result,
                                        const Action::result_callback_t &callback);

    std::atomic<bool> _in_air_state_known {false};
    std::atomic<bool> _in_air {false};

    std::atomic<bool>_vtol_transition_support_known {false};
    std::atomic<bool> _vtol_transition_possible {false};

    float _relative_takeoff_altitude_m = 2.5f;

    float _max_speed_m_s = NAN;

    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;
};

} // namespace dronecore
