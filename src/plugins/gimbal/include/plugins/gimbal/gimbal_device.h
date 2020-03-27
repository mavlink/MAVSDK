#pragma once

#include <functional>
#include <memory>

#include "plugin_base.h"

namespace mavsdk {

class GimbalDeviceImpl;
class System;

/**
 * @brief the GimbalDevice class provides callbacks for listening to gimbal device
 * commands. It also provides the ability to command a gimbal device, i. e. from
 * a custom gimbal manager.
 *
 * Warning: This is used for low level gimbal device control. this API should
 * only be used if you are 1) testing a gimbal manager or 2) writing a
 * custom gimbal manager, i. e. for a ground station.
 */
class GimbalDevice : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *      ```cpp
     *      auto gimbal_device = std::make_shared<GimbalDevice>(system);
     *      ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit GimbalDevice(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~GimbalDevice();

    /**
     * @brief Gimbal device capabilities.
     */
    enum class Capabilities {
        HAS_RETRACT = 1, /**< @brief Supports retracted mode. */
        HAS_NEUTRAL = 2, /**< @brief Supports neutral mode. */
        HAS_ROLL_AXIS = 4, /**< @brief Supports roll axis control. */
        HAS_ROLL_FOLLOW = 8, /**< @brief Supports follow mode for roll axis. */
        HAS_ROLL_LOCK = 16, /**< @brief Supports lock mode for roll axis. */
        HAS_PITCH_AXIS = 32, /**< @brief Supports pitch axis control. */
        HAS_PITCH_FOLLOW = 64, /**< @brief Supports follow mode for pitch axis. */
        HAS_PITCH_LOCK = 128, /**< @brief Supports lock mode for pitch axis. */
        HAS_YAW_AXIS = 256, /**< @brief Supports yaw axis control. */
        HAS_YAW_FOLLOW = 512, /**< @brief Supports follow mode for yaw axis. */
        HAS_YAW_LOCK = 1024, /**< @brief Supports lock mode for yaw axis. */
        HAS_INFINITE_YAW = 2048 /**< @brief Supports infinite yaw. */
    };

    /**
     * @brief Gimbal device operation flags.
     */
    enum class OperationFlags {
        RETRACT = 1, /**< @brief Retracted mode. */
        NEUTRAL = 2, /**< @brief Neutral mode. */
        ROLL_LOCK = 4, /**< @brief Lock roll axis. */
        PITCH_LOCK = 8, /**< @brief Lock pitch axis. */
        YAW_LOCK = 16, /**< @brief Lock yaw axis. */
    };

    /**
     * @brief Gimbal device error flags.
     */
    enum class Errors {
        AT_ROLL_LIMIT = 1, /**< @brief Gimbal device is limited by hardware roll limit. */
        AT_PITCH_LIMIT = 2, /**< @brief Gimbal device is limited by hardware pitch limit. */
        AT_YAW_LIMIT = 4, /**< @brief Gimbal device is limited by hardware yaw limit. */
        ENCODER_ERROR = 8, /**< @brief There is an error with the gimbal encoders. */
        POWER_ERROR = 8, /**< @brief There is an error with the gimbal power source. */
        MOTOR_ERROR = 8, /**< @brief There is an error with the gimbal motors. */
        SOFTWARE_ERROR = 8, /**< @brief There is an error with the gimbal's software. */
        COMMS_ERROR = 8, /**< @brief There is an error with the gimbal's communication. */
    };

    /**
     * @brief Possible results returned for gimbal commands.
     */
    enum class Result {
        UNKNOWN = 0, /**< @brief Unspecified error. */
        SUCCESS, /**< @brief Success. The gimbal command was accepted. */
        ERROR, /**< @brief Error. An error occured sending the command. */
        TIMEOUT, /**< @brief Timeout. A timouet occured sending the command */
    };

    /**
     * @brief Returns a human-readable English string for GimbalDevice::Result;
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the GimbalDevice::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Callback type for asynchronous Gimbal calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Set gimbal device attitude (synchronous).
     *
     * this sets the desired attitude of the connected gimbal.
     * The function will return when the command is accepted, however it might
     * take the gimbal longer to actually be set to the new attitude.
     *
     * @param operation_flags Flags to configure gimbal motion, see GimbalDevice::OperationFlags.
     * @param quat_w Quaternion w component.
     * @param quat_x Quaternion x component.
     * @param quat_y Quaternion y component.
     * @param quat_z Quaternion z component.
     * @param angular_velocity_x X component of angular velocity, positive is banking to the right, NaN to be ignored.
     * @param angular_velocity_y Y component of angular velocity, positive is tilting up, NaN to be ignored.
     * @param angular_velocity_z Z component of angular velocity, positive is panning to the right, NaN to be ignored.
     * @return Result of request.
     */
    Result set_attitude(int operation_flags,
            float quat_w, float quat_x, float quat_y, float quat_z,
            float angular_velocity_x, float angular_velocity_y, float angular_velocity_z);

    /**
     * @brief Set gimbal device attitude (synchronous).
     *
     * This sets the desired attitude of the connected gimbal.
     * The callback will be called when the command is accepted, however it might
     * take the gimbal longer to actually be set to the new attitude.
     *
     * @param operation_flags Flags to configure gimbal motion, see GimbalDevice::OperationFlags.
     * @param quat_w Quaternion w component.
     * @param quat_x Quaternion x component.
     * @param quat_y Quaternion y component.
     * @param quat_z Quaternion z component.
     * @param angular_velocity_x X component of angular velocity, positive is banking to the right, NaN to be ignored.
     * @param angular_velocity_y Y component of angular velocity, positive is tilting up, NaN to be ignored.
     * @param angular_velocity_z Z component of angular velocity, positive is panning to the right, NaN to be ignored.
     * @param callback Function to be called with result of request.
     */
    void set_attitude_async(int operation_flags, float quat_w, float quat_x, 
            float quat_y, float quat_z,
            float angular_velocity_x, float angular_velocity_y, float angular_velocity_z, result_callback_t callback);

    /** @brief Set gimbal device attitude (synchronous).
     *
     * Posts the current autopilot state to the connected gimbal device.
     *
     * @param timestamp Timestamp (time since boot of autopilot).
     * @param quat_w Quaternion w component.
     * @param quat_x Quaternion x component.
     * @param quat_y Quaternion y component.
     * @param quat_z Quaternion z component.
     * @param quat_estimated_delay_us Estimated delay of the attitude data.
     * @param vel_x X Speed in NED (North, East, Down).
     * @param vel_y Y Speed in NED (North, East, Down).
     * @param vel_z Z Speed in NED (North, East, Down).
     * @param vel_estimated_delay_us Estimated delay of the velocity data.
     * @param feed_forward_angular_velocity_z Feed forward Z component of angular velocity, positive is yawing to the right, NaN to be ignored. This is to indicate if the autopilot is actively yawing.
     */
    void post_autopilot_state(uint64_t timestamp,
            float quat_w, float quat_x, float quat_y, float quat_z,
            uint32_t quat_estimated_delay_us,
            float vel_x, float vel_y, float vel_z, uint32_t vel_estimated_delay_us,
            float feed_forward_angular_velocity_z);

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<GimbalDeviceImpl> _impl;
};

} // namespace mavsdk
