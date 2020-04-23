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

    Shell::Result send(std::string command);
    void receive_async(Shell::receive_callback_t callback);

    ShellImpl(const ShellImpl&) = delete;
    ShellImpl& operator=(const ShellImpl&) = delete;

private:
    bool send_command_message(std::string command);
    void process_shell_message(const mavlink_message_t& message);

    static constexpr uint16_t timeout_ms = 1000;

    struct {
        std::mutex mutex{};
        Shell::receive_callback_t callback{nullptr};
    } _receive{};
};
} // namespace mavsdk
