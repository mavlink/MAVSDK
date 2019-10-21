#pragma once

#include <functional>
#include <memory>
#include <string>

#include "plugin_base.h"

namespace mavsdk {

class CalibrationImpl;
class System;

/**
 * @brief The Calibration class enables to calibrate sensors of a drone such
 * as gyro, accelerometer, and magnetometer.
 */
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
    explicit Calibration(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Calibration();

    /**
     * @brief Possible results returned for calibration commands.
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
     * @brief Progress data coming from calibration.
     *
     * Can be a progress percentage, or an instruction text.
     */
    struct ProgressData {
        bool has_progress; ///< Whether or not this struct contains progress information.
        float progress; ///< The actual progress.
        bool has_status_text; ///< Whether or not this struct contains an instruction text.
        std::string status_text; ///< The actual instruction text.

        /**
         * Constructor for ProgressData.
         *
         * @param _has_progress Whether or not this struct contains progress information.
         * @param _progress The actual progress.
         * @param _has_status_text Whether or not this struct contains an instruction text.
         * @param _status_text The actual instruction text.
         */
        ProgressData(
            const bool _has_progress,
            const float _progress,
            const bool _has_status_text,
            const std::string& _status_text) :
            has_progress(_has_progress),
            progress(_progress),
            has_status_text(_has_status_text),
            status_text(_status_text)
        {}
    };

    /**
     * @brief Returns a human-readable English string for Calibration::Result.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the Calibration::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Callback type for asynchronous calibration call.
     */
    typedef std::function<void(const Result result, const ProgressData)> calibration_callback_t;

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
     * @brief Perform gimbal accelerometer calibration (asynchronous call).
     *
     * @param callback Function to receive result and progress of calibration.
     */
    void calibrate_gimbal_accelerometer_async(calibration_callback_t callback);

    /**
     * @brief Cancel ongoing calibration.
     *
     */
    void cancel_calibration();

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Calibration(const Calibration&) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Calibration& operator=(const Calibration&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<CalibrationImpl> _impl;
};

} // namespace mavsdk
