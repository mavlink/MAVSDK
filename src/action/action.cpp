#include "action/action.h"

#include <mavsdk/plugins/action/action.h>
#include <functional>
#include <memory>

using namespace mavsdk;

// Helper to convert C++ Action::Result to C mavsdk_action_result_t
static mavsdk_action_result_t action_result_from_cpp(Action::Result result)
{
    switch (result) {
        case Action::Result::Success:
            return MAVSDK_ACTION_RESULT_SUCCESS;
        case Action::Result::NoSystem:
            return MAVSDK_ACTION_RESULT_NO_SYSTEM;
        case Action::Result::ConnectionError:
            return MAVSDK_ACTION_RESULT_CONNECTION_ERROR;
        case Action::Result::Busy:
            return MAVSDK_ACTION_RESULT_BUSY;
        case Action::Result::CommandDenied:
            return MAVSDK_ACTION_RESULT_COMMAND_DENIED;
        case Action::Result::CommandDeniedLandedStateUnknown:
            return MAVSDK_ACTION_RESULT_COMMAND_DENIED_LANDED_STATE_UNKNOWN;
        case Action::Result::CommandDeniedNotLanded:
            return MAVSDK_ACTION_RESULT_COMMAND_DENIED_NOT_LANDED;
        case Action::Result::Timeout:
            return MAVSDK_ACTION_RESULT_TIMEOUT;
        case Action::Result::VtolTransitionSupportUnknown:
            return MAVSDK_ACTION_RESULT_VTOL_TRANSITION_SUPPORT_UNKNOWN;
        case Action::Result::NoVtolTransitionSupport:
            return MAVSDK_ACTION_RESULT_NO_VTOL_TRANSITION_SUPPORT;
        case Action::Result::ParameterError:
            return MAVSDK_ACTION_RESULT_PARAMETER_ERROR;
        case Action::Result::Unsupported:
            return MAVSDK_ACTION_RESULT_UNSUPPORTED;
        case Action::Result::Failed:
            return MAVSDK_ACTION_RESULT_FAILED;
        case Action::Result::InvalidArgument:
            return MAVSDK_ACTION_RESULT_INVALID_ARGUMENT;
        case Action::Result::Unknown:
        default:
            return MAVSDK_ACTION_RESULT_UNKNOWN;
    }
}

// Helper to convert C orbit yaw behavior to C++
static Action::OrbitYawBehavior orbit_yaw_behavior_to_cpp(mavsdk_action_orbit_yaw_behavior_t yaw_behavior)
{
    switch (yaw_behavior) {
        case MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_HOLD_FRONT_TO_CIRCLE_CENTER:
            return Action::OrbitYawBehavior::HoldFrontToCircleCenter;
        case MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_HOLD_INITIAL_HEADING:
            return Action::OrbitYawBehavior::HoldInitialHeading;
        case MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_UNCONTROLLED:
            return Action::OrbitYawBehavior::Uncontrolled;
        case MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_HOLD_FRONT_TANGENT_TO_CIRCLE:
            return Action::OrbitYawBehavior::HoldFrontTangentToCircle;
        case MAVSDK_ACTION_ORBIT_YAW_BEHAVIOR_RC_CONTROLLED:
            return Action::OrbitYawBehavior::RcControlled;
        default:
            return Action::OrbitYawBehavior::HoldFrontToCircleCenter;
    }
}

// Callback wrapper structures
struct ActionResultCallbackWrapper {
    mavsdk_action_result_callback_t user_callback;
    void* user_data;
};

struct GetAltitudeCallbackWrapper {
    mavsdk_action_get_altitude_callback_t user_callback;
    void* user_data;
};

