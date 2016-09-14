#pragma once

#include "plugin_impl_base.h"
#include "mavlink_include.h"
#include "device_impl.h"
#include "action.h"
#include <cstdint>

namespace dronelink {

class ActionImpl : public PluginImplBase
{
public:
    ActionImpl();
    ~ActionImpl();

    void init() override;
    void deinit() override;

    Action::Result arm() const;
    Action::Result disarm() const;
    Action::Result kill() const;
    Action::Result takeoff() const;
    Action::Result land() const;
    Action::Result return_to_land() const;

    void arm_async(Action::CallbackData callback);
    void disarm_async(Action::CallbackData callback);
    void kill_async(Action::CallbackData callback);
    void takeoff_async(Action::CallbackData callback);
    void land_async(Action::CallbackData callback);
    void return_to_land_async(Action::CallbackData callback);

private:
    bool is_arm_allowed() const;
    bool is_disarm_allowed() const;

    void process_extended_sys_state(const mavlink_message_t &message);

    static void report_result(Action::CallbackData callback_data, Action::Result result);

    static Action::Result action_result_from_command_result(DeviceImpl::CommandResult result);

    static void command_result_callback(DeviceImpl::CommandResult command_result,
                                        void *user);

    bool _in_air_state_known;
    bool _in_air;

    static constexpr uint8_t VEHICLE_MODE_FLAG_CUSTOM_MODE_ENABLED = 1;

    // FIXME: these chould potentially change anytime
    static constexpr uint8_t PX4_CUSTOM_MAIN_MODE_AUTO = 4;
    static constexpr uint8_t PX4_CUSTOM_SUB_MODE_AUTO_RTL = 5;
};

} // namespace dronelink
