#include "shell_impl.h"
#include "system.h"
#include "global_include.h"
#include "px4_custom_mode.h"
#include <cmath>
#include <functional>
#include <array>
#include "log.h"

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
    _transfer_closed_future(_transfer_closed_promise.get_future())
{
    _parent->register_plugin(this);
    _transfer_closed_promise.set_value();
}

ShellImpl::~ShellImpl()
{
    _parent->unregister_plugin(this);
}

bool ShellImpl::is_transfer_in_progress()
{
    if (!_transfer_closed_future.valid()) {
        return true;
    }
    return _transfer_closed_future.wait_for(std::chrono::seconds(0)) != std::future_status::ready;
}

void ShellImpl::shell_command_response_async(Shell::result_callback_t& callback)
{
    {
        std::lock_guard<std::mutex> lock(_future_mutex);
        if (is_transfer_in_progress()) {
            return;
        }
    }
    _result_subscription = callback;
}

void ShellImpl::send_to_subscription(Shell::Result arg)
{
    if (_result_subscription) {
        auto callback = _result_subscription;
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
}

void ShellImpl::shell_command(const Shell::ShellMessage& shell_message)
{
    {
        std::lock_guard<std::mutex> lock(_future_mutex);
        if (is_transfer_in_progress()) {
            return;
        }

        _transfer_closed_promise = std::promise<void>();
        _transfer_closed_future = _transfer_closed_promise.get_future();
    }

    if (!_parent->is_connected()) {
        send_to_subscription(Shell::Result{Shell::Result::ResultCode::NO_SYSTEM, {}});
        _transfer_closed_promise.set_value();
        return;
    }

    _shell_message = shell_message;

    if (!send_shell_message_mavlink()) {
        send_to_subscription(Shell::Result{Shell::Result::ResultCode::CONNECTION_ERROR, {}});
        _transfer_closed_promise.set_value();
        return;
    }

    if (_result_subscription) {
        if (!shell_message.need_response) {
            send_to_subscription(Shell::Result{Shell::Result::ResultCode::SUCCESS, {}});
            _transfer_closed_promise.set_value();
        } else {
            if (_shell_message_timeout_cookie) {
                _parent->unregister_timeout_handler(_shell_message_timeout_cookie);
                _shell_message_timeout_cookie = nullptr;
            }
            _parent->register_timeout_handler(
                std::bind(&ShellImpl::receive_shell_message_timeout, this),
                shell_message.timeout / 1000.0,
                &_shell_message_timeout_cookie);
        }
    } else {
        _transfer_closed_promise.set_value();
    }
}

void ShellImpl::receive_shell_message_timeout()
{
    send_to_subscription(_result);
    _parent->unregister_timeout_handler(_shell_message_timeout_cookie);
    _shell_message_timeout_cookie = nullptr;
    _result.result_code = Shell::Result::ResultCode::NO_RESPONSE;
    _result.response = Shell::ShellMessage{};

    if (is_transfer_in_progress()) {
        _transfer_closed_promise.set_value();
    }
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
    if (!is_transfer_in_progress()) { // Skip symbols after ESC
        return;
    }
    int count = 0;
    uint8_t data[MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN + 1]{0};

    _result.result_code = Shell::Result::ResultCode::SUCCESS;

    _result.response.timeout = mavlink_msg_serial_control_get_timeout(&message);
    _result.response.need_response =
        mavlink_msg_serial_control_get_flags(&message) & SERIAL_CONTROL_FLAG_RESPOND;

    mavlink_msg_serial_control_get_data(&message, data);
    count = mavlink_msg_serial_control_get_count(&message);
    data[count] = '\0';
    _result.response.data += std::string((char*)data);

    size_t escape_pos = _result.response.data.find('\e'); // Find termination (ESC)
    if (escape_pos != std::string::npos) {
        _result.response.data.erase(
            _result.response.data.begin() + escape_pos, _result.response.data.end());

        send_to_subscription(_result);
        if (_shell_message_timeout_cookie) {
            _parent->unregister_timeout_handler(_shell_message_timeout_cookie);
            _shell_message_timeout_cookie = nullptr;
        }
        _result.result_code = Shell::Result::ResultCode::NO_RESPONSE;
        _result.response = Shell::ShellMessage{};

        if (is_transfer_in_progress()) {
            _transfer_closed_promise.set_value();
        }
    } else {
        if (_shell_message_timeout_cookie) {
            _parent->refresh_timeout_handler(_shell_message_timeout_cookie);
        }
    }
}

} // namespace mavsdk