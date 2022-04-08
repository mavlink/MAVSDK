
#include "flight_mode.h"
#include "mavlink_include.h"
#include "ardupilot_custom_mode.h"
#include "px4_custom_mode.h"
#include "mavlink_command_sender.h"

namespace mavsdk {

FlightMode to_flight_mode_from_custom_mode(
    Sender::Autopilot autopilot, MAV_TYPE mav_type, uint32_t custom_mode)
{
    if (autopilot == Sender::Autopilot::ArduPilot) {
        switch (mav_type) {
            case MAV_TYPE::MAV_TYPE_SURFACE_BOAT:
            case MAV_TYPE::MAV_TYPE_GROUND_ROVER:
                return to_flight_mode_from_ardupilot_rover_mode(custom_mode);
            default:
                return to_flight_mode_from_ardupilot_copter_mode(custom_mode);
        }
    } else {
        return to_flight_mode_from_px4_mode(custom_mode);
    }
}

FlightMode to_flight_mode_from_ardupilot_rover_mode(uint32_t custom_mode)
{
    switch (static_cast<ardupilot::RoverMode>(custom_mode)) {
        case ardupilot::RoverMode::Auto:
            return FlightMode::Mission;
        case ardupilot::RoverMode::Acro:
            return FlightMode::Acro;
        case ardupilot::RoverMode::Hold:
            return FlightMode::Hold;
        case ardupilot::RoverMode::RTL:
            return FlightMode::ReturnToLaunch;
        case ardupilot::RoverMode::Manual:
            return FlightMode::Manual;
        case ardupilot::RoverMode::Follow:
            return FlightMode::FollowMe;
        default:
            return FlightMode::Unknown;
    }
}
FlightMode to_flight_mode_from_ardupilot_copter_mode(uint32_t custom_mode)
{
    switch (static_cast<ardupilot::CopterMode>(custom_mode)) {
        case ardupilot::CopterMode::Auto:
            return FlightMode::Mission;
        case ardupilot::CopterMode::Acro:
            return FlightMode::Acro;
        case ardupilot::CopterMode::Alt_Hold:
        case ardupilot::CopterMode::POS_HOLD:
        case ardupilot::CopterMode::Flow_Hold:
            return FlightMode::Hold;
        case ardupilot::CopterMode::RTL:
        case ardupilot::CopterMode::Auto_RTL:
            return FlightMode::ReturnToLaunch;
        case ardupilot::CopterMode::Land:
            return FlightMode::Land;
        default:
            return FlightMode::Unknown;
    }
}

FlightMode to_flight_mode_from_px4_mode(uint32_t custom_mode)
{
    px4::px4_custom_mode px4_custom_mode;
    px4_custom_mode.data = custom_mode;

    switch (px4_custom_mode.main_mode) {
        case px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD:
            return FlightMode::Offboard;
        case px4::PX4_CUSTOM_MAIN_MODE_MANUAL:
            return FlightMode::Manual;
        case px4::PX4_CUSTOM_MAIN_MODE_POSCTL:
            return FlightMode::Posctl;
        case px4::PX4_CUSTOM_MAIN_MODE_ALTCTL:
            return FlightMode::Altctl;
        case px4::PX4_CUSTOM_MAIN_MODE_RATTITUDE:
            return FlightMode::Rattitude;
        case px4::PX4_CUSTOM_MAIN_MODE_ACRO:
            return FlightMode::Acro;
        case px4::PX4_CUSTOM_MAIN_MODE_STABILIZED:
            return FlightMode::Stabilized;
        case px4::PX4_CUSTOM_MAIN_MODE_AUTO:
            switch (px4_custom_mode.sub_mode) {
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_READY:
                    return FlightMode::Ready;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF:
                    return FlightMode::Takeoff;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER:
                    return FlightMode::Hold;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION:
                    return FlightMode::Mission;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL:
                    return FlightMode::ReturnToLaunch;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND:
                    return FlightMode::Land;
                case px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET:
                    return FlightMode::FollowMe;
                default:
                    return FlightMode::Unknown;
            }
        default:
            return FlightMode::Unknown;
    }
}

} // namespace mavsdk