#include "shell_impl.h"
#include "system.h"
#include "callback_list.tpp"

namespace mavsdk {

template class CallbackList<std::string>;

void ShellImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_SERIAL_CONTROL,
        [this](const mavlink_message_t& message) { process_shell_message(message); },
        this);
}

void ShellImpl::deinit()
{
    _system_impl->unregister_all_mavlink_message_handlers(this);
}

void ShellImpl::enable() {}

void ShellImpl::disable() {}

ShellImpl::ShellImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

ShellImpl::ShellImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
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

Shell::ReceiveHandle ShellImpl::subscribe_receive(const Shell::ReceiveCallback& callback)
{
    std::lock_guard<std::mutex> lock(_receive.mutex);
    return _receive.callbacks.subscribe(callback);
}

void ShellImpl::unsubscribe_receive(Shell::ReceiveHandle handle)
{
    std::lock_guard<std::mutex> lock(_receive.mutex);
    _receive.callbacks.unsubscribe(handle);
}

bool ShellImpl::send_command_message(std::string command)
{
    mavlink_message_t message;

    while (command.length() > MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN) {
        mavlink_msg_serial_control_pack(
            _system_impl->get_own_system_id(),
            _system_impl->get_own_component_id(),
            &message,
            static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_SHELL),
            0,
            timeout_ms,
            0,
            static_cast<uint8_t>(MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN),
            reinterpret_cast<const uint8_t*>(command.c_str()),
            _system_impl->get_system_id(),
            _system_impl->get_autopilot_id());
        command.erase(0, MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN);
        if (!_system_impl->send_message(message)) {
            return false;
        }
    }

    uint8_t flags = 0;
    {
        // We only ask for a response if we have subscribed to a response.
        std::lock_guard<std::mutex> lock(_receive.mutex);
        if (!_receive.callbacks.empty()) {
            flags |= SERIAL_CONTROL_FLAG_RESPOND;
        }
    }

    uint8_t data[MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN]{};
    memcpy(data, command.c_str(), command.length());

    mavlink_msg_serial_control_pack(
        _system_impl->get_own_system_id(),
        _system_impl->get_own_component_id(),
        &message,
        static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_SHELL),
        flags,
        timeout_ms,
        0,
        static_cast<uint8_t>(command.length()),
        data,
        _system_impl->get_system_id(),
        _system_impl->get_autopilot_id());

    return _system_impl->send_message(message);
}

void ShellImpl::process_shell_message(const mavlink_message_t& message)
{
    mavlink_serial_control_t serial_control;
    mavlink_msg_serial_control_decode(&message, &serial_control);

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

    std::lock_guard<std::mutex> lock(_receive.mutex);
    _receive.callbacks.queue(
        response, [this](const auto& func) { _system_impl->call_user_callback(func); });
}

} // namespace mavsdk
