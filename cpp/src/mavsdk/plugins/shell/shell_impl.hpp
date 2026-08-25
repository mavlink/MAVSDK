#pragma once

#include <atomic>
#include <optional>

#include "plugins/shell/shell.hpp"
#include "mavlink_include.hpp"
#include "plugin_impl_base.hpp"
#include "system.hpp"
#include "callback_list.hpp"

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
    Shell::Result set_device(Shell::Device device);
    Shell::ReceiveHandle subscribe_receive(const Shell::ReceiveCallback& callback);
    void unsubscribe_receive(Shell::ReceiveHandle handle);

    ShellImpl(const ShellImpl&) = delete;
    ShellImpl& operator=(const ShellImpl&) = delete;

private:
    bool send_command_message(std::string command);
    void process_shell_message(const mavlink_message_t& message);
    static std::optional<uint8_t> device_to_mavlink(Shell::Device device);

    static constexpr uint16_t timeout_ms = 1000;

    struct Receive {
        explicit Receive(asio::io_context& io_context) : callbacks(io_context) {}
        CallbackList<std::string> callbacks;
    } _receive;

    std::atomic<uint8_t> _device{
        static_cast<uint8_t>(SERIAL_CONTROL_DEV::SERIAL_CONTROL_DEV_SHELL)};
};
} // namespace mavsdk
