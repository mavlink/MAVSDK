#include "modes_fallback.hpp"
#include "ardupilot_custom_mode.hpp"
#include "px4_custom_mode.hpp"

namespace mavsdk {

std::string modes_standard_mode_name(Modes::StandardMode standard_mode)
{
    switch (standard_mode) {
        case Modes::StandardMode::PositionHold:
            return "Position";
        case Modes::StandardMode::Orbit:
            return "Orbit";
        case Modes::StandardMode::Cruise:
            return "Cruise";
        case Modes::StandardMode::AltitudeHold:
            return "Altitude";
        case Modes::StandardMode::SafeRecovery:
            return "Safe Recovery";
        case Modes::StandardMode::Mission:
            return "Mission";
        case Modes::StandardMode::Land:
            return "Land";
        case Modes::StandardMode::Takeoff:
            return "Takeoff";
        case Modes::StandardMode::NonStandard:
        default:
            return "";
    }
}

namespace {

bool is_fixed_wing(MAV_TYPE vehicle_type)
{
    return vehicle_type == MAV_TYPE_FIXED_WING;
}

bool is_vtol(MAV_TYPE vehicle_type)
{
    switch (vehicle_type) {
        case MAV_TYPE_VTOL_TAILSITTER_DUOROTOR:
        case MAV_TYPE_VTOL_TAILSITTER_QUADROTOR:
        case MAV_TYPE_VTOL_TILTROTOR:
        case MAV_TYPE_VTOL_FIXEDROTOR:
        case MAV_TYPE_VTOL_TAILSITTER:
        case MAV_TYPE_VTOL_TILTWING:
            return true;
        default:
            return false;
    }
}

Modes::Mode make_mode(
    std::string name, Modes::StandardMode standard_mode, uint32_t custom_mode, bool advanced)
{
    Modes::Mode mode{};
    mode.name = standard_mode == Modes::StandardMode::NonStandard ?
                    std::move(name) :
                    modes_standard_mode_name(standard_mode);
    mode.standard_mode = standard_mode;
    mode.custom_mode = custom_mode;
    mode.is_user_selectable = true;
    mode.is_advanced = advanced;
    mode.is_auto_mode = false;
    return mode;
}

uint32_t px4_mode(uint8_t main_mode, uint8_t sub_mode = 0)
{
    px4::px4_custom_mode custom_mode{};
    custom_mode.main_mode = main_mode;
    custom_mode.sub_mode = sub_mode;
    return custom_mode.data;
}

// Mirrors what PX4 v1.15 and later reports in AVAILABLE_MODES, so that the same names work
// against older and newer PX4 versions.
std::vector<Modes::Mode> px4_modes(MAV_TYPE vehicle_type)
{
    using px4::PX4_CUSTOM_MAIN_MODE_ACRO;
    using px4::PX4_CUSTOM_MAIN_MODE_ALTCTL;
    using px4::PX4_CUSTOM_MAIN_MODE_AUTO;
    using px4::PX4_CUSTOM_MAIN_MODE_MANUAL;
    using px4::PX4_CUSTOM_MAIN_MODE_OFFBOARD;
    using px4::PX4_CUSTOM_MAIN_MODE_POSCTL;
    using px4::PX4_CUSTOM_MAIN_MODE_STABILIZED;
    using px4::PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET;
    using px4::PX4_CUSTOM_SUB_MODE_AUTO_LAND;
    using px4::PX4_CUSTOM_SUB_MODE_AUTO_LOITER;
    using px4::PX4_CUSTOM_SUB_MODE_AUTO_MISSION;
    using px4::PX4_CUSTOM_SUB_MODE_AUTO_RTL;
    using px4::PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF;
    using SM = Modes::StandardMode;

    // PX4 maps position control to "Position" for multicopters and "Cruise" for fixed-wing,
    // and to neither for VTOLs.
    SM position_standard_mode = SM::PositionHold;
    if (is_vtol(vehicle_type)) {
        position_standard_mode = SM::NonStandard;
    } else if (is_fixed_wing(vehicle_type)) {
        position_standard_mode = SM::Cruise;
    }

    return {
        make_mode("Manual", SM::NonStandard, px4_mode(PX4_CUSTOM_MAIN_MODE_MANUAL), true),
        make_mode("Altitude", SM::AltitudeHold, px4_mode(PX4_CUSTOM_MAIN_MODE_ALTCTL), false),
        make_mode("Position", position_standard_mode, px4_mode(PX4_CUSTOM_MAIN_MODE_POSCTL), false),
        make_mode(
            "Mission",
            SM::Mission,
            px4_mode(PX4_CUSTOM_MAIN_MODE_AUTO, PX4_CUSTOM_SUB_MODE_AUTO_MISSION),
            true),
        make_mode(
            "Hold",
            SM::NonStandard,
            px4_mode(PX4_CUSTOM_MAIN_MODE_AUTO, PX4_CUSTOM_SUB_MODE_AUTO_LOITER),
            true),
        make_mode(
            "Return",
            SM::SafeRecovery,
            px4_mode(PX4_CUSTOM_MAIN_MODE_AUTO, PX4_CUSTOM_SUB_MODE_AUTO_RTL),
            true),
        make_mode("Acro", SM::NonStandard, px4_mode(PX4_CUSTOM_MAIN_MODE_ACRO), true),
        make_mode("Offboard", SM::NonStandard, px4_mode(PX4_CUSTOM_MAIN_MODE_OFFBOARD), true),
        make_mode("Stabilized", SM::NonStandard, px4_mode(PX4_CUSTOM_MAIN_MODE_STABILIZED), true),
        make_mode(
            "Takeoff",
            SM::Takeoff,
            px4_mode(PX4_CUSTOM_MAIN_MODE_AUTO, PX4_CUSTOM_SUB_MODE_AUTO_TAKEOFF),
            true),
        make_mode(
            "Land",
            SM::Land,
            px4_mode(PX4_CUSTOM_MAIN_MODE_AUTO, PX4_CUSTOM_SUB_MODE_AUTO_LAND),
            true),
        make_mode(
            "Follow Target",
            SM::NonStandard,
            px4_mode(PX4_CUSTOM_MAIN_MODE_AUTO, PX4_CUSTOM_SUB_MODE_AUTO_FOLLOW_TARGET),
            true),
    };
}

template<typename T> Modes::Mode ardupilot_mode(const char* name, T mode_number)
{
    return make_mode(
        name, Modes::StandardMode::NonStandard, static_cast<uint32_t>(mode_number), false);
}

// Names as ArduPilot 4.7 and later reports them in AVAILABLE_MODES (Mode::name()).
std::vector<Modes::Mode> ardupilot_copter_modes()
{
    using M = ardupilot::CopterMode;
    return {
        ardupilot_mode("Stabilize", M::Stabilize),
        ardupilot_mode("Acro", M::Acro),
        ardupilot_mode("Altitude Hold", M::AltHold),
        ardupilot_mode("Auto", M::Auto),
        ardupilot_mode("Guided", M::Guided),
        ardupilot_mode("Loiter", M::Loiter),
        ardupilot_mode("RTL", M::Rtl),
        ardupilot_mode("Circle", M::Circle),
        ardupilot_mode("Land", M::Land),
        ardupilot_mode("Drift", M::Drift),
        ardupilot_mode("Sport", M::Sport),
        ardupilot_mode("Flip", M::Flip),
        ardupilot_mode("Autotune", M::AutoTune),
        ardupilot_mode("Position Hold", M::PosHold),
        ardupilot_mode("Brake", M::Brake),
        ardupilot_mode("Throw", M::Throw),
        ardupilot_mode("Avoid ADSB", M::AvoidAdsb),
        ardupilot_mode("Guided No GPS", M::GuidedNoGps),
        ardupilot_mode("Smart RTL", M::SmartRtl),
        ardupilot_mode("Flow Hold", M::FlowHold),
        ardupilot_mode("Follow", M::Follow),
        ardupilot_mode("ZigZag", M::Zigzag),
        ardupilot_mode("SystemID", M::SystemId),
        ardupilot_mode("Autorotate", M::AutoRotate),
        ardupilot_mode("Auto RTL", M::AutoRtl),
        ardupilot_mode("Turtle", M::Turtle),
    };
}

std::vector<Modes::Mode> ardupilot_plane_modes()
{
    using M = ardupilot::PlaneMode;
    return {
        ardupilot_mode("Manual", M::Manual),
        ardupilot_mode("Circle", M::Circle),
        ardupilot_mode("Stabilize", M::Stabilize),
        ardupilot_mode("Training", M::Training),
        ardupilot_mode("Acro", M::Acro),
        ardupilot_mode("FBWA", M::Fbwa),
        ardupilot_mode("FBWB", M::Fbwb),
        ardupilot_mode("Cruise", M::Cruise),
        ardupilot_mode("Autotune", M::Autotune),
        ardupilot_mode("Auto", M::Auto),
        ardupilot_mode("RTL", M::Rtl),
        ardupilot_mode("Loiter", M::Loiter),
        ardupilot_mode("Takeoff", M::Takeoff),
        ardupilot_mode("Avoid ADSB", M::AvoidAdsb),
        ardupilot_mode("Guided", M::Guided),
        ardupilot_mode("QStabilize", M::QStabilize),
        ardupilot_mode("QHover", M::QHover),
        ardupilot_mode("QLoiter", M::QLoiter),
        ardupilot_mode("QLand", M::QLand),
        ardupilot_mode("QRTL", M::QRtl),
        ardupilot_mode("QAutotune", M::QAutotune),
        ardupilot_mode("QAcro", M::QAcro),
        ardupilot_mode("Thermal", M::Thermal),
    };
}

std::vector<Modes::Mode> ardupilot_rover_modes()
{
    using M = ardupilot::RoverMode;
    return {
        ardupilot_mode("Manual", M::Manual),
        ardupilot_mode("Acro", M::Acro),
        ardupilot_mode("Steering", M::Steering),
        ardupilot_mode("Hold", M::Hold),
        ardupilot_mode("Loiter", M::Loiter),
        ardupilot_mode("Follow", M::Follow),
        ardupilot_mode("Simple", M::Simple),
        ardupilot_mode("Auto", M::Auto),
        ardupilot_mode("RTL", M::RTL),
        ardupilot_mode("Smart RTL", M::Smart_RTL),
        ardupilot_mode("Guided", M::Guided),
    };
}

} // namespace

std::vector<Modes::Mode> modes_fallback(Autopilot autopilot, MAV_TYPE vehicle_type)
{
    switch (autopilot) {
        case Autopilot::Px4:
            return px4_modes(vehicle_type);
        case Autopilot::ArduPilot:
            // Same vehicle type split as to_flight_mode_from_custom_mode().
            switch (vehicle_type) {
                case MAV_TYPE_SURFACE_BOAT:
                case MAV_TYPE_GROUND_ROVER:
                    return ardupilot_rover_modes();
                case MAV_TYPE_FIXED_WING:
                    return ardupilot_plane_modes();
                default:
                    if (is_vtol(vehicle_type)) {
                        return ardupilot_plane_modes();
                    }
                    return ardupilot_copter_modes();
            }
        default:
            return {};
    }
}

} // namespace mavsdk
