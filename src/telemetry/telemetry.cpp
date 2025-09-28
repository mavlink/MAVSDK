#include "telemetry/telemetry.h"

#include <mavsdk/plugins/telemetry/telemetry.h>
#include <functional>
#include <memory>
#include <string>

using namespace mavsdk;

// Helper to convert C++ Telemetry::Result to C mavsdk_telemetry_result_t
static mavsdk_telemetry_result_t telemetry_result_from_cpp(Telemetry::Result result)
{
    switch (result) {
        case Telemetry::Result::Success:
            return MAVSDK_TELEMETRY_RESULT_SUCCESS;
        case Telemetry::Result::NoSystem:
            return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
        case Telemetry::Result::ConnectionError:
            return MAVSDK_TELEMETRY_RESULT_CONNECTION_ERROR;
        case Telemetry::Result::Busy:
            return MAVSDK_TELEMETRY_RESULT_BUSY;
        case Telemetry::Result::CommandDenied:
            return MAVSDK_TELEMETRY_RESULT_COMMAND_DENIED;
        case Telemetry::Result::Timeout:
            return MAVSDK_TELEMETRY_RESULT_TIMEOUT;
        case Telemetry::Result::Unsupported:
            return MAVSDK_TELEMETRY_RESULT_UNSUPPORTED;
        case Telemetry::Result::Unknown:
        default:
            return MAVSDK_TELEMETRY_RESULT_UNKNOWN;
    }
}

// Helper to convert C++ FixType to C
static mavsdk_telemetry_fix_type_t fix_type_from_cpp(Telemetry::FixType fix_type)
{
    switch (fix_type) {
        case Telemetry::FixType::NoGps:
            return MAVSDK_TELEMETRY_FIX_TYPE_NO_GPS;
        case Telemetry::FixType::NoFix:
            return MAVSDK_TELEMETRY_FIX_TYPE_NO_FIX;
        case Telemetry::FixType::Fix2D:
            return MAVSDK_TELEMETRY_FIX_TYPE_FIX_2D;
        case Telemetry::FixType::Fix3D:
            return MAVSDK_TELEMETRY_FIX_TYPE_FIX_3D;
        case Telemetry::FixType::FixDgps:
            return MAVSDK_TELEMETRY_FIX_TYPE_FIX_DGPS;
        case Telemetry::FixType::RtkFloat:
            return MAVSDK_TELEMETRY_FIX_TYPE_RTK_FLOAT;
        case Telemetry::FixType::RtkFixed:
            return MAVSDK_TELEMETRY_FIX_TYPE_RTK_FIXED;
        default:
            return MAVSDK_TELEMETRY_FIX_TYPE_NO_FIX;
    }
}

// Helper to convert C++ FlightMode to C
static mavsdk_telemetry_flight_mode_t flight_mode_from_cpp(Telemetry::FlightMode flight_mode)
{
    switch (flight_mode) {
        case Telemetry::FlightMode::Ready:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_READY;
        case Telemetry::FlightMode::Takeoff:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_TAKEOFF;
        case Telemetry::FlightMode::Hold:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_HOLD;
        case Telemetry::FlightMode::Mission:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_MISSION;
        case Telemetry::FlightMode::ReturnToLaunch:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_RETURN_TO_LAUNCH;
        case Telemetry::FlightMode::Land:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_LAND;
        case Telemetry::FlightMode::Offboard:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_OFFBOARD;
        case Telemetry::FlightMode::FollowMe:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_FOLLOW_ME;
        case Telemetry::FlightMode::Manual:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_MANUAL;
        case Telemetry::FlightMode::Altctl:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_ALTCTL;
        case Telemetry::FlightMode::Posctl:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_POSCTL;
        case Telemetry::FlightMode::Acro:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_ACRO;
        case Telemetry::FlightMode::Stabilized:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_STABILIZED;
        case Telemetry::FlightMode::Rattitude:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_RATTITUDE;
        case Telemetry::FlightMode::Unknown:
        default:
            return MAVSDK_TELEMETRY_FLIGHT_MODE_UNKNOWN;
    }
}

