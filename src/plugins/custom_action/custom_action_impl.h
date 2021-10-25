#pragma once

#include "plugins/custom_action/custom_action.h"
#include "plugin_impl_base.h"
#include <json/json.h>

namespace mavsdk {

class CustomActionImpl : public PluginImplBase {
public:
    explicit CustomActionImpl(System& system);
    explicit CustomActionImpl(std::shared_ptr<System> system);
    ~CustomActionImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    mavlink_message_t
    process_custom_action_command(const MavlinkCommandReceiver::CommandLong& command);

    void process_command_cancellation(const mavlink_message_t& message);

    CustomAction::Result respond_custom_action(
        CustomAction::ActionToExecute action_to_execute, CustomAction::Result result) const;

    void respond_custom_action_async(
        CustomAction::ActionToExecute action_to_execute,
        CustomAction::Result result,
        const CustomAction::ResultCallback& callback) const;

    CustomAction::Result set_custom_action(CustomAction::ActionToExecute& action) const;

    void set_custom_action_async(
        CustomAction::ActionToExecute& action, const CustomAction::ResultCallback& callback) const;

    CustomAction::ActionToExecute custom_action() const;

    void subscribe_custom_action(CustomAction::CustomActionCallback callback);

    bool custom_action_cancellation() const;

    void
    subscribe_custom_action_cancellation(CustomAction::CustomActionCancellationCallback callback);

    std::pair<CustomAction::Result, CustomAction::ActionMetadata>
    custom_action_metadata(CustomAction::ActionToExecute& action, std::string& file) const;

    void custom_action_metadata_async(
        CustomAction::ActionToExecute& action,
        const std::string& file,
        const CustomAction::CustomActionMetadataCallback& callback) const;

    CustomAction::Result execute_custom_action_stage(CustomAction::Stage& stage) const;

    void execute_custom_action_stage_async(
        CustomAction::Stage& stage, const CustomAction::ResultCallback& callback) const;

    CustomAction::Result execute_custom_action_global_script(std::string& global_script) const;

    void execute_custom_action_global_script_async(
        std::string& global_script, const CustomAction::ResultCallback& callback) const;

    void command_result_callback(
        MavlinkCommandSender::Result command_result,
        const CustomAction::ResultCallback& callback) const;

    static int exec_command(const std::string& cmd_str);

    static CustomAction::Result custom_action_result_from_script_result(int result);

    static CustomAction::Result
    custom_action_result_from_mavlink_parameters_result(MAVLinkParameters::Result result);

private:
    static CustomAction::Result
    custom_action_result_from_command_result(MavlinkCommandSender::Result result);

    static MAV_RESULT mavlink_command_result_from_custom_action_result(CustomAction::Result result);

    void store_custom_action(CustomAction::ActionToExecute action_to_execute);

    void store_custom_action_cancellation(bool action_cancel);

    mutable std::mutex _custom_action_mutex{};
    CustomAction::ActionToExecute _custom_action{};

    mutable std::mutex _custom_action_cancellation_mutex{};
    bool _custom_action_cancellation{};

    std::mutex _subscription_mutex{};
    CustomAction::CustomActionCallback _custom_action_command_subscription{nullptr};
    CustomAction::CustomActionCancellationCallback _custom_action_command_cancel_subscription{
        nullptr};
};

} // namespace mavsdk
