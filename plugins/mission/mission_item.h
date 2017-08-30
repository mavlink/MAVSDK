#pragma once

namespace dronecore {

class MissionItemImpl;
class MissionImpl;

/**
 * A mission consists of a vector of `MissionItem`s.
 *
 * Each MissionItem can contain a position and/or actions.
 * Mission items are not connected to a device and don't communicate to a device, they are just
 * building blocks to assemble a mission which can be sent to (or received from) a device.
 */
class MissionItem
{
public:
    /**
     * Constructor to make a new `MissionItem`.
     */
    MissionItem();

    /**
     * Destructor for a MissionItem.
     */
    ~MissionItem();

    /**
     * Sets the position of a mission item.
     *
     * @param latitude_deg latitude of the waypoint in degrees
     * @param longitude_deg longitude of the waypoint in degrees
     */
    void set_position(double latitude_deg, double longitude_deg);

    /**
     * Sets the relative altitude of a mission item.
     *
     * @param relative_altitude_m altitude relative to takeoff position in meters
     */
    void set_relative_altitude(float altitude_m);

    /**
     * Sets the fly-through property of a mission item.
     *
     * @param fly_through if true the drone will fly through the waypoint without stopping
     *                    if false the drone will come to a stop at the waypoint before continuing
     */
    void set_fly_through(bool fly_through);

    /**
     * Sets the speed to use after a mission item.
     *
     * @speed_m_s speed to use after this mission item in meters/second
     */
    void set_speed(float speed_m_s);

    /**
     * Sets the pitch and yaw angle of a gimbal at that mission item.
     *
     * @param pitch_deg the new pitch angle of the gimbal in degrees (0: horizontal, positive up,
     *                  -90: vertical downward facing)
     * @param yaw_deg the new yaw angle of the gimbal in degrees (0: forward, positive clock-wise,
     *                90: to the right
     */
    void set_gimbal_pitch_and_yaw(float pitch_deg, float yaw_deg);

    /**
     * Possible camera actions at a mission item.
     */
    enum class CameraAction {
        TAKE_PHOTO,
        START_PHOTO_INTERVAL,
        STOP_PHOTO_INTERVAL,
        START_VIDEO,
        STOP_VIDEO,
        NONE
    };

    /**
     * Sets the camera action for a mission item.
     *
     * @param action the camera action
     */
    void set_camera_action(CameraAction action);

    /**
     * Set the camera photo interval.
     *
     * This only has an effect if used together with the camera action `START_PHOTO_INTERVAL`.
     *
     * @param interval_s interval between photo captures in seconds
     */
    void set_camera_photo_interval(double interval_s);

    /**
     * Gets the latitude of a mission item.
     *
     * @return latitude in degrees
     */
    double get_latitude_deg() const;

    /**
     * Gets the longitude of a mission item.
     *
     * @return longitude in degrees
     */
    double get_longitude_deg() const;

    /**
     * Gets the relative altitude of a mission item.
     *
     * @return the altitude relative to the takeoff position in meters
     */
    float get_relative_altitude_m() const;

    /**
     * Gets the fly-through property of a mission item.
     *
     * @return true if the drone will fly through the waypoint without stopping
     *         false if the drone will come to a stop at the waypoint before continuing
     */
    bool get_fly_through() const;

    /**
     * Gets the speed to be used after this mission items.
     *
     * @return speed in meters/second
     */
    float get_speed_m_s() const;

    /**
     * Gets the camera action set for this mission item.
     *
     * @return camera action enum
     */
    CameraAction get_camera_action() const;

    /**
     * Gets the camera photo interval that was set for this mission item.
     *
     * This only has an effect if used together with the camera action `START_PHOTO_INTERVAL`.
     *
     * @return camera photo interval in seconds
     */
    double get_camera_photo_interval_s() const;

    // We need to make MissionImpl a friend so it can access _impl.
    friend MissionImpl;

    // Non-copyable
    MissionItem(const MissionItem &) = delete;
    const MissionItem &operator=(const MissionItem &) = delete;

private:
    // Underlying implementation, set at instantiation
    MissionItemImpl *_impl;
};

} // namespace dronecore