extern "C" {

// ===== Action Creation/Destruction =====
mavsdk_action_t mavsdk_action_create(mavsdk_system_t system)
{
    auto* cpp_system_ptr = static_cast<System**>(system);
    if (!cpp_system_ptr || !*cpp_system_ptr) {
        return nullptr;
    }
    
    // Use the shared_ptr constructor since we're storing System** in our handle
    auto* action = new Action(std::shared_ptr<System>(*cpp_system_ptr, [](System*) {
        // Empty deleter - we don't own the System, the mavsdk_t does
    }));
    return static_cast<mavsdk_action_t>(action);
}

void mavsdk_action_destroy(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    delete cpp_action;
}

// ===== Basic Actions (Async) =====
void mavsdk_action_arm_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->arm_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_arm_force_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->arm_force_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_disarm_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->disarm_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_takeoff_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->takeoff_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_land_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->land_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_reboot_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->reboot_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_shutdown_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->shutdown_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_terminate_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->terminate_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_kill_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->kill_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_return_to_launch_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->return_to_launch_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_hold_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->hold_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_transition_to_fixedwing_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->transition_to_fixedwing_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_transition_to_multicopter_async(mavsdk_action_t action, mavsdk_action_result_callback_t callback, void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->transition_to_multicopter_async([wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

// ===== Basic Actions (Sync) =====
mavsdk_action_result_t mavsdk_action_arm(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->arm());
}

mavsdk_action_result_t mavsdk_action_arm_force(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->arm_force());
}

mavsdk_action_result_t mavsdk_action_disarm(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->disarm());
}

mavsdk_action_result_t mavsdk_action_takeoff(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->takeoff());
}

mavsdk_action_result_t mavsdk_action_land(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->land());
}

mavsdk_action_result_t mavsdk_action_reboot(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->reboot());
}

mavsdk_action_result_t mavsdk_action_shutdown(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->shutdown());
}

mavsdk_action_result_t mavsdk_action_terminate(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->terminate());
}

mavsdk_action_result_t mavsdk_action_kill(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->kill());
}

mavsdk_action_result_t mavsdk_action_return_to_launch(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->return_to_launch());
}

mavsdk_action_result_t mavsdk_action_hold(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->hold());
}

mavsdk_action_result_t mavsdk_action_transition_to_fixedwing(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->transition_to_fixedwing());
}

mavsdk_action_result_t mavsdk_action_transition_to_multicopter(mavsdk_action_t action)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->transition_to_multicopter());
}

// ===== Advanced Actions (Async) =====
void mavsdk_action_goto_location_async(
    mavsdk_action_t action,
    double latitude_deg,
    double longitude_deg,
    float absolute_altitude_m,
    float yaw_deg,
    mavsdk_action_result_callback_t callback,
    void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->goto_location_async(
        latitude_deg, longitude_deg, absolute_altitude_m, yaw_deg,
        [wrapper](Action::Result result) {
            if (wrapper->user_callback) {
                wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
            }
            delete wrapper;
        });
}

void mavsdk_action_do_orbit_async(
    mavsdk_action_t action,
    float radius_m,
    float velocity_ms,
    mavsdk_action_orbit_yaw_behavior_t yaw_behavior,
    double latitude_deg,
    double longitude_deg,
    double absolute_altitude_m,
    mavsdk_action_result_callback_t callback,
    void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->do_orbit_async(
        radius_m, velocity_ms, orbit_yaw_behavior_to_cpp(yaw_behavior),
        latitude_deg, longitude_deg, absolute_altitude_m,
        [wrapper](Action::Result result) {
            if (wrapper->user_callback) {
                wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
            }
            delete wrapper;
        });
}

void mavsdk_action_set_actuator_async(
    mavsdk_action_t action,
    int32_t index,
    float value,
    mavsdk_action_result_callback_t callback,
    void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->set_actuator_async(index, value, [wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

// ===== Advanced Actions (Sync) =====
mavsdk_action_result_t mavsdk_action_goto_location(
    mavsdk_action_t action,
    double latitude_deg,
    double longitude_deg,
    float absolute_altitude_m,
    float yaw_deg)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->goto_location(latitude_deg, longitude_deg, absolute_altitude_m, yaw_deg));
}

mavsdk_action_result_t mavsdk_action_do_orbit(
    mavsdk_action_t action,
    float radius_m,
    float velocity_ms,
    mavsdk_action_orbit_yaw_behavior_t yaw_behavior,
    double latitude_deg,
    double longitude_deg,
    double absolute_altitude_m)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->do_orbit(
        radius_m, velocity_ms, orbit_yaw_behavior_to_cpp(yaw_behavior),
        latitude_deg, longitude_deg, absolute_altitude_m));
}

