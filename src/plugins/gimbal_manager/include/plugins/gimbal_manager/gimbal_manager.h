#pragma once

#include <functional>
#include <memory>

#include "plugin_base.h"

namespace mavsdk {

class GimbalManagerImpl;
class System;

/**
 * @brief The GimbalManager class interfaces with a MAVLink gimbal manager.
 *
 * Synchronous and asynchronous variants of the interface methods are supplied.
 */
class GimbalManager : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto gimbal_manager = std::make_shared<GimbalManager>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit GimbalManager(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~GimbalManager();

    /**
     * @brief Gimbal manager capabilities.
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
        HAS_INFINITE_YAW = 2048, /**< @brief Supports infinite yaw. */
        CAN_POINT_LOCATION_LOCAL = 65536, /**< @brief Supports pointing to a local position. */
        CAN_POINT_LOCATION_GLOBAL = 131072, /**< @brief Supports pointing to a global latitude, longitude, altitude position. */
        HAS_TRACKING_POINT = 262144, /**< @brief Supports tracking of a point on the camera. */
        HAS_TRACKING_RECTANGLE = 524288, /**< @brief Supports tracking of a rectangle on the camera. */
        SUPPORTS_FOCAL_LENGTH_SCALE = 1048576, /**< @brief Supports pitching and yawing at an angular velocity scaled by focal length (the more zoomed in, the slower the movement). */
        SUPPORTS_NUDGING = 2097152, /**< @brief Supports nudging when pointing to a location or tracking. */
        SUPPORTS_OVERRIDE = 4194304 /**< @brief Supports overriding when pointing to a location or tracking. */
    };

    /**
     * @brief Gimbal manager operation flags.
     */
    enum class OperationFlags {
        RETRACT = 1, /**< @brief Retracted mode. */
        NEUTRAL = 2, /**< @brief Neutral mode. */
        ROLL_LOCK = 4, /**< @brief Lock roll axis. */
        PITCH_LOCK = 8, /**< @brief Lock pitch axis. */
        YAW_LOCK = 16, /**< @brief Lock yaw axis. */
        ANGULAR_VELOCITY_RELATIVE_TO_FOCAL_LENGTH = 1048576, /**< @brief Scale angular velocity relative to focal length. This means the gimbal moves slower if it is zoomed in. */
        NUDGE = 2097152, /**< @brief Interpret attitude control on top of pointing to a location or tracking. If this flag is set, the quaternion is relative to the existing tracking angle. */
        OVERRIDE = 4194304, /**< @brief Completely override pointing to a location or tracking. If this flag is set, the quaternion is (as usual) according to OperationFlags::YAW_LOCK. */
        NONE = 8388608 /**< @brief Give up control previously set using GimbalManager::set_attitude(). Do not mix with other flags. */
    };

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
     * @brief Returns a human-readable English string for GimbalManager::Result.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the GimbalManager::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Callback type for asynchronous GimbalManager calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Id type for storing system/component id of the manager
     * as well as which gimbal device it is associated with.
     *
     * Note that gimbal managers usually don't have their own component,
     * and are usually hosted on an autopilot, companion computer,
     * or the gimbal device itself.
     */
    struct ManagerId {
        uint8_t system_id; /**< @brief System ID of gimbal manager. */
        uint8_t component_id; /**< @brief Component ID of gimbal manager. */
        uint8_t assoc_device; /*< @brief Gimbal component ID this gimbal manager is responsible for. */
    };

    /**
     * @brief Information type for storing information (capabilities, ID, etc)
     * about gimbal managers.
     */
    struct Information {
        uint32_t capabilities; /**< @brief Capabilities of the gimbal manager. */
        ManagerId id; /**< @brief Manager identification. */
        float tilt_max; /**< @brief Maximum tilt/pitch angle (positive: up, negative: down) */
        float tilt_min; /**< @brief Minimum tilt/pitch angle (positive: up, negative: down) */
        float tilt_rate_max; /**< @brief Maximum tilt/pitch angular rate (positive: up, negative: down) */
        float pan_max; /**< @brief Maximum pan/yaw angle (positive: up, negative: down) */
        float pan_min; /**< @brief Minimum pan/yaw angle (positive: up, negative: down) */
        float pan_rate_max; /**< @brief Maximum pan/yaw angular rate (positive: up, negative: down) */
    };