// Helper to convert C++ LandedState to C
static mavsdk_telemetry_landed_state_t landed_state_from_cpp(Telemetry::LandedState landed_state)
{
    switch (landed_state) {
        case Telemetry::LandedState::OnGround:
            return MAVSDK_TELEMETRY_LANDED_STATE_ON_GROUND;
        case Telemetry::LandedState::InAir:
            return MAVSDK_TELEMETRY_LANDED_STATE_IN_AIR;
        case Telemetry::LandedState::TakingOff:
            return MAVSDK_TELEMETRY_LANDED_STATE_TAKING_OFF;
        case Telemetry::LandedState::Landing:
            return MAVSDK_TELEMETRY_LANDED_STATE_LANDING;
        case Telemetry::LandedState::Unknown:
        default:
            return MAVSDK_TELEMETRY_LANDED_STATE_UNKNOWN;
    }
}

// Helper to convert C++ VtolState to C
static mavsdk_telemetry_vtol_state_t vtol_state_from_cpp(Telemetry::VtolState vtol_state)
{
    switch (vtol_state) {
        case Telemetry::VtolState::TransitionToFw:
            return MAVSDK_TELEMETRY_VTOL_STATE_TRANSITION_TO_FW;
        case Telemetry::VtolState::TransitionToMc:
            return MAVSDK_TELEMETRY_VTOL_STATE_TRANSITION_TO_MC;
        case Telemetry::VtolState::Mc:
            return MAVSDK_TELEMETRY_VTOL_STATE_MC;
        case Telemetry::VtolState::Fw:
            return MAVSDK_TELEMETRY_VTOL_STATE_FW;
        case Telemetry::VtolState::Undefined:
        default:
            return MAVSDK_TELEMETRY_VTOL_STATE_UNDEFINED;
    }
}

// Helper to copy Position from C++ to C
static void copy_position_to_c(const Telemetry::Position& cpp_pos, mavsdk_telemetry_position_t* c_pos)
{
    if (!c_pos) return;
    c_pos->latitude_deg = cpp_pos.latitude_deg;
    c_pos->longitude_deg = cpp_pos.longitude_deg;
    c_pos->absolute_altitude_m = cpp_pos.absolute_altitude_m;
    c_pos->relative_altitude_m = c_pos->relative_altitude_m;
}

// Helper to copy Quaternion from C++ to C
static void copy_quaternion_to_c(const Telemetry::Quaternion& cpp_quat, mavsdk_telemetry_quaternion_t* c_quat)
{
    if (!c_quat) return;
    c_quat->w = cpp_quat.w;
    c_quat->x = cpp_quat.x;
    c_quat->y = cpp_quat.y;
    c_quat->z = cpp_quat.z;
    c_quat->timestamp_us = cpp_quat.timestamp_us;
}

// Helper to copy EulerAngle from C++ to C
static void copy_euler_angle_to_c(const Telemetry::EulerAngle& cpp_euler, mavsdk_telemetry_euler_angle_t* c_euler)
{
    if (!c_euler) return;
    c_euler->roll_deg = cpp_euler.roll_deg;
    c_euler->pitch_deg = cpp_euler.pitch_deg;
    c_euler->yaw_deg = cpp_euler.yaw_deg;
    c_euler->timestamp_us = cpp_euler.timestamp_us;
}

// Helper to copy AngularVelocityBody from C++ to C
static void copy_angular_velocity_body_to_c(const Telemetry::AngularVelocityBody& cpp_ang_vel, mavsdk_telemetry_angular_velocity_body_t* c_ang_vel)
{
    if (!c_ang_vel) return;
    c_ang_vel->roll_rad_s = cpp_ang_vel.roll_rad_s;
    c_ang_vel->pitch_rad_s = cpp_ang_vel.pitch_rad_s;
    c_ang_vel->yaw_rad_s = cpp_ang_vel.yaw_rad_s;
}

// Helper to copy VelocityNed from C++ to C
static void copy_velocity_ned_to_c(const Telemetry::VelocityNed& cpp_vel, mavsdk_telemetry_velocity_ned_t* c_vel)
{
    if (!c_vel) return;
    c_vel->north_m_s = cpp_vel.north_m_s;
    c_vel->east_m_s = cpp_vel.east_m_s;
    c_vel->down_m_s = cpp_vel.down_m_s;
}

