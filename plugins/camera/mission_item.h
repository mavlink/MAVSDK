#pragma once
#include <string>
#include <memory>

namespace dronecore {

class MissionItemImpl;
class MissionImpl;

/**
 * @brief A mission is a vector of `MissionItem`s.
 *
 * Each MissionItem can contain a position and/or actions.
 * Mission items are just building blocks to assemble a mission,
 * which can be sent to (or received from) a system. They cannot be used independently.
 */
class MissionItem
{
public:
    /**
     * @brief Constructor (internal use only).
     */
    MissionItem();

    /**
     * @brief Destructor (internal use only).
     */
    ~MissionItem();

    /**
     * @brief Set the position of a mission item.
     *
     * @param latitude_deg Latitude of the waypoint in degrees.
     * @param longitude_deg Longitude of the waypoint in degrees.
     */
    void set_position(double latitude_deg, double longitude_deg);

    /**
     * @brief Set the relative altitude of a mission item.
     *
     * @param altitude_m Altitude relative to takeoff position in metres.
     */
    void set_relative_altitude(float altitude_m);

    /**
     * @brief Set the fly-through property of a mission item.
     *
     * @param fly_through If `true` the drone will fly through the waypoint without stopping.
     *                    If `false` the drone will come to a stop at the waypoint before continuing.
     */
    void set_fly_through(bool fly_through);

    /**
     * @brief Set the speed to use after a mission item.
     *
     * @param speed_m_s Speed to use after this mission item in metres/second.
     */
    void set_speed(float speed_m_s);

    /**
     * @brief Set the pitch and yaw angle of a gimbal at that mission item.
     *
     * @param pitch_deg The new pitch angle of the gimbal in degrees (0: horizontal, positive up,
     *                  -90: vertical downward facing).
     * @param yaw_deg The new yaw angle of the gimbal in degrees (0: forward, positive clock-wise,
     *                90: to the right).
     */
    void set_gimbal_pitch_and_yaw(float pitch_deg, float yaw_deg);

    /**
     * @brief Set loiter time in seconds.
     *
     * This specifies the delay at a waypoint before executing next mission item.
     * It can be used to wait for vehicle to slow down or a gimbal to arrive at the set
     * orientation.
     *
     * @param loiter_time_s The time to wait (loiter), in seconds.
     */
    void set_loiter_time(float loiter_time_s);

    /**
     * @brief Possible camera actions at a mission item.
     * @sa to_str()
     */
    enum class CameraAction {
        TAKE_PHOTO, /**< @brief Take single photo. */
        START_PHOTO_INTERVAL, /**< @brief Start capturing photos at regular intervals - see set_camera_photo_interval(). */
        STOP_PHOTO_INTERVAL, /**< @brief Stop capturing photos at regular intervals. */
        START_VIDEO, /**< @brief Start capturing video. */
        STOP_VIDEO, /**< @brief Stop capturing video. */
        NONE /**< @brief No action. */
    };

    /**
     * @brief Converts #CameraAction to English strings.
     * @param camera_action Enum #CameraAction.
     * @return Human readable english string for #CameraAction.
     */
    static std::string to_str(CameraAction camera_action);

    /**
     * @brief Set the camera action for a mission item.
     *
     * @param action The camera action.
     */
    void set_camera_action(CameraAction action);

    /**
     * @brief Set the camera photo interval.
     *
     * This only has an effect if used together with CameraAction::START_PHOTO_INTERVAL.
     *
     * @param interval_s Interval between photo captures in seconds.
     */
    void set_camera_photo_interval(double interval_s);

    /**
     * @brief Get the latitude of a mission item.
     *
     * @return Latitude in degrees.
     */
    double get_latitude_deg() const;

    /**
     * @brief Get the longitude of a mission item.
     *
     * @return Longitude in degrees.
     */
    double get_longitude_deg() const;

    /**
     * @brief Reports whether position info (Lat, Lon)
     * was set for this mission item.
     * @return true if Lat, Lon is set for this mission item.
     */
    bool has_position_set() const;

    /**
     * @brief Get the relative altitude of a mission item.
     *
     * @return The altitude relative to the takeoff position in metres.
     */
    float get_relative_altitude_m() const;

    /**
     * @brief Get the fly-through property of a mission item.
     *
     * @return true if the drone will fly through the waypoint without stopping.
     *         false if the drone will come to a stop at the waypoint before continuing.
     */
    bool get_fly_through() const;

    /**
     * @brief Get the speed to be used after this mission item.
     *
     * @return Speed in metres/second.
     */
    float get_speed_m_s() const;

    /**
     * @brief Get loiter time in seconds.
     *
     * @return Loiter time in seconds.
     */
    float get_loiter_time_s() const;

    /**
     * @brief Get the camera action set for this mission item.
     *
     * @return Camera action enum value.
     */
    CameraAction get_camera_action() const;

    /**
     * @brief Get the camera photo interval that was set for this mission item.
     *
     * This only has an effect if used together with CameraAction::START_PHOTO_INTERVAL.
     *
     * @return Camera photo interval in seconds.
     */
    double get_camera_photo_interval_s() const;

    /**
     * @private
     * We need to make MissionImpl a friend so it can access _impl.
     */
    friend MissionImpl;

    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    MissionItem(const MissionItem &) = delete;
    /**
    * @brief Equality operator (object is not copyable).
    */
    const MissionItem &operator=(const MissionItem &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<MissionItemImpl> _impl;
};

} // namespace dronecore
