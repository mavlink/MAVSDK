#pragma once

#include "plugins/transponder/transponder.h"
#include "plugin_impl_base.h"
#include "callback_list.h"

namespace mavsdk {

class TransponderImpl : public PluginImplBase {
public:
    explicit TransponderImpl(System& system);
    explicit TransponderImpl(std::shared_ptr<System> system);
    ~TransponderImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Transponder::Result set_rate_transponder(double rate_hz);

    void set_rate_transponder_async(double rate_hz, const Transponder::ResultCallback callback);

    Transponder::AdsbVehicle transponder() const;

    Transponder::TransponderHandle
    subscribe_transponder(const Transponder::TransponderCallback& callback);
    void unsubscribe_transponder(Transponder::TransponderHandle handle);

private:
    void set_transponder(Transponder::AdsbVehicle transponder);

    void process_transponder(const mavlink_message_t& message);

    static Transponder::Result
    transponder_result_from_command_result(MavlinkCommandSender::Result command_result);

    static void command_result_callback(
        MavlinkCommandSender::Result command_result, const Transponder::ResultCallback& callback);

    mutable std::mutex _transponder_mutex{};
    Transponder::AdsbVehicle _transponder{};

    std::mutex _subscription_mutex{};
    CallbackList<Transponder::AdsbVehicle> _transponder_subscriptions{};
};

} // namespace mavsdk
