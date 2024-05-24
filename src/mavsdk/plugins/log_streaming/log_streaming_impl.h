#pragma once

#include "plugins/log_streaming/log_streaming.h"

#include "plugin_impl_base.h"
#include "callback_list.h"

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
    enum class DropState {
        Unknown,
        Dropped,
        RecoveringFromDropped,
        Ok,
    };

    void reset();
    void process_logging_data(const mavlink_message_t& message);
    void process_logging_data_acked(const mavlink_message_t& message);
    bool is_duplicate(uint16_t sequence) const;
    void check_drop_state(uint16_t sequence, uint8_t first_message);
    void process_message();

    static LogStreaming::Result
    log_streaming_result_from_command_result(MavlinkCommandSender::Result result);

    std::mutex _mutex{};
    CallbackList<LogStreaming::LogStreamingRaw> _subscription_callbacks{};
    std::vector<uint8_t> _ulog_data{};
    DropState _drop_state{DropState::Unknown};
    unsigned _drops{0};
    uint16_t _current_sequence{0};
    bool _active = false;

    bool _debugging{false};
};

} // namespace mavsdk