    /**
     * @brief Requests information about the gimbal manager.
     *
     * This sends a request for gimbal manager information.
     * The functon will return immediately. To capture the response (information),
     * see GimbalManager::subscribe_information_async.
     *
     * @return Result of request.
     */

    Result request_information();

    /**
     * @brief Subscribe to gimbal manager information responses.
     * To request information from any/all gimbal managers, use GimbalManager::request_information.
     * 
     * Note: This only works if the manager class is not bound to a specific gimbal manager component.
     *
     * @param callback Callback to be called when information is received.
     */
    void subscribe_information_async(
            std::function<void(const Information information)> callback);

    /**
     * @brief Set gimbal attitude (synchronous).
     *
     * This sets the desired attitude of the specified gimbal.
     * The function will return when the command is accepted, however, it might
     * take the gimbal(s) longer to actually be set to the new attitude.
     *
     * @param tilt_rate The angular rate to tilt the gimbal, in degs/s. If tilt_deg is not NaN,
     * the gimbal will rotate to tilt_deg at this rate. If tilt_rate is NaN, the gimbal will
     * continuously turn at this rate.
     * @param pan_rate The angular rate to pan the gimbal, in degs/s. If pan_deg is not NaN,
     * the gimbal will rotate to pan_deg at this rate. If pan_rate is NaN, the gimbal will
     * continuously turn at this rate.
     * @param tilt_deg Angle to tilt to, in degrees. Set to NaN to leave unchanged, or if
     * tilt_rate is set, to enable continuous rotation.
     * @param pan_deg Angle to pan to, in degrees. Set to NaN to leave unchanged, or if
     * pan_rate is set, to enable continuous rotation.
     * @param operation_flags Additional flags to configure gimbal motion, see GimbalManager::OperationFlags.
     * @param id Id of gimbal device to control. Set to one of MAV_COMPONENT_ID_GIMBAL[1-6]
     * for a MAVLink enabled gimbal device, [1-6] for a non-MAVLink gimbal device,
     * or 0 to command all connected gimbal devices.
     * @return Result of request.
     */
    Result set_attitude(float tilt_rate, float pan_rate, float tilt_deg,
                        float pan_deg, int operation_flags, uint8_t id);

    /**
     * @brief Set gimbal attitude (asynchronous).
     *
     * This sets the desired attitude of the specified gimbal.
     * The callback will be called when the command is accepted, however, it might
     * take the gimbal device longer to actually be set to the new attitude.
     *
     * @param tilt_rate The angular rate to tilt the gimbal, in degs/s. If tilt_deg is not NaN,
     * the gimbal will rotate to tilt_deg at this rate. If tilt_rate is NaN, the gimbal will
     * continuously turn at this rate.
     * @param pan_rate The angular rate to pan the gimbal, in degs/s. If pan_deg is not NaN,
     * the gimbal will rotate to pan_deg at this rate. If pan_rate is NaN, the gimbal will
     * continuously turn at this rate.
     * @param tilt_deg Angle to tilt to, in degrees. Set to NaN to leave unchanged, or if
     * tilt_rate is set, to enable continuous rotation.
     * @param pan_deg Angle to pan to, in degrees. Set to NaN to leave unchanged, or if
     * pan_rate is set, to enable continuous rotation.
     * @param operation_flags Additional flags to configure gimbal motion, see GimbalManager::OperationFlags.
     * @param id Id of gimbal device to control. Set to one of MAV_COMPONENT_ID_GIMBAL[1-6]
     * for a MAVLink enabled gimbal device, [1-6] for a non-MAVLink gimbal device,
     * or 0 to command all connected gimbal devices.
     * @param callback Function to be called with result of request.
     */
    void set_attitude_async(float tilt_rate, float pan_rate, float tilt_deg,
                        float pan_deg, int operation_flags, uint8_t id,
                        result_callback_t callback);

    /**
     * @brief Binds the manager class to a specific gimbal manager
     *
     * @param information Information about the connected gimbal manager, includings its system/component ID and associated gimbal device.
     */
    void bind(Information information);

    /**
     * @brief Unbinds the manager class from a specific gimbal manager.
     */
    void unbind();

