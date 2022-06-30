#pragma once

#include <mutex>

#include "plugins/shell/shell.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "system.h"
#include "callback_list.h"

namespace mavsdk {

class System;

class ShellImpl : public PluginImplBase {
public:
    explicit ShellImpl(System& system);
    explicit ShellImpl(std::shared_ptr<System> system);
    ~ShellImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Shell::Result send(std::string command);
    Shell::ReceiveHandle subscribe_receive(const Shell::ReceiveCallback& callback);
    void unsubscribe_receive(Shell::ReceiveHandle handle);

    ShellImpl(const ShellImpl&) = delete;
    ShellImpl& operator=(const ShellImpl&) = delete;

private:
    bool send_command_message(std::string command);
    void process_shell_message(const mavlink_message_t& message);

    static constexpr uint16_t timeout_ms = 1000;

    struct {
        std::mutex mutex{};
        CallbackList<std::string> callbacks{};
    } _receive{};
};
} // namespace mavsdk
