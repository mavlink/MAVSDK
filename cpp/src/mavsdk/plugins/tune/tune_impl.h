#pragma once

#include "mavlink_include.h"
#include "plugins/tune/tune.h"
#include "plugin_impl_base.h"
#include "system.h"

namespace mavsdk {

class TuneImpl : public PluginImplBase {
public:
    explicit TuneImpl(System& system);
    explicit TuneImpl(std::shared_ptr<System> system);
    ~TuneImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Tune::Result play_tune(const Tune::TuneDescription& tune);
    void play_tune_async(const Tune::TuneDescription& tune, const Tune::ResultCallback& callback);

    // Non-copyable
    TuneImpl(const TuneImpl&) = delete;
    const TuneImpl& operator=(const TuneImpl&) = delete;

private:
    void timeout_happened();

    void report_tune_result(const Tune::ResultCallback& callback, Tune::Result result);

    void receive_command_result(
        MavlinkCommandSender::Result command_result, const Tune::ResultCallback& callback);

    Tune::ResultCallback _result_callback = nullptr;

    std::vector<std::shared_ptr<mavlink_message_t>> _mavlink_tune_item_messages;
};

} // namespace mavsdk
