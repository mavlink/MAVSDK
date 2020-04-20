#pragma once

#include <functional>
#include <memory>

#include "plugin_base.h"

#ifdef ERROR
#undef ERROR
#endif

namespace mavsdk {

class GimbalImpl;
class System;

/**
 * @brief The Gimbal class provides control over a gimbal.
 *
 * Synchronous and asynchronous variants of the gimbal methods are supplied.
 */
class Gimbal : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto gimbal = std::make_shared<Gimbal>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Gimbal(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Gimbal();

    /**
     * @brief Possible results returned for gimbal commands.
     */
    enum class Result {
        UNKNOWN = 0, /**< @brief Unspecified error. */
        SUCCESS, /**< @brief Success. The gimbal command was accepted. */
        ERROR, /**< @brief Error. An error occured sending the command. */
        TIMEOUT /**< @brief Timeout. A timeout occured sending the command. */
    };

    /**
     * @brief Gimbal mode type.
     */
    enum class GimbalMode {
        YAW_FOLLOW, /**< @brief Yaw follow mode. */
        YAW_LOCK /**< @brief Yaw lock mode. */
    };

    /**
     * @brief Returns a human-readable English string for Gimbal::Result.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the Gimbal::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Callback type for asynchronous Gimbal calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Set gimbal pitch and yaw angles (synchronous).
     *
     * This sets the desired pitch and yaw angles of a gimbal.
     * The function will return when the command is accepted, however, it might
     * take the gimbal longer to actually be set to the new angles.
     *
     * @param pitch_deg The pitch angle in degrees. Negative to point down.
     * @param yaw_deg The yaw angle in degrees. Positive for clock-wise, range -180..180 or 0..360.
     * The yaw angle is relative to vehicle heading if the `GimbalMode` is `YAW_FOLLOW` and relative
     * to absolute North if the `GimbalMode` is `YAW_LOCK`.
     * @return Result of request.
     */
    Result set_pitch_and_yaw(float pitch_deg, float yaw_deg);

    /**
     * @brief Set gimbal pitch and yaw angles (asynchronous).
     *
     * This sets the desired pitch and yaw angles of a gimbal.
     * The callback will be called when the command is accepted, however, it might
     * take the gimbal longer to actually be set to the new angles.
     *
     * @param pitch_deg The pitch angle in degrees. Negative to point down.
     * @param yaw_deg The yaw angle in degrees. Positive for clock-wise, range -180..180 or 0..360.
     * The yaw angle is relative to vehicle heading if the `GimbalMode` is `YAW_FOLLOW` and relative
     * to absolute North if the `GimbalMode` is `YAW_LOCK`.
     * @param callback Function to call with result of request.
     */
    void set_pitch_and_yaw_async(float pitch_deg, float yaw_deg, result_callback_t callback);

    /**
     * @brief Set gimbal mode (synchronous).
     *
     * This sets the desired yaw mode of a gimbal.
     * The function will return when the command is accepted, however, it might
     * take the gimbal longer to actually be set to the new angles.
     *
     * @param gimbal_mode The mode to be set. Either yaw lock or yaw follow.
     * Yaw lock will fix the gimbal poiting to an absolute direction.
     * Yaw follow will point the gimbal to the vehicle heading.
     * @return Result of request.
     */
    Result set_gimbal_mode(const Gimbal::GimbalMode gimbal_mode);

    /**
     * @brief Set gimbal mode (asynchronous).
     *
     * This sets the desired yaw mode of a gimbal.
     * The function will return when the command is accepted, however, it might
     * take the gimbal longer to actually be set to the new angles.
     *
     * @param gimbal_mode The mode to be set. Either yaw lock or yaw follow.
     * Yaw lock will fix the gimbal poiting to an absolute direction.
     * Yaw follow will point the gimbal to the vehicle heading.
     * @param callback Function to call with result of request.
     */
    void set_gimbal_mode_async(const Gimbal::GimbalMode gimbal_mode, result_callback_t callback);

    /**
     * @brief Set gimbal region of interest (ROI).
     *
     * This sets a region of interest that the gimbal will point to.
     * The gimbal will continue to point to the specified region until it
     * receives a new command.
     * The function will return when the command is accepted, however, it might
     * take the gimbal longer to actually rotate to the ROI.
     *
     * @param latitude_deg Latitude in degrees.
     * @param longitude_deg Longitude in degrees.
     * @param altitude_m Altitude in meters (ASML).
     * @return Result of request.
     */
    Result set_roi_location(double latitude_deg, double longitude_deg, float altitude_m);

    /**
     * @brief Set gimbal region of interest (ROI) (asynchronous).
     *
     * This sets a region of interest that the gimbal will point to.
     * The gimbal will continue to point to the specified region until it
     * receives a new command.
     * The callback will be called when the command is accepted, however, it might
     * take the gimbal longer to actually be set to the new angles.
     *
     * @param latitude_deg Latitude in degrees.
     * @param longitude_deg Longitude in degrees.
     * @param altitude_m Altitude in meters (ASML).
     * @param callback Function to call with result of request.
     */
    void set_roi_location_async(
        double latitude_deg, double longitude_deg, float altitude_m, result_callback_t callback);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Gimbal(const Gimbal&) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Gimbal& operator=(const Gimbal&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<GimbalImpl> _impl;
};

} // namespace mavsdk
