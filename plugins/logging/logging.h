#pragma once

#include <functional>

namespace dronecore {

class LoggingImpl;

/**
 * The Logging class allows log data using logger and log streaming from the vehicle.
 *
 * Note that this feature is not implemented yet.
 */
class Logging
{
public:
    /**
     * Constructor for Logging called internally.
     */
    explicit Logging(LoggingImpl *impl);

    /**
     * Destructor for Logging called internally.
     */
    ~Logging();

    /**
     * Results for logging requests.
     */
    enum class Result {
        SUCCESS = 0,
        NO_DEVICE,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        TIMEOUT,
        UNKNOWN
    };

    /**
     * Returns human-readable English string for `Logging::Result`.
     *
     * @param result result enum
     */
    static const char *result_str(Result result);

    /**
     * Callback type for logging requests.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * Start logging (synchronous).
     *
     * Note this is not implemented yet.
     *
     * @return result of request
     */
    Result start_logging() const;

    /**
     * Stop logging (synchronous).
     *
     * Note this is not implemented yet.
     *
     * @return result of request
     */
    Result stop_logging() const;

    /**
     * Start logging (asynchronous).
     *
     * Note this is not implemented yet.
     *
     * @param callback to get result of request
     */
    void start_logging_async(result_callback_t callback);

    /**
     * Stop logging (asynchronous).
     *
     * Note this is not implemented yet.
     *
     * @param callback to get result of request
     */
    void stop_logging_async(result_callback_t callback);

    // Non-copyable
    Logging(const Logging &) = delete;
    const Logging &operator=(const Logging &) = delete;

private:
    // Underlying implementation, set at instantiation
    LoggingImpl *_impl;
};

} // namespace dronecore
