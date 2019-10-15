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

void ShellImpl::deinit() {}

void ShellImpl::enable() {}

void ShellImpl::disable() {}

ShellImpl::ShellImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

ShellImpl::~ShellImpl()
{
    _parent->unregister_plugin(this);
}

void ShellImpl::shell_message_response_async(Shell::result_callback_t callback)
{
    _result_subscription = callback;
}

void ShellImpl::shell_command(const Shell::ShellMessage& shell_message)
{
    if (!_parent->is_connected()) {
        if (_result_subscription) {
            auto callback = _result_subscription;
            Shell::Result arg{Shell::Result::ResultCode::NO_SYSTEM, {}};
            _parent->call_user_callback([callback, arg]() { callback(arg); });
            return;
        }
    }

    _shell_message_mutex.lock();
    _shell_message = shell_message;
    _shell_message_mutex.unlock();

    if (!send_shell_message_mavlink()) {
        if (_result_subscription) {
            auto callback = _result_subscription;
            Shell::Result arg{Shell::Result::ResultCode::CONNECTION_ERROR, {}};
            _parent->call_user_callback([callback, arg]() { callback(arg); });
            return;
        }
    }
    if (_result_subscription) {
        if (!shell_message.need_response) {
            auto callback = _result_subscription;
            Shell::Result arg{Shell::Result::ResultCode::SUCCESS, {}};
            _parent->call_user_callback([callback, arg]() { callback(arg); });
        } else {
            if (_shell_message_timeout_cookie) {
                _parent->unregister_timeout_handler(_shell_message_timeout_cookie);
                _shell_message_timeout_cookie = nullptr;
            }
            _parent->register_timeout_handler(
                std::bind(&ShellImpl::receive_shell_message_timeout, this),
                shell_message.timeout / 1000,
                &_shell_message_timeout_cookie);
        }
    }
}

void ShellImpl::receive_shell_message_timeout()
{
    std::lock_guard<std::mutex> lock_result(_result_mutex);

    if (_result_subscription) {
        auto callback = _result_subscription;
        Shell::Result arg = _result;
        _parent->call_user_callback([callback, arg]() { callback(arg); });
    }
    _parent->unregister_timeout_handler(_shell_message_timeout_cookie);
    _shell_message_timeout_cookie = nullptr;
    _result.result_code = Shell::Result::ResultCode::NO_RESPONSE;
    _result.response = Shell::ShellMessage{};
}

bool ShellImpl::send_shell_message_mavlink()
{
    _shell_message_mutex.lock();
    auto shell_message = _shell_message;
    _shell_message_mutex.unlock();

    mavlink_message_t message;

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
    int count = 0;
    uint8_t data[MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN + 1]{0};

    std::lock_guard<std::mutex> lock_result(_result_mutex);

    _result.result_code = Shell::Result::ResultCode::SUCCESS;

    _result.response.timeout = mavlink_msg_serial_control_get_timeout(&message);
    _result.response.need_response =
        mavlink_msg_serial_control_get_flags(&message) & SERIAL_CONTROL_FLAG_RESPOND;

    mavlink_msg_serial_control_get_data(&message, data);
    count = mavlink_msg_serial_control_get_count(&message);
    data[count] = '\0';
    _result.response.data += std::string((char*)data);

    // In fact max bytes amount is 69.
    // See implementation of read()
    // https://github.com/PX4/Firmware/blob/master/src/modules/mavlink/mavlink_shell.h#L73
    if (count < (MAVLINK_MSG_SERIAL_CONTROL_FIELD_DATA_LEN - 1)) {
        if (_result_subscription) {
            auto callback = _result_subscription;
            Shell::Result arg = _result;
            _parent->call_user_callback([callback, arg]() { callback(arg); });
        }
        if (_shell_message_timeout_cookie) {
            _parent->unregister_timeout_handler(_shell_message_timeout_cookie);
            _shell_message_timeout_cookie = nullptr;
        }
        _result.result_code = Shell::Result::ResultCode::NO_RESPONSE;
        _result.response = Shell::ShellMessage{};
    } else {
        if (_shell_message_timeout_cookie) {
            _parent->refresh_timeout_handler(_shell_message_timeout_cookie);
        }
    }
}

} // namespace mavsdk