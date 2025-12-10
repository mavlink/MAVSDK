#pragma once

#include "log_streaming_backend.h"
#include "system_impl.h"

#include <mutex>

namespace mavsdk {

class LogStreamingBackendArdupilot : public LogStreamingBackend {
public:
    LogStreamingBackendArdupilot() = default;
    ~LogStreamingBackendArdupilot() override = default;

    void init(SystemImpl* system_impl) override;
    void deinit() override;

    void start_log_streaming_async(const LogStreaming::ResultCallback& callback) override;
    void stop_log_streaming_async(const LogStreaming::ResultCallback& callback) override;

    void set_data_callback(DataCallback callback) override;
    void set_debugging(bool debugging) override;

private:
    void process_remote_log_data_block(const mavlink_message_t& message);
    void send_remote_log_block_status(uint32_t seqno, uint8_t status);

    SystemImpl* _system_impl{nullptr};
    DataCallback _data_callback{};

    std::mutex _mutex{};
    bool _active{false};
    uint32_t _last_seqno{0};
    bool _first_block_received{false};

    bool _debugging{false};
};

} // namespace mavsdk