// Helper to copy GpsInfo from C++ to C
static void copy_gps_info_to_c(const Telemetry::GpsInfo& cpp_gps, mavsdk_telemetry_gps_info_t* c_gps)
{
    if (!c_gps) return;
    c_gps->num_satellites = cpp_gps.num_satellites;
    c_gps->fix_type = fix_type_from_cpp(cpp_gps.fix_type);
}

// Helper to copy Battery from C++ to C
static void copy_battery_to_c(const Telemetry::Battery& cpp_battery, mavsdk_telemetry_battery_t* c_battery)
{
    if (!c_battery) return;
    c_battery->id = cpp_battery.id;
    c_battery->temperature_degc = cpp_battery.temperature_degc;
    c_battery->voltage_v = cpp_battery.voltage_v;
    c_battery->current_battery_a = cpp_battery.current_battery_a;
    c_battery->capacity_consumed_ah = cpp_battery.capacity_consumed_ah;
    c_battery->remaining_percent = cpp_battery.remaining_percent;
    c_battery->time_remaining_s = cpp_battery.time_remaining_s;
}

// Helper to copy Health from C++ to C
static void copy_health_to_c(const Telemetry::Health& cpp_health, mavsdk_telemetry_health_t* c_health)
{
    if (!c_health) return;
    c_health->is_gyrometer_calibration_ok = cpp_health.is_gyrometer_calibration_ok;
    c_health->is_accelerometer_calibration_ok = cpp_health.is_accelerometer_calibration_ok;
    c_health->is_magnetometer_calibration_ok = cpp_health.is_magnetometer_calibration_ok;
    c_health->is_local_position_ok = cpp_health.is_local_position_ok;
    c_health->is_global_position_ok = cpp_health.is_global_position_ok;
    c_health->is_home_position_ok = cpp_health.is_home_position_ok;
    c_health->is_armable = cpp_health.is_armable;
}

// Helper to copy RcStatus from C++ to C
static void copy_rc_status_to_c(const Telemetry::RcStatus& cpp_rc, mavsdk_telemetry_rc_status_t* c_rc)
{
    if (!c_rc) return;
    c_rc->was_available_once = cpp_rc.was_available_once;
    c_rc->is_available = cpp_rc.is_available;
    c_rc->signal_strength_percent = cpp_rc.signal_strength_percent;
}

// Callback wrapper structures
struct TelemetryResultCallbackWrapper {
    mavsdk_telemetry_result_callback_t user_callback;
    void* user_data;
};

struct PositionCallbackWrapper {
    mavsdk_telemetry_position_callback_t user_callback;
    void* user_data;
};

struct HomeCallbackWrapper {
    mavsdk_telemetry_home_callback_t user_callback;
    void* user_data;
};

struct InAirCallbackWrapper {
    mavsdk_telemetry_in_air_callback_t user_callback;
    void* user_data;
};

struct LandedStateCallbackWrapper {
    mavsdk_telemetry_landed_state_callback_t user_callback;
    void* user_data;
};

struct ArmedCallbackWrapper {
    mavsdk_telemetry_armed_callback_t user_callback;
    void* user_data;
};

struct VtolStateCallbackWrapper {
    mavsdk_telemetry_vtol_state_callback_t user_callback;
    void* user_data;
};

struct AttitudeQuaternionCallbackWrapper {
    mavsdk_telemetry_attitude_quaternion_callback_t user_callback;
    void* user_data;
};

struct AttitudeEulerCallbackWrapper {
    mavsdk_telemetry_attitude_euler_callback_t user_callback;
    void* user_data;
};

struct AttitudeAngularVelocityBodyCallbackWrapper {
    mavsdk_telemetry_attitude_angular_velocity_body_callback_t user_callback;
    void* user_data;
};

struct VelocityNedCallbackWrapper {
    mavsdk_telemetry_velocity_ned_callback_t user_callback;
    void* user_data;
};

