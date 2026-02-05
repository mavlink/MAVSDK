#pragma once

#include "plugins/log_streaming/log_streaming.h"

#include <functional>
#include <vector>

namespace mavsdk {

class SystemImpl;

class LogStreamingBackend {
public:
    virtual ~LogStreamingBackend() = default;

    virtual void init(SystemImpl* system_impl) = 0;
    virtual void deinit() = 0;

    virtual void start_log_streaming_async(const LogStreaming::ResultCallback& callback) = 0;
    virtual void stop_log_streaming_async(const LogStreaming::ResultCallback& callback) = 0;

    using DataCallback = std::function<void(const std::vector<uint8_t>&)>;
    virtual void set_data_callback(DataCallback callback) = 0;

    virtual void set_debugging(bool debugging) = 0;
};

} // namespace mavsdk
