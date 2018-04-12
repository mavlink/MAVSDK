#include "mission_item.h"

#include <iomanip>
#include <iostream>
#include <vector>

#include "mission_item_impl.h"

namespace dronecore {

MissionItem::MissionItem() :
    _impl { new MissionItemImpl() }
{
}

MissionItem::~MissionItem()
{
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

void MissionItem::set_loiter_time(float loiter_time_s)
{
    _impl->set_loiter_time(loiter_time_s);
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

bool MissionItem::has_position_set() const
{
    return _impl->is_position_finite();
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

float MissionItem::get_gimbal_pitch_deg() const
{
    return _impl->get_gimbal_pitch_deg();
}

float MissionItem::get_gimbal_yaw_deg() const
{
    return _impl->get_gimbal_yaw_deg();
}

float MissionItem::get_loiter_time_s() const
{
    return _impl->get_loiter_time_s();
}

MissionItem::CameraAction MissionItem::get_camera_action() const
{
    return _impl->get_camera_action();
}

double MissionItem::get_camera_photo_interval_s() const
{
    return _impl->get_camera_photo_interval_s();
}

std::string MissionItem::to_str(MissionItem::CameraAction camera_action)
{
    switch (camera_action) {
        case MissionItem::CameraAction::TAKE_PHOTO:
            return "Take photo";
        case MissionItem::CameraAction::START_PHOTO_INTERVAL:
            return "Start photo interval";
        case MissionItem::CameraAction::STOP_PHOTO_INTERVAL:
            return "Stop photo interval";
        case MissionItem::CameraAction::START_VIDEO:
            return "Start video";
        case MissionItem::CameraAction::STOP_VIDEO:
            return "Stop video";
        default:
            return "Unknown";
    }
}

bool operator==(const MissionItem &lhs, const MissionItem &rhs)
{
    return (lhs.get_latitude_deg() == rhs.get_latitude_deg()
            || isnan(lhs.get_latitude_deg()) && isnan(rhs.get_latitude_deg()))
           && (lhs.get_longitude_deg() == rhs.get_longitude_deg()
               || isnan(lhs.get_longitude_deg()) && isnan(rhs.get_longitude_deg()))
           && (lhs.get_relative_altitude_m() == rhs.get_relative_altitude_m()
               || isnan(lhs.get_relative_altitude_m()) && isnan(rhs.get_relative_altitude_m()))
           && lhs.get_fly_through() == rhs.get_fly_through()
           && (lhs.get_speed_m_s() == rhs.get_speed_m_s()
               || isnan(lhs.get_speed_m_s()) && isnan(rhs.get_speed_m_s()))
           && (lhs.get_gimbal_pitch_deg() == rhs.get_gimbal_pitch_deg()
               || isnan(lhs.get_gimbal_pitch_deg()) && isnan(rhs.get_gimbal_pitch_deg()))
           && (lhs.get_gimbal_yaw_deg() == rhs.get_gimbal_yaw_deg()
               || isnan(lhs.get_gimbal_yaw_deg()) && isnan(rhs.get_gimbal_yaw_deg()))
           && (lhs.get_loiter_time_s() == rhs.get_loiter_time_s()
               || isnan(lhs.get_loiter_time_s()) && isnan(rhs.get_loiter_time_s()))
           && lhs.get_camera_action() == rhs.get_camera_action()
           && lhs.get_camera_photo_interval_s() == rhs.get_camera_photo_interval_s();
}

std::ostream &operator<<(std::ostream &str, MissionItem const &mission_item)
{
    return str << std::endl << "[" << std::endl << std::setprecision(10)
           << "\tlat: " << mission_item.get_latitude_deg() << std::endl
           << "\tlon: " << mission_item.get_longitude_deg() << std::endl
           << "\trel_alt: " << mission_item.get_relative_altitude_m() << std::endl
           << "\tis_fly_through: " << (mission_item.get_fly_through() ? "true" : "false") << std::endl
           << "\tspeed: " << mission_item.get_speed_m_s() << std::endl
           << "\tgimbal_pitch: " << mission_item.get_gimbal_pitch_deg() << std::endl
           << "\tgimbal_yaw: " << mission_item.get_gimbal_yaw_deg() << std::endl
           << "\tloiter_time: " << mission_item.get_loiter_time_s() << std::endl
           << "\tcamera_action: " << mission_item.get_camera_action() << std::endl
           << "\tcamera_photo_interval: " << mission_item.get_camera_photo_interval_s()
           << std::endl << "]" << std::endl;
}

std::ostream &operator<<(std::ostream &str, MissionItem::CameraAction const &camera_action)
{
    switch (camera_action) {
        case MissionItem::CameraAction::TAKE_PHOTO:
            return str << "CameraAction::TAKE_PHOTO";
        case MissionItem::CameraAction::START_PHOTO_INTERVAL:
            return str << "CameraAction::START_PHOTO_INTERVAL";
        case MissionItem::CameraAction::STOP_PHOTO_INTERVAL:
            return str << "CameraAction::STOP_PHOTO_INTERVAL";
        case MissionItem::CameraAction::START_VIDEO:
            return str << "CameraAction::START_VIDEO";
        case MissionItem::CameraAction::STOP_VIDEO:
            return str << "CameraAction::STOP_VIDEO";
        case MissionItem::CameraAction::NONE:
            return str << "CameraAction::NONE";
    }
}

} // namespace dronelin
