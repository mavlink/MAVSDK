#include "mission_item.h"
#include "mission_item_impl.h"
#include <vector>

namespace dronecore {

MissionItem::MissionItem() :
    _impl(new MissionItemImpl())
{
}

MissionItem::~MissionItem()
{
    delete _impl;
}


void MissionItem::set_position(double latitude_deg, double longitude_deg)
{
    _impl->set_position(latitude_deg, longitude_deg);
}

void MissionItem::set_relative_altitude(float relative_altitude_m)
{
    _impl->set_relative_altitude(relative_altitude_m);
}

void MissionItem::set_speed(float speed_m_s)
{
    _impl->set_speed(speed_m_s);
}

void MissionItem::set_fly_through(bool fly_through)
{
    _impl->set_fly_through(fly_through);
}

void MissionItem::set_gimbal_pitch_and_yaw(float pitch_deg, float yaw_deg)
{
    _impl->set_gimbal_pitch_and_yaw(pitch_deg, yaw_deg);
}

void MissionItem::set_camera_action_delay(float delay_s)
{
    _impl->set_camera_action_delay(delay_s);
}

void MissionItem::set_camera_action(CameraAction action)
{
    _impl->set_camera_action(action);
}

void MissionItem::set_camera_photo_interval(double interval_s)
{
    _impl->set_camera_photo_interval(interval_s);
}

double MissionItem::get_latitude_deg() const
{
    return _impl->get_latitude_deg();
}

double MissionItem::get_longitude_deg() const
{
    return _impl->get_longitude_deg();
}

float MissionItem::get_relative_altitude_m() const
{
    return _impl->get_relative_altitude_m();
}

bool MissionItem::get_fly_through() const
{
    return _impl->get_fly_through();
}

float MissionItem::get_speed_m_s() const
{
    return _impl->get_speed_m_s();
}

float MissionItem::get_camera_action_delay_s() const
{
    return _impl->get_camera_action_delay_s();
}

MissionItem::CameraAction MissionItem::get_camera_action() const
{
    return _impl->get_camera_action();
}

double MissionItem::get_camera_photo_interval_s() const
{
    return _impl->get_camera_photo_interval_s();
}

} // namespace dronelin