mavsdk_action_result_t mavsdk_action_set_actuator(
    mavsdk_action_t action,
    int32_t index,
    float value)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->set_actuator(index, value));
}

// ===== Configuration Getters (Async) =====
void mavsdk_action_get_takeoff_altitude_async(
    mavsdk_action_t action,
    mavsdk_action_get_altitude_callback_t callback,
    void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new GetAltitudeCallbackWrapper{callback, user_data};
    cpp_action->get_takeoff_altitude_async([wrapper](Action::Result result, float altitude) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), altitude, wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_get_return_to_launch_altitude_async(
    mavsdk_action_t action,
    mavsdk_action_get_altitude_callback_t callback,
    void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new GetAltitudeCallbackWrapper{callback, user_data};
    cpp_action->get_return_to_launch_altitude_async([wrapper](Action::Result result, float altitude) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), altitude, wrapper->user_data);
        }
        delete wrapper;
    });
}

// ===== Configuration Getters (Sync) =====
mavsdk_action_result_t mavsdk_action_get_takeoff_altitude(
    mavsdk_action_t action,
    float* altitude_out)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action || !altitude_out) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    
    auto result = cpp_action->get_takeoff_altitude();
    *altitude_out = result.second;
    return action_result_from_cpp(result.first);
}

mavsdk_action_result_t mavsdk_action_get_return_to_launch_altitude(
    mavsdk_action_t action,
    float* altitude_out)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action || !altitude_out) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    
    auto result = cpp_action->get_return_to_launch_altitude();
    *altitude_out = result.second;
    return action_result_from_cpp(result.first);
}

// ===== Configuration Setters (Async) =====
void mavsdk_action_set_takeoff_altitude_async(
    mavsdk_action_t action,
    float altitude,
    mavsdk_action_result_callback_t callback,
    void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->set_takeoff_altitude_async(altitude, [wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_set_return_to_launch_altitude_async(
    mavsdk_action_t action,
    float relative_altitude_m,
    mavsdk_action_result_callback_t callback,
    void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->set_return_to_launch_altitude_async(relative_altitude_m, [wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

void mavsdk_action_set_current_speed_async(
    mavsdk_action_t action,
    float speed_m_s,
    mavsdk_action_result_callback_t callback,
    void* user_data)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return;
    
    auto* wrapper = new ActionResultCallbackWrapper{callback, user_data};
    cpp_action->set_current_speed_async(speed_m_s, [wrapper](Action::Result result) {
        if (wrapper->user_callback) {
            wrapper->user_callback(action_result_from_cpp(result), wrapper->user_data);
        }
        delete wrapper;
    });
}

// ===== Configuration Setters (Sync) =====
mavsdk_action_result_t mavsdk_action_set_takeoff_altitude(
    mavsdk_action_t action,
    float altitude)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->set_takeoff_altitude(altitude));
}

mavsdk_action_result_t mavsdk_action_set_return_to_launch_altitude(
    mavsdk_action_t action,
    float relative_altitude_m)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->set_return_to_launch_altitude(relative_altitude_m));
}

mavsdk_action_result_t mavsdk_action_set_current_speed(
    mavsdk_action_t action,
    float speed_m_s)
{
    auto* cpp_action = static_cast<Action*>(action);
    if (!cpp_action) return MAVSDK_ACTION_RESULT_NO_SYSTEM;
    return action_result_from_cpp(cpp_action->set_current_speed(speed_m_s));
}

} // extern "C"