struct GpsInfoCallbackWrapper {
    mavsdk_telemetry_gps_info_callback_t user_callback;
    void* user_data;
};

struct BatteryCallbackWrapper {
    mavsdk_telemetry_battery_callback_t user_callback;
    void* user_data;
};

struct FlightModeCallbackWrapper {
    mavsdk_telemetry_flight_mode_callback_t user_callback;
    void* user_data;
};

struct HealthCallbackWrapper {
    mavsdk_telemetry_health_callback_t user_callback;
    void* user_data;
};

struct RcStatusCallbackWrapper {
    mavsdk_telemetry_rc_status_callback_t user_callback;
    void* user_data;
};

struct StatusTextCallbackWrapper {
    mavsdk_telemetry_status_text_callback_t user_callback;
    void* user_data;
};

struct HealthAllOkCallbackWrapper {
    mavsdk_telemetry_health_all_ok_callback_t user_callback;
    void* user_data;
};

extern "C" {

// ===== Telemetry Creation/Destruction =====
mavsdk_telemetry_t mavsdk_telemetry_create(mavsdk_system_t system)
{
    auto* cpp_system_ptr = static_cast<System**>(system);
    if (!cpp_system_ptr || !*cpp_system_ptr) {
        return nullptr;
    }
    
    auto* telemetry = new Telemetry(std::shared_ptr<System>(*cpp_system_ptr, [](System*) {
        // Empty deleter - we don't own the System, the mavsdk_t does
    }));
    return static_cast<mavsdk_telemetry_t>(telemetry);
}

void mavsdk_telemetry_destroy(mavsdk_telemetry_t telemetry)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    delete cpp_telemetry;
}

