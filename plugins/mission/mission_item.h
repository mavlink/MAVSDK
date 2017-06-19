#pragma once

namespace dronelink {

class MissionItemImpl;
class MissionImpl;

class MissionItem
{
public:
    MissionItem();
    ~MissionItem();

    void set_position(double latitude_deg, double longitude_deg);
    void set_relative_altitude(float altitude_m);
    void set_fly_through(bool fly_through);
    void set_speed(float speed_m_s);
    void set_gimbal_pitch_and_yaw(float pitch_deg, float yaw_deg);

    enum class CameraAction {
        TAKE_PHOTO,
        START_PHOTO_INTERVAL,
        STOP_PHOTO_INTERVAL,
        START_VIDEO,
        STOP_VIDEO,
        NONE
    };

    void set_camera_action(CameraAction action);
    void set_camera_photo_interval(double interval_s);

    double get_latitude_deg() const;
    double get_longitude_deg() const;
    float get_relative_altitude_m() const;
    bool get_fly_through() const;
    float get_speed_m_s() const;

    CameraAction get_camera_action() const;
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

} // namespace dronelink
