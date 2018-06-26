#pragma once

#include <functional>
#include "plugin_base.h"

namespace dronecore {

class CalibrationImpl;
class System;

class Calibration : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto calibration = std::make_shared<Calibration>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Calibration(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Calibration();

    /**
     * @brief Possible results returned for camera commands.
     */
    enum class Result {
        UNKNOWN,
        SUCCESS,
        IN_PROGRESS,
        INSTRUCTION,
        FAILED,
        NO_SYSTEM,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        TIMEOUT,
        CANCELLED
    };

    /**
     * @brief Returns a human-readable English string for Calibration::Result.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the Calibration::Result.
     */
    static const char *result_str(Result result);

    /**
     * @brief Callback type for asynchronous calibration call.
     */
    typedef std::function<void(Result result, float progress, const std::string &text)>
        calibration_callback_t;

    /**
     * @brief Perform gyro calibration (asynchronous call).
     *
     * @param callback Function to receive result and progress of calibration.
     */
    void calibrate_gyro_async(calibration_callback_t callback);

    /**
     * @brief Perform accelerometer calibration (asynchronous call).
     *
     * @param callback Function to receive result and progress of calibration.
     */
    void calibrate_accelerometer_async(calibration_callback_t callback);

    /**
     * @brief Perform magnetometer calibration (asynchronous call).
     *
     * @param callback Function to receive result and progress of calibration.
     */
    void calibrate_magnetometer_async(calibration_callback_t callback);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Calibration(const Calibration &) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Calibration &operator=(const Calibration &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    CalibrationImpl *_impl;
};

} // namespace dronecore
