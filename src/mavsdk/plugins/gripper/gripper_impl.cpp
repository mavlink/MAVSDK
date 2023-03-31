#include "gripper_impl.h"

namespace mavsdk {

GripperImpl::GripperImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

GripperImpl::GripperImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

GripperImpl::~GripperImpl()
{
    _system_impl->unregister_plugin(this);
}

void GripperImpl::init() {}

void GripperImpl::deinit() {}

void GripperImpl::enable() {}

void GripperImpl::disable() {}

void GripperImpl::grab_async(uint32_t instance, const Gripper::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_GRIPPER;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Gripper::GripperAction::Grab);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH; // TODO

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Gripper::Result GripperImpl::grab(uint32_t instance)
{
    auto prom = std::promise<Gripper::Result>();
    auto fut = prom.get_future();

    grab_async(instance, [&prom](Gripper::Result result) { prom.set_value(result); });

    return fut.get();
}

void GripperImpl::release_async(uint32_t instance, const Gripper::ResultCallback callback)
{
    MavlinkCommandSender::CommandLong command{};

    command.command = MAV_CMD_DO_GRIPPER;
    command.params.maybe_param1 = static_cast<float>(instance);
    command.params.maybe_param2 = static_cast<float>(Gripper::GripperAction::Release);

    command.target_component_id = MAV_COMPONENT::MAV_COMP_ID_WINCH; // TODO

    _system_impl->send_command_async(
        command, [this, callback](MavlinkCommandSender::Result result, float) {
            command_result_callback(result, callback);
        });
}

Gripper::Result GripperImpl::release(uint32_t instance)
{
    auto prom = std::promise<Gripper::Result>();
    auto fut = prom.get_future();

    release_async(instance, [&prom](Gripper::Result result) { prom.set_value(result); });

    return fut.get();
}

Gripper::Result GripperImpl::gripper_result_from_command_result(MavlinkCommandSender::Result result)
{
    switch (result) {
        case MavlinkCommandSender::Result::Success:
            return Gripper::Result::Success;
        case MavlinkCommandSender::Result::NoSystem:
            return Gripper::Result::NoSystem;
        case MavlinkCommandSender::Result::ConnectionError:
            // Fallthrough
        case MavlinkCommandSender::Result::Timeout:
            return Gripper::Result::Timeout;
        case MavlinkCommandSender::Result::Busy:
            return Gripper::Result::Busy;
        case MavlinkCommandSender::Result::Denied:
            // Fallthrough
        case MavlinkCommandSender::Result::TemporarilyRejected:
            // Fallthrough
        case MavlinkCommandSender::Result::Failed:
            return Gripper::Result::Failed;
        case MavlinkCommandSender::Result::Unsupported:
            return Gripper::Result::Unsupported;
        default:
            return Gripper::Result::Unknown;
    }
}

void GripperImpl::command_result_callback(
    MavlinkCommandSender::Result command_result, const Gripper::ResultCallback& callback) const
{
    Gripper::Result action_result = gripper_result_from_command_result(command_result);

    if (callback) {
        auto temp_callback = callback;
        _system_impl->call_user_callback(
            [temp_callback, action_result]() { temp_callback(action_result); });
    }
}

} // namespace mavsdk
