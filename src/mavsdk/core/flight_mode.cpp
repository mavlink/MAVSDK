
#include "autopilot.h"
#include "flight_mode.h"
#include "mavlink_include.h"
#include "ardupilot_custom_mode.h"
#include "px4_custom_mode.h"
#include "mavlink_command_sender.h"

namespace mavsdk {

FlightMode
to_flight_mode_from_custom_mode(Autopilot autopilot, MAV_TYPE mav_type, uint32_t custom_mode)
{
    if (autopilot == Autopilot::ArduPilot) {
        switch (mav_type) {
            case MAV_TYPE::MAV_TYPE_SURFACE_BOAT:
            case MAV_TYPE::MAV_TYPE_GROUND_ROVER:
                return to_flight_mode_from_ardupilot_rover_mode(custom_mode);
            case MAV_TYPE::MAV_TYPE_FIXED_WING:
                return to_flight_mode_from_ardupilot_plane_mode(custom_mode);
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
        case ardupilot::RoverMode::Guided:
            return FlightMode::Offboard;
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
        case ardupilot::CopterMode::AltHold:
            return FlightMode::Altctl;
        case ardupilot::CopterMode::PosHold:
            return FlightMode::Posctl;
        case ardupilot::CopterMode::FlowHold:
        case ardupilot::CopterMode::Loiter:
            return FlightMode::Hold;
        case ardupilot::CopterMode::Rtl:
        case ardupilot::CopterMode::AutoRtl:
            return FlightMode::ReturnToLaunch;
        case ardupilot::CopterMode::Land:
            return FlightMode::Land;
        case ardupilot::CopterMode::Follow:
            return FlightMode::FollowMe;
        case ardupilot::CopterMode::Guided:
            return FlightMode::Offboard;
        case ardupilot::CopterMode::Stabilize:
            return FlightMode::Stabilized;

        case ardupilot::CopterMode::Circle:
            return FlightMode::Circle;
        case ardupilot::CopterMode::Drift:
            return FlightMode::Drift;
        case ardupilot::CopterMode::Sport:
            return FlightMode::Sport;
        case ardupilot::CopterMode::Flip:
            return FlightMode::Flip;
        case ardupilot::CopterMode::AutoTune:
            return FlightMode::Autotune;
        case ardupilot::CopterMode::Break:
            return FlightMode::Brake;
        case ardupilot::CopterMode::Throw:
            return FlightMode::Throw;
        case ardupilot::CopterMode::AvoidAdbs:
            return FlightMode::AvoidADSB;
        case ardupilot::CopterMode::GuidedNoGps:
            return FlightMode::GuidedNoGPS;
        case ardupilot::CopterMode::SmartRtl:
            return FlightMode::SmartRTL;
        case ardupilot::CopterMode::Zigzag:
            return FlightMode::ZigZag;
        case ardupilot::CopterMode::SystemId:
            return FlightMode::SystemId;
        case ardupilot::CopterMode::AutoRotate:
            return FlightMode::AutoRotate;
        case ardupilot::CopterMode::Turtle:
            return FlightMode::Turtle;

        case ardupilot::CopterMode::Unknown:
        default:
            return FlightMode::Unknown;
    }
}
FlightMode to_flight_mode_from_ardupilot_plane_mode(uint32_t custom_mode)
{
    switch (static_cast<ardupilot::PlaneMode>(custom_mode)) {
        case ardupilot::PlaneMode::Manual:
            return FlightMode::Manual;
        case ardupilot::PlaneMode::Auto:
            return FlightMode::Mission;
        case ardupilot::PlaneMode::Acro:
            return FlightMode::Acro;
        case ardupilot::PlaneMode::Autotune:
            return FlightMode::Altctl;
        case ardupilot::PlaneMode::Fbwa:
            return FlightMode::FBWA;
        case ardupilot::PlaneMode::Fbwb:
            return FlightMode::FBWB;
        case ardupilot::PlaneMode::Guided:
            return FlightMode::Offboard;
        case ardupilot::PlaneMode::Loiter:
            return FlightMode::Hold;
        case ardupilot::PlaneMode::Rtl:
            return FlightMode::ReturnToLaunch;
        case ardupilot::PlaneMode::Stabilize:
            return FlightMode::Stabilized;
        case ardupilot::PlaneMode::QStabilize:
            return FlightMode::QStabilize;
        case ardupilot::PlaneMode::QLoiter:
            return FlightMode::QLoiter;
        case ardupilot::PlaneMode::QLand:
            return FlightMode::QLand;
        case ardupilot::PlaneMode::QRtl:
            return FlightMode::QRtl;

        case ardupilot::PlaneMode::Circle:
            return FlightMode::Circle;
        case ardupilot::PlaneMode::Training:
            return FlightMode::Training;
        case ardupilot::PlaneMode::Cruise:
            return FlightMode::Cruise;
        case ardupilot::PlaneMode::Takeoff:
            return FlightMode::Takeoff;
        case ardupilot::PlaneMode::AvoidAdsb:
            return FlightMode::AvoidADSB;
        case ardupilot::PlaneMode::Initializing:
            return FlightMode::Initializing;
        case ardupilot::PlaneMode::QHover:
            return FlightMode::QHover;
        case ardupilot::PlaneMode::QAutotune:
            return FlightMode::QAutotune;
        case ardupilot::PlaneMode::QAcro:
            return FlightMode::QAcro;
        case ardupilot::PlaneMode::Thermal:
            return FlightMode::Thermal;

        case ardupilot::PlaneMode::Unknown:
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
