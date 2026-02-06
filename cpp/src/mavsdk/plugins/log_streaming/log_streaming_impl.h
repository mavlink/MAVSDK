#pragma once

#include "plugins/log_streaming/log_streaming.h"

#include "call_every_handler.h"
#include "plugin_impl_base.h"
#include "callback_list.h"
#include "log_streaming_backend.h"

#include <memory>
#include <mutex>

namespace mavsdk {

class LogStreamingImpl : public PluginImplBase {
public:
    explicit LogStreamingImpl(System& system);
    explicit LogStreamingImpl(std::shared_ptr<System> system);

    ~LogStreamingImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void start_log_streaming_async(const LogStreaming::ResultCallback& callback);

    LogStreaming::Result start_log_streaming();

    void stop_log_streaming_async(const LogStreaming::ResultCallback& callback);

    LogStreaming::Result stop_log_streaming();

    LogStreaming::LogStreamingRawHandle
    subscribe_log_streaming_raw(const LogStreaming::LogStreamingRawCallback& callback);

    void unsubscribe_log_streaming_raw(LogStreaming::LogStreamingRawHandle handle);

private:
    bool maybe_create_backend();
    void process_data(const std::vector<uint8_t>& data);

    std::mutex _mutex{};
    std::unique_ptr<LogStreamingBackend> _backend{};
    CallbackList<LogStreaming::LogStreamingRaw> _subscription_callbacks{};

    bool _debugging{false};

    // For waiting on autopilot type during start
    LogStreaming::ResultCallback _start_callback{};
    CallEveryHandler::Cookie _check_autopilot_cookie{};
    unsigned _autopilot_poll_count{0};
};

} // namespace mavsdk
