#include "system.h" // C wrapper header
#include <mavsdk/mavsdk.h>
#include <mavsdk/system.h>
#include <mavsdk/component_type.h>
#include <cstring>
#include <vector>

using namespace mavsdk;

extern "C" {

// --- Creation / Destruction ---
void mavsdk_system_destroy(mavsdk_system_t system) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    delete cpp_system_ptr;
}

// --- Basic Queries ---
bool mavsdk_system_has_autopilot(mavsdk_system_t system) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    return (*cpp_system_ptr)->has_autopilot();
}

bool mavsdk_system_is_standalone(mavsdk_system_t system) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    return (*cpp_system_ptr)->is_standalone();
}

bool mavsdk_system_has_camera(mavsdk_system_t system, int camera_id) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    return (*cpp_system_ptr)->has_camera(camera_id);
}

bool mavsdk_system_has_gimbal(mavsdk_system_t system) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    return (*cpp_system_ptr)->has_gimbal();
}

bool mavsdk_system_is_connected(mavsdk_system_t system) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    return (*cpp_system_ptr)->is_connected();
}

uint8_t mavsdk_system_get_system_id(mavsdk_system_t system) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    return (*cpp_system_ptr)->get_system_id();
}

// --- Component IDs ---
uint8_t* mavsdk_system_component_ids(mavsdk_system_t system, size_t* count) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    auto ids = (*cpp_system_ptr)->component_ids();
    *count = ids.size();
    if (ids.empty()) {
        return nullptr;
    }
    uint8_t* c_ids = new uint8_t[ids.size()];
    std::memcpy(c_ids, ids.data(), ids.size() * sizeof(uint8_t));
    return c_ids;
}

void mavsdk_system_free_component_ids(uint8_t* ids) {
    delete[] ids;
}

// --- Subscriptions ---

// Internal callback wrapper structures
struct IsConnectedCallbackWrapper {
    mavsdk_is_connected_callback_t user_callback;
    void* user_data;
};

struct ComponentDiscoveredCallbackWrapper {
    mavsdk_component_discovered_callback_t user_callback;
    void* user_data;
};

struct ComponentDiscoveredIdCallbackWrapper {
    mavsdk_component_discovered_id_callback_t user_callback;
    void* user_data;
};

// Is Connected subscription
mavsdk_is_connected_handle_t mavsdk_system_subscribe_is_connected(
    mavsdk_system_t system,
    mavsdk_is_connected_callback_t callback,
    void* user_data)
{
    auto* cpp_system_ptr = static_cast<System**>(system);
    
    auto* wrapper = new IsConnectedCallbackWrapper{callback, user_data};
    
    auto cpp_handle = (*cpp_system_ptr)->subscribe_is_connected(
        [wrapper](bool is_connected) {
            if (wrapper->user_callback) {
                wrapper->user_callback(is_connected, wrapper->user_data);
            }
        }
    );
    
    // Store both the C++ handle and wrapper
    auto* handle_pair = new std::pair<System::IsConnectedHandle, IsConnectedCallbackWrapper*>(
        cpp_handle, wrapper
    );
    
    return static_cast<mavsdk_is_connected_handle_t>(handle_pair);
}

void mavsdk_system_unsubscribe_is_connected(
    mavsdk_system_t system,
    mavsdk_is_connected_handle_t handle)
{
    auto* cpp_system_ptr = static_cast<System**>(system);
    auto* handle_pair = static_cast<std::pair<System::IsConnectedHandle, IsConnectedCallbackWrapper*>*>(handle);
    
    if (handle_pair) {
        (*cpp_system_ptr)->unsubscribe_is_connected(handle_pair->first);
        delete handle_pair->second; // delete the wrapper
        delete handle_pair; // delete the pair
    }
}

// Component Discovered subscription
mavsdk_component_discovered_handle_t mavsdk_system_subscribe_component_discovered(
    mavsdk_system_t system,
    mavsdk_component_discovered_callback_t callback,
    void* user_data)
{
    auto* cpp_system_ptr = static_cast<System**>(system);
    
    auto* wrapper = new ComponentDiscoveredCallbackWrapper{callback, user_data};
    
    auto cpp_handle = (*cpp_system_ptr)->subscribe_component_discovered(
        [wrapper](ComponentType component_type) {
            if (wrapper->user_callback) {
                wrapper->user_callback(static_cast<mavsdk_component_type_t>(component_type), wrapper->user_data);
            }
        }
    );
    
    auto* handle_pair = new std::pair<System::ComponentDiscoveredHandle, ComponentDiscoveredCallbackWrapper*>(
        cpp_handle, wrapper
    );
    
    return static_cast<mavsdk_component_discovered_handle_t>(handle_pair);
}

