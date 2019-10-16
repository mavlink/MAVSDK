#pragma once

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

    bool is_transfer_in_progress();

    std::promise<void> _transfer_closed_promise{};
    std::future<void> _transfer_closed_future{};

    std::mutex _future_mutex{};

    void* _shell_message_timeout_cookie{nullptr};

    Shell::result_callback_t _result_subscription{nullptr};

    void send_to_subscription(Shell::Result arg);

    Shell::ShellMessage _shell_message{};

    Shell::Result _result{};
};
} // namespace mavsdk