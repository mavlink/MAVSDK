#pragma once

#include <functional>
#include "plugin_base.h"

namespace dronecore {

class LoggingImpl;
class Device;

/**
 * @brief The Logging class allows log data using logger and log streaming from the vehicle.
 *
 * **This feature is not yet implemented**.
 */
class Logging : public PluginBase
{
public:
    /**
     * @brief Constructor (internal use only).
     */
    explicit Logging(Device *device);

    /**
     * @brief Destructor (internal use only).
     */
    ~Logging();

    /**
     * @brief Results for logging requests.
     */
    enum class Result {
        SUCCESS = 0, /**< @brief %Request succeeded. */
        NO_DEVICE, /**< @brief No device connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        BUSY, /**< @brief %Device busy. */
        COMMAND_DENIED, /**< @brief Command denied. */
        TIMEOUT, /**< @brief Timeout. */
        UNKNOWN /**< @brief Unknown error. */
    };

    /**
     * @brief Returns human-readable English string for Logging::Result.
     *
     * @param result Enum for which string is required.
     * @return result Human-readable string for Logging::Result.
     */
    static const char *result_str(Result result);

    /**
     * @brief Callback type for logging requests.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Start logging (synchronous).
     *
     * **This feature is not yet implemented**.
     *
     * @return Result of request.
     */
    Result start_logging() const;

    /**
     * @brief Stop logging (synchronous).
     *
     * **This feature is not yet implemented**.
     *
     * @return Result of request.
     */
    Result stop_logging() const;

    /**
     * @brief Start logging (asynchronous).
     *
     * **This feature is not yet implemented**.
     *
     * @param callback Callback to get result of request.
     */
    void start_logging_async(result_callback_t callback);

    /**
     * @brief Stop logging (asynchronous).
     *
     * **This feature is not yet implemented**.
     *
     * @param callback Callback to get result of request.
     */
    void stop_logging_async(result_callback_t callback);

    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    Logging(const Logging &) = delete;
    /**
    * @brief Equality operator (object is not copyable).
    */
    const Logging &operator=(const Logging &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    LoggingImpl *_impl;
};

} // namespace dronecore
