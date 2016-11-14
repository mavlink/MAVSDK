#pragma once

#include <functional>

namespace dronelink {

class LoggingImpl;

class Logging
{
public:
    explicit Logging(LoggingImpl *impl);
    ~Logging();

    enum class Result {
        SUCCESS = 0,
        NO_DEVICE,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        TIMEOUT,
        UNKNOWN
    };

    static const char *result_str(Result);

    typedef std::function<void(Result)> result_callback_t;

    Result start_logging() const;
    Result stop_logging() const;

    void start_logging_async(result_callback_t callback);
    void stop_logging_async(result_callback_t callback);

    // Non-copyable
    Logging(const Logging &) = delete;
    const Logging &operator=(const Logging &) = delete;

private:
    // Underlying implementation, set at instantiation
    LoggingImpl *_impl;
};

} // namespace dronelink