// ===== Subscription Methods =====
mavsdk_telemetry_position_handle_t mavsdk_telemetry_subscribe_position(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new PositionCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_position([wrapper](Telemetry::Position position) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_position_t c_position;
            copy_position_to_c(position, &c_position);
            wrapper->user_callback(&c_position, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::PositionHandle, PositionCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_position_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_position(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::PositionHandle, PositionCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_position(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_home_handle_t mavsdk_telemetry_subscribe_home(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_home_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new HomeCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_home([wrapper](Telemetry::Position home) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_position_t c_home;
            copy_position_to_c(home, &c_home);
            wrapper->user_callback(&c_home, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::HomeHandle, HomeCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_home_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_home(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_home_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::HomeHandle, HomeCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_home(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_in_air_handle_t mavsdk_telemetry_subscribe_in_air(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_in_air_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new InAirCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_in_air([wrapper](bool in_air) {
        if (wrapper->user_callback) {
            wrapper->user_callback(in_air, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::InAirHandle, InAirCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_in_air_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_in_air(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_in_air_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::InAirHandle, InAirCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_in_air(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_landed_state_handle_t mavsdk_telemetry_subscribe_landed_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_landed_state_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new LandedStateCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_landed_state([wrapper](Telemetry::LandedState landed_state) {
        if (wrapper->user_callback) {
            wrapper->user_callback(landed_state_from_cpp(landed_state), wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::LandedStateHandle, LandedStateCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_landed_state_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_landed_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_landed_state_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::LandedStateHandle, LandedStateCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_landed_state(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_armed_handle_t mavsdk_telemetry_subscribe_armed(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_armed_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new ArmedCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_armed([wrapper](bool armed) {
        if (wrapper->user_callback) {
            wrapper->user_callback(armed, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::ArmedHandle, ArmedCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_armed_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_armed(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_armed_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::ArmedHandle, ArmedCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_armed(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_vtol_state_handle_t mavsdk_telemetry_subscribe_vtol_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_vtol_state_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new VtolStateCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_vtol_state([wrapper](Telemetry::VtolState vtol_state) {
        if (wrapper->user_callback) {
            wrapper->user_callback(vtol_state_from_cpp(vtol_state), wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::VtolStateHandle, VtolStateCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_vtol_state_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_vtol_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_vtol_state_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::VtolStateHandle, VtolStateCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_vtol_state(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_attitude_quaternion_handle_t mavsdk_telemetry_subscribe_attitude_quaternion(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_quaternion_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new AttitudeQuaternionCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_attitude_quaternion([wrapper](Telemetry::Quaternion quaternion) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_quaternion_t c_quaternion;
            copy_quaternion_to_c(quaternion, &c_quaternion);
            wrapper->user_callback(&c_quaternion, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::AttitudeQuaternionHandle, AttitudeQuaternionCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_attitude_quaternion_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_attitude_quaternion(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_quaternion_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::AttitudeQuaternionHandle, AttitudeQuaternionCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_attitude_quaternion(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_attitude_euler_handle_t mavsdk_telemetry_subscribe_attitude_euler(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_euler_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new AttitudeEulerCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_attitude_euler([wrapper](Telemetry::EulerAngle euler_angle) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_euler_angle_t c_euler_angle;
            copy_euler_angle_to_c(euler_angle, &c_euler_angle);
            wrapper->user_callback(&c_euler_angle, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::AttitudeEulerHandle, AttitudeEulerCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_attitude_euler_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_attitude_euler(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_euler_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::AttitudeEulerHandle, AttitudeEulerCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_attitude_euler(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_attitude_angular_velocity_body_handle_t mavsdk_telemetry_subscribe_attitude_angular_velocity_body(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_angular_velocity_body_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new AttitudeAngularVelocityBodyCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_attitude_angular_velocity_body([wrapper](Telemetry::AngularVelocityBody angular_velocity) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_angular_velocity_body_t c_angular_velocity;
            copy_angular_velocity_body_to_c(angular_velocity, &c_angular_velocity);
            wrapper->user_callback(&c_angular_velocity, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::AttitudeAngularVelocityBodyHandle, AttitudeAngularVelocityBodyCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_attitude_angular_velocity_body_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_attitude_angular_velocity_body(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_attitude_angular_velocity_body_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::AttitudeAngularVelocityBodyHandle, AttitudeAngularVelocityBodyCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_attitude_angular_velocity_body(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_velocity_ned_handle_t mavsdk_telemetry_subscribe_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_velocity_ned_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new VelocityNedCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_velocity_ned([wrapper](Telemetry::VelocityNed velocity) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_velocity_ned_t c_velocity;
            copy_velocity_ned_to_c(velocity, &c_velocity);
            wrapper->user_callback(&c_velocity, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::VelocityNedHandle, VelocityNedCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_velocity_ned_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_velocity_ned(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_velocity_ned_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::VelocityNedHandle, VelocityNedCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_velocity_ned(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_gps_info_handle_t mavsdk_telemetry_subscribe_gps_info(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_gps_info_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new GpsInfoCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_gps_info([wrapper](Telemetry::GpsInfo gps_info) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_gps_info_t c_gps_info;
            copy_gps_info_to_c(gps_info, &c_gps_info);
            wrapper->user_callback(&c_gps_info, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::GpsInfoHandle, GpsInfoCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_gps_info_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_gps_info(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_gps_info_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::GpsInfoHandle, GpsInfoCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_gps_info(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_battery_handle_t mavsdk_telemetry_subscribe_battery(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_battery_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new BatteryCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_battery([wrapper](Telemetry::Battery battery) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_battery_t c_battery;
            copy_battery_to_c(battery, &c_battery);
            wrapper->user_callback(&c_battery, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::BatteryHandle, BatteryCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_battery_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_battery(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_battery_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::BatteryHandle, BatteryCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_battery(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_flight_mode_handle_t mavsdk_telemetry_subscribe_flight_mode(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_flight_mode_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new FlightModeCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_flight_mode([wrapper](Telemetry::FlightMode flight_mode) {
        if (wrapper->user_callback) {
            wrapper->user_callback(flight_mode_from_cpp(flight_mode), wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::FlightModeHandle, FlightModeCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_flight_mode_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_flight_mode(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_flight_mode_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::FlightModeHandle, FlightModeCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_flight_mode(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_health_handle_t mavsdk_telemetry_subscribe_health(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new HealthCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_health([wrapper](Telemetry::Health health) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_health_t c_health;
            copy_health_to_c(health, &c_health);
            wrapper->user_callback(&c_health, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::HealthHandle, HealthCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_health_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_health(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::HealthHandle, HealthCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_health(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_rc_status_handle_t mavsdk_telemetry_subscribe_rc_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_rc_status_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new RcStatusCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_rc_status([wrapper](Telemetry::RcStatus rc_status) {
        if (wrapper->user_callback) {
            mavsdk_telemetry_rc_status_t c_rc_status;
            copy_rc_status_to_c(rc_status, &c_rc_status);
            wrapper->user_callback(&c_rc_status, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::RcStatusHandle, RcStatusCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_rc_status_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_rc_status(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_rc_status_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::RcStatusHandle, RcStatusCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_rc_status(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

mavsdk_telemetry_health_all_ok_handle_t mavsdk_telemetry_subscribe_health_all_ok(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_all_ok_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return nullptr;
    
    auto* wrapper = new HealthAllOkCallbackWrapper{callback, user_data};
    auto cpp_handle = cpp_telemetry->subscribe_health_all_ok([wrapper](bool health_all_ok) {
        if (wrapper->user_callback) {
            wrapper->user_callback(health_all_ok, wrapper->user_data);
        }
    });
    
    auto* handle_pair = new std::pair<Telemetry::HealthAllOkHandle, HealthAllOkCallbackWrapper*>(cpp_handle, wrapper);
    return static_cast<mavsdk_telemetry_health_all_ok_handle_t>(handle_pair);
}

void mavsdk_telemetry_unsubscribe_health_all_ok(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_all_ok_handle_t handle)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    auto* handle_pair = static_cast<std::pair<Telemetry::HealthAllOkHandle, HealthAllOkCallbackWrapper*>*>(handle);
    
    if (cpp_telemetry && handle_pair) {
        cpp_telemetry->unsubscribe_health_all_ok(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

// ===== Rate Setting Methods (Async) =====
void mavsdk_telemetry_set_rate_position_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return;
    
    auto* wrapper = new TelemetryResultCallbackWrapper{callback, user_data};
    cpp_telemetry->set_rate_position_async(rate_hz, [wrapper](Telemetry::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(telemetry_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_telemetry_set_rate_home_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return;
    
    auto* wrapper = new TelemetryResultCallbackWrapper{callback, user_data};
    cpp_telemetry->set_rate_home_async(rate_hz, [wrapper](Telemetry::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(telemetry_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_telemetry_set_rate_in_air_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return;
    
    auto* wrapper = new TelemetryResultCallbackWrapper{callback, user_data};
    cpp_telemetry->set_rate_in_air_async(rate_hz, [wrapper](Telemetry::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(telemetry_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_telemetry_set_rate_attitude_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return;
    
    auto* wrapper = new TelemetryResultCallbackWrapper{callback, user_data};
    cpp_telemetry->set_rate_attitude_quaternion_async(rate_hz, [wrapper](Telemetry::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(telemetry_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_telemetry_set_rate_velocity_ned_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return;
    
    auto* wrapper = new TelemetryResultCallbackWrapper{callback, user_data};
    cpp_telemetry->set_rate_velocity_ned_async(rate_hz, [wrapper](Telemetry::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(telemetry_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_telemetry_set_rate_gps_info_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return;
    
    auto* wrapper = new TelemetryResultCallbackWrapper{callback, user_data};
    cpp_telemetry->set_rate_gps_info_async(rate_hz, [wrapper](Telemetry::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(telemetry_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_telemetry_set_rate_battery_async(
    mavsdk_telemetry_t telemetry,
    double rate_hz,
    mavsdk_telemetry_result_callback_t callback,
    void* user_data)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return;
    
    auto* wrapper = new TelemetryResultCallbackWrapper{callback, user_data};
    cpp_telemetry->set_rate_battery_async(rate_hz, [wrapper](Telemetry::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(telemetry_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

// ===== Rate Setting Methods (Sync) =====
mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_position(
    mavsdk_telemetry_t telemetry,
    double rate_hz)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    return telemetry_result_from_cpp(cpp_telemetry->set_rate_position(rate_hz));
}

mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_home(
    mavsdk_telemetry_t telemetry,
    double rate_hz)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    return telemetry_result_from_cpp(cpp_telemetry->set_rate_home(rate_hz));
}

mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_in_air(
    mavsdk_telemetry_t telemetry,
    double rate_hz)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    return telemetry_result_from_cpp(cpp_telemetry->set_rate_in_air(rate_hz));
}

mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_attitude(
    mavsdk_telemetry_t telemetry,
    double rate_hz)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    return telemetry_result_from_cpp(cpp_telemetry->set_rate_attitude_quaternion(rate_hz));
}

mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_velocity_ned(
    mavsdk_telemetry_t telemetry,
    double rate_hz)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    return telemetry_result_from_cpp(cpp_telemetry->set_rate_velocity_ned(rate_hz));
}

mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_gps_info(
    mavsdk_telemetry_t telemetry,
    double rate_hz)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    return telemetry_result_from_cpp(cpp_telemetry->set_rate_gps_info(rate_hz));
}

mavsdk_telemetry_result_t mavsdk_telemetry_set_rate_battery(
    mavsdk_telemetry_t telemetry,
    double rate_hz)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    return telemetry_result_from_cpp(cpp_telemetry->set_rate_battery(rate_hz));
}

// ===== Polling Methods =====
mavsdk_telemetry_result_t mavsdk_telemetry_position(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_t* position_out)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry || !position_out) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    
    auto position = cpp_telemetry->position();
    copy_position_to_c(position, position_out);
    return MAVSDK_TELEMETRY_RESULT_SUCCESS;
}

mavsdk_telemetry_result_t mavsdk_telemetry_home(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_position_t* home_out)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry || !home_out) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    
    auto home = cpp_telemetry->home();
    copy_position_to_c(home, home_out);
    return MAVSDK_TELEMETRY_RESULT_SUCCESS;
}

mavsdk_telemetry_result_t mavsdk_telemetry_in_air(
    mavsdk_telemetry_t telemetry,
    bool* in_air_out)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry || !in_air_out) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    
    *in_air_out = cpp_telemetry->in_air();
    return MAVSDK_TELEMETRY_RESULT_SUCCESS;
}

mavsdk_telemetry_result_t mavsdk_telemetry_landed_state(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_landed_state_t* landed_state_out)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry || !landed_state_out) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    
    auto landed_state = cpp_telemetry->landed_state();
    *landed_state_out = landed_state_from_cpp(landed_state);
    return MAVSDK_TELEMETRY_RESULT_SUCCESS;
}

mavsdk_telemetry_result_t mavsdk_telemetry_armed(
    mavsdk_telemetry_t telemetry,
    bool* armed_out)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry || !armed_out) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    
    *armed_out = cpp_telemetry->armed();
    return MAVSDK_TELEMETRY_RESULT_SUCCESS;
}

mavsdk_telemetry_result_t mavsdk_telemetry_flight_mode(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_flight_mode_t* flight_mode_out)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry || !flight_mode_out) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    
    auto flight_mode = cpp_telemetry->flight_mode();
    *flight_mode_out = flight_mode_from_cpp(flight_mode);
    return MAVSDK_TELEMETRY_RESULT_SUCCESS;
}

mavsdk_telemetry_result_t mavsdk_telemetry_health(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_health_t* health_out)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry || !health_out) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    
    auto health = cpp_telemetry->health();
    copy_health_to_c(health, health_out);
    return MAVSDK_TELEMETRY_RESULT_SUCCESS;
}

mavsdk_telemetry_result_t mavsdk_telemetry_battery(
    mavsdk_telemetry_t telemetry,
    mavsdk_telemetry_battery_t* battery_out)
{
    auto* cpp_telemetry = static_cast<Telemetry*>(telemetry);
    if (!cpp_telemetry || !battery_out) return MAVSDK_TELEMETRY_RESULT_NO_SYSTEM;
    
    auto battery = cpp_telemetry->battery();
    copy_battery_to_c(battery, battery_out);
    return MAVSDK_TELEMETRY_RESULT_SUCCESS;
}

} // extern "C"