    /**
     * @brief Track camera point.
     *
     * This commands the specified gimbal to track a point in a camera view.
     * The function will return when the command is accepted, however, it might
     * take the gimbal longer to actually be set to the new angles.
     *
     * @param x X position of camera point to track, normalized from 0..1. 0 is left, 1 is right
     * @param y Y position of camera point to track, normalized from 0..1. 0 is left, 1 is right
     * @param id Id of gimbal device to control. Set to one of MAV_COMPONENT_ID_GIMBAL[1-6]
     * for a MAVLink enabled gimbal device, [1-6] for a non-MAVLink gimbal device,
     * or 0 to command all connected gimbal devices.
     * @return Result of request.
     */
    // TODO consider adding this back later
    //Result track_point(float x, float y, uint8_t id);

    /**
     * @brief Track camera point.
     *
     * This commands the specified gimbal to track a point in a camera view.
     * The callback will be called when the command is accepted, however, it might
     * take the gimbal longer to actually be set to the new angles.
     *
     * @param x X position of camera point to track, normalized from 0..1. 0 is left, 1 is right
     * @param y Y position of camera point to track, normalized from 0..1. 0 is left, 1 is right
     * @param id Id of gimbal device to control. Set to one of MAV_COMPONENT_ID_GIMBAL[1-6]
     * for a MAVLink enabled gimbal device, [1-6] for a non-MAVLink gimbal device,
     * or 0 to command all connected gimbal devices.
     * @param callback Function to call with result of request.
     */
    // TODO consider adding this back later
    //void track_point_async(float x, float y, uint8_t id, result_callback_t callback);
    
    /**
     * @brief Track camera rectangle.
     *
     * This commands the specified gimbal to track a rectangle in a camera view.
     * The function will return when the command is accepted, however, it might
     * take the gimbal longer to actually be set to the new angles.
     *
     * @param x1 X value of top left corner of rectangle to track, normalized from 0..1. 0 is left, 1 is right.
     * @param y1 Y value of top left corner of rectangle to track, normalized from 0..1. 0 is left, 1 is right.
     * @param x2 X value of bottom right corner of rectangle to track, normalized from 0..1. 0 is left, 1 is right.
     * @param y2 Y value of bottom right corner of rectangle to track, normalized from 0..1. 0 is left, 1 is right.
     * @param id Id of gimbal device to control. Set to one of MAV_COMPONENT_ID_GIMBAL[1-6]
     * for a MAVLink enabled gimbal device, [1-6] for a non-MAVLink gimbal device,
     * or 0 to command all connected gimbal devices.
     * @return Result of request.
     */
    // TODO consider adding this back later
    //Result track_rectangle(float x1, float y1, float x2, float y2, uint8_t id);

    /**
     * @brief Track camera rectangle.
     *
     * This commands the specified gimbal to track a rectangle in a camera view.
     * The function will return when the command is accepted, however, it might
     * take the gimbal longer to actually be set to the new angles.
     *
     * @param x1 X value of top left corner of rectangle to track, normalized from 0..1. 0 is left, 1 is right.
     * @param y1 Y value of top left corner of rectangle to track, normalized from 0..1. 0 is left, 1 is right.
     * @param x2 X value of bottom right corner of rectangle to track, normalized from 0..1. 0 is left, 1 is right.
     * @param y2 Y value of bottom right corner of rectangle to track, normalized from 0..1. 0 is left, 1 is right.
     * @param id Id of gimbal device to control. Set to one of MAV_COMPONENT_ID_GIMBAL[1-6]
     * for a MAVLink enabled gimbal device, [1-6] for a non-MAVLink gimbal device,
     * or 0 to command all connected gimbal devices.
     * @param callback Function to call with result of request.
     */
    // TODO consider adding this back later
    //void track_rectangle_async(float x1, float y1, float x2, float y2, uint8_t id, result_callback_t callback);

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
     * @return Result of request.
     */
    Result set_roi_location(
        double latitude_deg, double longitude_deg, float altitude_m);

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
    GimbalManager(const GimbalManager&) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const GimbalManager& operator=(const GimbalManager&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<GimbalManagerImpl> _impl;

    /** @private Information about the gimbal manager this class is connected to. */
    Information _information;

    /** @private Whether or not this class is currently bound to a gimbal manager. */
    bool _bound = false;
};

} // namespace mavsdk
