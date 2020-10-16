#include "failure_impl.h"

namespace mavsdk {

FailureImpl::FailureImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

FailureImpl::FailureImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

FailureImpl::~FailureImpl()
{
    _parent->unregister_plugin(this);
}

void FailureImpl::init() {}

void FailureImpl::deinit() {}

void FailureImpl::enable()
{
    constexpr auto param_name = "SYS_FAILURE_EN";

    _parent->get_param_int_async(
        param_name,
        [this](MAVLinkParameters::Result result, int32_t value) {
            if (result == MAVLinkParameters::Result::Success) {
                if (value == 1) {
                    _enabled = EnabledState::Enabled;
                } else if (value == 0) {
                    _enabled = EnabledState::Disabled;
                } else {
                    _enabled = EnabledState::Unknown;
                }
            } else {
                _enabled = EnabledState::Unknown;
            }
        },
        this);

    _parent->subscribe_param_int(
        param_name,
        [this](int value) {
            if (value == 1) {
                _enabled = EnabledState::Enabled;
            } else if (value == 0) {
                _enabled = EnabledState::Disabled;
            } else {
                _enabled = EnabledState::Unknown;
            }
        },
        this);
}

void FailureImpl::disable()
{
    _enabled = EnabledState::Init;
}

Failure::Result FailureImpl::inject(
    Failure::FailureUnit failure_unit, Failure::FailureType failure_type, int32_t instance)
{
    while (_enabled == EnabledState::Init) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // If the param is unknown we ignore it and try anyway.
    if (_enabled == EnabledState::Disabled) {
        return Failure::Result::Disabled;
    }

    MAVLinkCommands::CommandLong command{};

    command.command = MAV_CMD_INJECT_FAILURE;
    command.params.param1 = static_cast<float>(failure_unit);
    command.params.param2 = static_cast<float>(failure_type);
    command.params.param3 = static_cast<float>(instance);
    command.target_component_id = _parent->get_autopilot_id();

    return failure_result_from_command_result(_parent->send_command(command));
}

Failure::Result
FailureImpl::failure_result_from_command_result(MAVLinkCommands::Result command_result)
{
    switch (command_result) {
        case MAVLinkCommands::Result::Success:
            return Failure::Result::Success;
        case MAVLinkCommands::Result::NoSystem:
            return Failure::Result::NoSystem;
        case MAVLinkCommands::Result::ConnectionError:
            return Failure::Result::ConnectionError;
        case MAVLinkCommands::Result::CommandDenied:
            return Failure::Result::Denied;
        case MAVLinkCommands::Result::Unsupported:
            return Failure::Result::Unsupported;
        case MAVLinkCommands::Result::Timeout:
            return Failure::Result::Timeout;
        default:
            return Failure::Result::Unknown;
    }
}

} // namespace mavsdk
