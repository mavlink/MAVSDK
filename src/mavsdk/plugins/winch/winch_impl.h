#pragma once

#include <mutex>

#include "plugins/winch/winch.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "callback_list.h"

namespace mavsdk {

class WinchImpl : public PluginImplBase {
public:
    explicit WinchImpl(System& system);
    explicit WinchImpl(std::shared_ptr<System> system);

    ~WinchImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void relax_async(uint32_t instance, const Winch::ResultCallback callback);
    Winch::Result relax(uint32_t instance);

    void relative_length_control_async(
        uint32_t instance, float length, float rate, const Winch::ResultCallback callback);
    Winch::Result relative_length_control(uint32_t instance, float length, float rate);

    void rate_control_async(uint32_t instance, float rate, const Winch::ResultCallback callback);
    Winch::Result rate_control(uint32_t instance, float rate);

    void lock_async(uint32_t instance, const Winch::ResultCallback callback);
    Winch::Result lock(uint32_t instance);

    void deliver_async(uint32_t instance, const Winch::ResultCallback callback);
    Winch::Result deliver(uint32_t instance);

    void hold_async(uint32_t instance, const Winch::ResultCallback callback);
    Winch::Result hold(uint32_t instance);

    void retract_async(uint32_t instance, const Winch::ResultCallback callback);
    Winch::Result retract(uint32_t instance);

    void load_line_async(uint32_t instance, const Winch::ResultCallback callback);
    Winch::Result load_line(uint32_t instance);

    void abandon_line_async(uint32_t instance, const Winch::ResultCallback callback);
    Winch::Result abandon_line(uint32_t instance);

    void load_payload_async(uint32_t instance, const Winch::ResultCallback callback);
    Winch::Result load_payload(uint32_t instance);

    Winch::Status status() const;

    Winch::StatusHandle subscribe_status(const Winch::StatusCallback& callback);
    void unsubscribe_status(Winch::StatusHandle handle);

private:
    static Winch::Result winch_result_from_command_result(MavlinkCommandSender::Result result);

    void command_result_callback(
        MavlinkCommandSender::Result command_result, const Winch::ResultCallback& callback) const;

    void set_status(Winch::Status status);

    void process_status(const mavlink_message_t& message);

    mutable std::mutex _status_mutex{};
    Winch::Status _status{};

    std::mutex _subscription_mutex{};

    CallbackList<Winch::Status> _status_subscriptions{};
};

} // namespace mavsdk