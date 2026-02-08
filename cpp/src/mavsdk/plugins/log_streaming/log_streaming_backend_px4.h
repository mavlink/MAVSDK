#pragma once

#include "log_streaming_backend.h"
#include "system_impl.h"

#include <mutex>

namespace mavsdk {

class LogStreamingBackendPx4 : public LogStreamingBackend {
public:
    LogStreamingBackendPx4() = default;
    ~LogStreamingBackendPx4() override = default;

    void init(SystemImpl* system_impl) override;
    void deinit() override;

    void start_log_streaming_async(const LogStreaming::ResultCallback& callback) override;
    void stop_log_streaming_async(const LogStreaming::ResultCallback& callback) override;

    void set_data_callback(DataCallback callback) override;
    void set_debugging(bool debugging) override;

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

    SystemImpl* _system_impl{nullptr};
    DataCallback _data_callback{};

    std::mutex _mutex{};
    std::vector<uint8_t> _ulog_data{};
    DropState _drop_state{DropState::Unknown};
    unsigned _drops{0};
    uint16_t _current_sequence{0};
    bool _active{false};

    bool _debugging{false};
};

} // namespace mavsdk