void mavsdk_system_unsubscribe_component_discovered(
    mavsdk_system_t system,
    mavsdk_component_discovered_handle_t handle)
{
    auto* cpp_system_ptr = static_cast<System**>(system);
    auto* handle_pair = static_cast<std::pair<System::ComponentDiscoveredHandle, ComponentDiscoveredCallbackWrapper*>*>(handle);
    
    if (handle_pair) {
        (*cpp_system_ptr)->unsubscribe_component_discovered(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

// Component Discovered with ID subscription
mavsdk_component_discovered_id_handle_t mavsdk_system_subscribe_component_discovered_id(
    mavsdk_system_t system,
    mavsdk_component_discovered_id_callback_t callback,
    void* user_data)
{
    auto* cpp_system_ptr = static_cast<System**>(system);
    
    auto* wrapper = new ComponentDiscoveredIdCallbackWrapper{callback, user_data};
    
    auto cpp_handle = (*cpp_system_ptr)->subscribe_component_discovered_id(
        [wrapper](ComponentType component_type, uint8_t component_id) {
            if (wrapper->user_callback) {
                wrapper->user_callback(static_cast<mavsdk_component_type_t>(component_type), component_id, wrapper->user_data);
            }
        }
    );
    
    auto* handle_pair = new std::pair<System::ComponentDiscoveredIdHandle, ComponentDiscoveredIdCallbackWrapper*>(
        cpp_handle, wrapper
    );
    
    return static_cast<mavsdk_component_discovered_id_handle_t>(handle_pair);
}

void mavsdk_system_unsubscribe_component_discovered_id(
    mavsdk_system_t system,
    mavsdk_component_discovered_id_handle_t handle)
{
    auto* cpp_system_ptr = static_cast<System**>(system);
    auto* handle_pair = static_cast<std::pair<System::ComponentDiscoveredIdHandle, ComponentDiscoveredIdCallbackWrapper*>*>(handle);
    
    if (handle_pair) {
        (*cpp_system_ptr)->unsubscribe_component_discovered_id(handle_pair->first);
        delete handle_pair->second;
        delete handle_pair;
    }
}

// --- Other System Functions ---
void mavsdk_system_enable_timesync(mavsdk_system_t system) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    (*cpp_system_ptr)->enable_timesync();
}

mavsdk_autopilot_t mavsdk_system_autopilot_type(mavsdk_system_t system) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    switch ((*cpp_system_ptr)->autopilot_type()) {
        case Autopilot::Px4: return MAVSDK_AUTOPILOT_PX4;
        case Autopilot::ArduPilot: return MAVSDK_AUTOPILOT_ARDUPILOT;
        case Autopilot::Unknown:
        default: return MAVSDK_AUTOPILOT_GENERIC;
    }
}

mavsdk_vehicle_t mavsdk_system_vehicle_type(mavsdk_system_t system) {
    auto* cpp_system_ptr = static_cast<System**>(system);
    switch ((*cpp_system_ptr)->vehicle_type()) {
        case Vehicle::FixedWing: return MAVSDK_VEHICLE_FIXED_WING;
        case Vehicle::Quadrotor:
        case Vehicle::Hexarotor:
        case Vehicle::Octorotor:
        case Vehicle::Decarotor:
        case Vehicle::Dodecarotor:
        case Vehicle::Tricopter:
        case Vehicle::GenericMultirotor:
            return MAVSDK_VEHICLE_MULTICOPTER;
        case Vehicle::Helicopter:
        case Vehicle::Coaxial:
            return MAVSDK_VEHICLE_MULTICOPTER; // Map helicopters to multicopter for C API
        case Vehicle::GroundRover: return MAVSDK_VEHICLE_ROVER;
        case Vehicle::Submarine: return MAVSDK_VEHICLE_SUBMARINE;
        case Vehicle::Generic:
        case Vehicle::Unknown:
        default: return MAVSDK_VEHICLE_GENERIC;
    }
}

} // extern "C"
