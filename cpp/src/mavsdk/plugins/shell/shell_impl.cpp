#include "shell_impl.hpp"
#include "mavlink_address.hpp"
#include "system.hpp"
#include "callback_list.tpp"
#include "mavsdk_export.h"

#include <optional>

namespace mavsdk {

template class MAVSDK_TEMPL_INST CallbackList<std::string>;

void ShellImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_SERIAL_CONTROL,
        [this](const mavlink_message_t& message) { process_shell_message(message); },
        this);
}

void ShellImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers_blocking(this);
}

void ShellImpl::enable() {}

void ShellImpl::disable() {}

ShellImpl::ShellImpl(System& system) : PluginImplBase(system), _receive(_system_impl->io_context())
{
    _system_impl->register_plugin(this);
}

ShellImpl::ShellImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system)),
    _receive(_system_impl->io_context())
{
    _system_impl->register_plugin(this);
}

ShellImpl::~ShellImpl()
{
    _system_impl->unregister_plugin(this);
}

Shell::Result ShellImpl::send(std::string command)
{
    if (!_system_impl->is_connected()) {
        return Shell::Result::NoSystem;
    }

    // In case a newline at the end of the command is missing, we add it here.
    if (command.back() != '\n') {
        command.append(1, '\n');
    }

    if (!send_command_message(command)) {
        return Shell::Result::ConnectionError;
    }

    return Shell::Result::Success;
}

Shell::Result ShellImpl::set_device(Shell::Device device)
{
    const auto mav_device = device_to_mavlink(device);
    if (!mav_device) {
        return Shell::Result::InvalidArgument;
    }

    _device.store(*mav_device, std::memory_order_relaxed);
    return Shell::Result::Success;
}

std::optional<uint8_t> ShellImpl::device_to_mavlink(Shell::Device device)
{
    switch (device) {
        case Shell::Device::Telem1:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_TELEM1);
        case Shell::Device::Telem2:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_TELEM2);
        case Shell::Device::Gps1:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_GPS1);
        case Shell::Device::Gps2:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_GPS2);
        case Shell::Device::Shell:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_SHELL);
        case Shell::Device::Serial0:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL0);
        case Shell::Device::Serial1:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL1);
        case Shell::Device::Serial2:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL2);
        case Shell::Device::Serial3:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL3);
        case Shell::Device::Serial4:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL4);
        case Shell::Device::Serial5:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL5);
        case Shell::Device::Serial6:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL6);
        case Shell::Device::Serial7:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL7);
        case Shell::Device::Serial8:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL8);
        case Shell::Device::Serial9:
            return static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_SERIAL9);
        default:
            return std::nullopt;
    }
}

Shell::ReceiveHandle ShellImpl::subscribe_receive(const Shell::ReceiveCallback& callback)
{
    return _receive.callbacks.subscribe(callback);
}

void ShellImpl::unsubscribe_receive(Shell::ReceiveHandle handle)
{
    _receive.callbacks.unsubscribe(handle);
}

bool ShellImpl::send_command_message(std::string command)
{
    mavlink_message_t message;
    const uint8_t device = _device.load(std::memory_order_relaxed);

    while (command.length() > MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN) {
        if (!_system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
                mavlink_msg_serial_control_pack_chan(
                    mavlink_address.system_id,
                    mavlink_address.component_id,
                    channel,
                    &message,
                    device,
                    0,
                    timeout_ms,
                    0,
                    static_cast<uint8_t>(MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN),
                    reinterpret_cast<const uint8_t*>(command.c_str()),
                    _system_impl->get_system_id(),
                    _system_impl->get_autopilot_id());
                command.erase(0, MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN);
                return message;
            })) {
            return false;
        }
    }

    uint8_t flags = 0;
    {
        // We only ask for a response if we have subscribed to a response.
        if (!_receive.callbacks.empty()) {
            flags |= SERIAL_CONTROL_FLAG_RESPOND;
        }
    }

    uint8_t data[MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN]{};
    memcpy(data, command.c_str(), command.length());

    return _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_msg_serial_control_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            device,
            flags,
            timeout_ms,
            0,
            static_cast<uint8_t>(command.length()),
            data,
            _system_impl->get_system_id(),
            _system_impl->get_autopilot_id());
        return message;
    });
}

void ShellImpl::process_shell_message(const mavlink_message_t& message)
{
    mavlink_serial_control_t serial_control;
    mavlink_msg_serial_control_decode(&message, &serial_control);

    if (serial_control.device != _device.load(std::memory_order_relaxed)) {
        return;
    }

    // This adds an additional byte for the null termination.
    char str_copy[sizeof(serial_control.data) + 1]{0};

    const auto len =
        std::min(static_cast<std::size_t>(serial_control.count), sizeof(serial_control.data));

    memcpy(str_copy, serial_control.data, len);

    std::string response(str_copy);

    // For the NuttShell (nsh>) we see these characters being sent but we're not sure
    // what they are for, so we're removing them for now.
    auto index = response.find({32, 27, '[', 'K'});
    if (index != std::string::npos) {
        response.erase(index, 4);
    }

    _receive.callbacks.queue(
        response, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

} // namespace mavsdk
