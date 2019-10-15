#pragma once

#include <atomic>
#include <mutex>

#include "plugins/shell/shell.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class System;

class ShellImpl : public PluginImplBase {
public:
    ShellImpl(System& system);
    ~ShellImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void shell_command(const Shell::ShellMessage& shell_message);

    void shell_command_response_async(Shell::result_callback_t& callback);
    ShellImpl(const ShellImpl&) = delete;
    ShellImpl& operator=(const ShellImpl&) = delete;

private:
    bool send_shell_message_mavlink();

    void process_shell_message(const mavlink_message_t& message);

    void receive_shell_message_timeout();

    mutable std::mutex _shell_message_timeout_cookie_mutex{};
    void* _shell_message_timeout_cookie{nullptr};

    mutable std::mutex _result_subscription_mutex{};
    Shell::result_callback_t _result_subscription{nullptr};

    mutable std::mutex _shell_message_mutex{};
    Shell::ShellMessage _shell_message{};

    mutable std::mutex _result_mutex{};
    Shell::Result _result{};
};
} // namespace mavsdk