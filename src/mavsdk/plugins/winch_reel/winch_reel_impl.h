#pragma once

#include <mutex>

#include "plugins/winch_reel/winch_reel.h"
#include "mavlink_include.h"
#include "plugin_impl_base.h"
#include "callback_list.h"

namespace mavsdk {

class WinchReelImpl : public PluginImplBase {
public:
    explicit WinchReelImpl(System& system);
    explicit WinchReelImpl(std::shared_ptr<System> system);

    ~WinchReelImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void relax_async(uint32_t instance, const WinchReel::ResultCallback callback);
    WinchReel::Result relax(uint32_t instance);

    void lock_async(uint32_t instance, const WinchReel::ResultCallback callback);
    WinchReel::Result lock(uint32_t instance);

    void deliver_async(uint32_t instance, const WinchReel::ResultCallback callback);
    WinchReel::Result deliver(uint32_t instance);

    void hold_async(uint32_t instance, const WinchReel::ResultCallback callback);
    WinchReel::Result hold(uint32_t instance);

    void retract_async(uint32_t instance, const WinchReel::ResultCallback callback);
    WinchReel::Result retract(uint32_t instance);

    void load_line_async(uint32_t instance, const WinchReel::ResultCallback callback);
    WinchReel::Result load_line(uint32_t instance);

    void load_payload_async(uint32_t instance, const WinchReel::ResultCallback callback);
    WinchReel::Result load_payload(uint32_t instance);

    WinchReel::Status status() const;

    WinchReel::StatusHandle subscribe_status(const WinchReel::StatusCallback& callback);
    void unsubscribe_status(WinchReel::StatusHandle handle);

private:
    static WinchReel::Result winch_result_from_command_result(MavlinkCommandSender::Result result);

    void command_result_callback(
        MavlinkCommandSender::Result command_result, const WinchReel::ResultCallback& callback) const;

    void set_status(WinchReel::Status status);

    void process_status(const mavlink_message_t& message);

    mutable std::mutex _status_mutex{};
    WinchReel::Status _status{};

    std::mutex _subscription_mutex{};

    CallbackList<WinchReel::Status> _status_subscriptions{};
};

} // namespace mavsdk
