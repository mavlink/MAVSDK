#include "shell_impl.h"
#include "system.h"

namespace mavsdk {

void ShellImpl::init()
{
    using namespace std::placeholders; // for `_1`
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_SERIAL_CONTROL,
        std::bind(&ShellImpl::process_shell_message, this, _1),
        this);
}

void ShellImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void ShellImpl::enable() {}

void ShellImpl::disable() {}

ShellImpl::ShellImpl(System& system) :
    PluginImplBase(system),
    _transfer_finished_future(_transfer_finished_promise.get_future())
{
    _parent->register_plugin(this);
    _transfer_finished_promise.set_value();
}

ShellImpl::~ShellImpl()
{
    _parent->unregister_plugin(this);
}

bool ShellImpl::is_transfer_in_progress()
{
    if (!_transfer_finished_future.valid()) { // Should never happen
        return false;
    }
    return _transfer_finished_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
}

Shell::Result ShellImpl::shell_command_response_async(Shell::result_callback_t& callback)
{
    std::lock_guard<std::mutex> lock(_transfer_mutex);
    if (is_transfer_in_progress()) {
        return Shell::Result::BUSY;
    }
    _result_subscription = callback;
    return Shell::Result::SUCCESS;
}

void ShellImpl::finish_transfer(Shell::Result result, Shell::ShellMessage response_shell_message)
{
    std::lock_guard<std::mutex> lock(_transfer_mutex);
    if (!is_transfer_in_progress()) {
        return;
    }
    if (_result_subscription) {
        auto callback = _result_subscription;
        _parent->call_user_callback([callback, result, response_shell_message]() {
            callback(result, response_shell_message);
        });
    }
    _transfer_finished_promise.set_value();
}

Shell::Result ShellImpl::shell_command(const Shell::ShellMessage& shell_message)
{
    {
        std::lock_guard<std::mutex> lock(_transfer_mutex);
        if (is_transfer_in_progress()) {
            return Shell::Result::BUSY;
        }

        _transfer_finished_promise = std::promise<void>();
        _transfer_finished_future = _transfer_finished_promise.get_future();
    }

    if (!_parent->is_connected()) {
        finish_transfer(Shell::Result::NO_SYSTEM, Shell::ShellMessage{});
        return Shell::Result::NO_SYSTEM;
    }

    _shell_message = shell_message;

    if (_shell_message.data.back() != '\n') {
        _shell_message.data.append(1, '\n');
    }

    if (!send_shell_message_mavlink()) {
        finish_transfer(Shell::Result::CONNECTION_ERROR, Shell::ShellMessage{});
        return Shell::Result::CONNECTION_ERROR;
    }

    if (!_result_subscription || !shell_message.need_response) {
        finish_transfer(Shell::Result::SUCCESS, Shell::ShellMessage{});
        return Shell::Result::SUCCESS;
    }

    _response = Shell::ShellMessage{};

    if (_shell_message_timeout_cookie) {
        _parent->unregister_timeout_handler(_shell_message_timeout_cookie);
        _shell_message_timeout_cookie = nullptr;
    }
    _parent->register_timeout_handler(
        std::bind(&ShellImpl::receive_shell_message_timeout, this),
        shell_message.timeout / 1000.0,
        &_shell_message_timeout_cookie);
    return Shell::Result::SUCCESS;
}

void ShellImpl::receive_shell_message_timeout()
{
    std::lock_guard<std::mutex> lock(_transfer_mutex);
    Shell::Result result =
        (_response.data.length() ? Shell::Result::SUCCESS : Shell::Result::NO_RESPONSE);
    if (!is_transfer_in_progress()) {
        return;
    }
    if (_result_subscription) {
        auto callback = _result_subscription;
        Shell::ShellMessage response = _response;
        _parent->call_user_callback([callback, result, response]() { callback(result, response); });
    }
    _transfer_finished_promise.set_value();

    _parent->unregister_timeout_handler(_shell_message_timeout_cookie);
    _shell_message_timeout_cookie = nullptr;
}

bool ShellImpl::send_shell_message_mavlink()
{
    mavlink_message_t message;
    Shell::ShellMessage shell_message{};

    shell_message = _shell_message;

    while (shell_message.data.length() > MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN) {
        mavlink_msg_serial_control_pack(
            _parent->get_own_system_id(),
            _parent->get_own_component_id(),
            &message,
            static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_SHELL),
            0,
            shell_message.timeout,
            0,
            static_cast<uint8_t>(MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN),
            reinterpret_cast<const uint8_t*>(shell_message.data.c_str()));
        shell_message.data.erase(0, MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN);
        if (!_parent->send_message(message)) {
            return false;
        }
    }

    uint8_t flags = shell_message.need_response ? SERIAL_CONTROL_FLAG_RESPOND : 0;

    uint8_t data[MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN]{};
    memcpy(data, shell_message.data.c_str(), shell_message.data.length());

    mavlink_msg_serial_control_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &message,
        static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_SHELL),
        flags,
        shell_message.timeout,
        0,
        static_cast<uint8_t>(shell_message.data.length()),
        data);

    return _parent->send_message(message);
}

void ShellImpl::process_shell_message(const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_transfer_mutex);
    if (!is_transfer_in_progress()) { // Skip symbols after ESC
        return;
    }
    int count = 0;
    uint8_t data[MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN + 1]{0};

    _response.timeout = mavlink_msg_serial_control_get_timeout(&message);
    _response.need_response =
        mavlink_msg_serial_control_get_flags(&message) & SERIAL_CONTROL_FLAG_RESPOND;

    mavlink_msg_serial_control_get_data(&message, data);
    count = mavlink_msg_serial_control_get_count(&message);
    data[count] = '\0';
    _response.data += std::string((char*)data);

    size_t escape_pos = _response.data.find(27); // Find termination (ESC)
    if (escape_pos != std::string::npos) {
        if (_shell_message_timeout_cookie) {
            _parent->unregister_timeout_handler(_shell_message_timeout_cookie);
            _shell_message_timeout_cookie = nullptr;
        }

        _response.data.erase(_response.data.begin() + escape_pos, _response.data.end());

        if (_result_subscription) {
            auto callback = _result_subscription;
            Shell::ShellMessage arg = _response;
            _parent->call_user_callback(
                [callback, arg]() { callback(Shell::Result::SUCCESS, arg); });
        }
        _transfer_finished_promise.set_value();
    } else {
        if (_shell_message_timeout_cookie) {
            _parent->refresh_timeout_handler(_shell_message_timeout_cookie);
        }
    }
}

} // namespace mavsdk
