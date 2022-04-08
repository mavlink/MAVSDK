#pragma once

#include <memory>
#include <array>
#include <functional>
#include <vector>

#include "deprecated.h"

namespace mavsdk {

class SystemImpl;
class MavsdkImpl;
class PluginImplBase;

/**
 * @brief This class represents a system, made up of one or more components
 * (e.g. autopilot, cameras, servos, gimbals, etc).
 *
 * System objects are used to interact with UAVs (including their components) and standalone
 * cameras. They are not created directly by application code, but are returned by the Mavsdk
 * class.
 */
class System {
public:
    /** @private Constructor, used internally
     *
     * This constructor is not (and should not be) directly called by application code.
     *
     * @param parent `MavsdkImpl` dependency.
     */
    explicit System(MavsdkImpl& parent);
    /**
     * @brief Destructor.
     */
    ~System();

    /**
     * @brief Initialize the system.
     *
     * This is not (and should not be) directly called by application code.
     *
     * @param system_id System id.
     * @param component_id Component id.
     * @param connected If true then the system doesn't wait for heartbeat to go into connected
     * state
     */
    void init(uint8_t system_id, uint8_t component_id, bool connected) const;

    /**
     * @brief Checks whether the system has an autopilot.
     * @return `true` if it has an autopilot, `false` otherwise.
     */
    bool has_autopilot() const;

    /**
     * @brief Checks whether the system is a standalone (non-autopilot).
     * @return `true` if stand alone, `false` otherwise.
     */
    bool is_standalone() const;

    /**
     * @brief Checks whether the system has a camera with the given camera ID.
     *
     * A System may have several cameras, with IDs starting from 0.
     * When called without passing a camera ID, it checks whether the system has
     * any camera.
     * @param camera_id ID of the camera starting from 0 onwards.
     * @return `true` if camera with the given camera ID is found, `false` otherwise.
     */
    bool has_camera(int camera_id = -1) const;

    /**
     * @brief Checks whether the system has a gimbal.
     * @return `true` if the system has a gimbal, false otherwise.
     */
    bool has_gimbal() const;

    /**
     * @brief Checks if the system is connected.
     *
     * A system is connected when heartbeats are arriving (discovered and not timed out).
     * @return `true` if the system is connected.
     */
    bool is_connected() const;

    /**
     * @brief MAVLink System ID of connected system.
     *
     * @note: this is 0 if nothing is connected yet.
     *
     * @return the system ID.
     */
    uint8_t get_system_id() const;

    /**
     * @brief MAVLink component IDs of connected system.
     *
     * @note: all components that have been seen at least once will be listed.
     *
     * @return a list of all component ids
     */
    std::vector<uint8_t> component_ids() const;

    /**
     * @brief type for is connected callback.
     */
    using IsConnectedCallback = std::function<void(bool)>;

    /**
     * @brief Subscribe to callback to be called when system connection state changes.
     *
     * @param callback Callback which will be called.
     */
    void subscribe_is_connected(IsConnectedCallback callback);

    /**
     * @brief Component Types
     */
    enum class ComponentType { UNKNOWN, AUTOPILOT, CAMERA, GIMBAL };

    /**
     * @brief type for component discovery callback
     */
    using DiscoverCallback = std::function<void(ComponentType)>;

    /**
     * @brief type for component discovery callback
     */
    using DiscoverIdCallback = std::function<void(ComponentType, uint8_t)>;

    /**
     * @brief Register a callback to be called when a component is discovered.
     *
     * @param callback a function of type void(ComponentType) which will be called with the
     * component type of the new component.
     */
    void register_component_discovered_callback(DiscoverCallback callback) const;

    /**
     * @brief Register a callback to be called when a component is discovered.
     *
     * @param callback a function of type void(ComponentType) which will be called with the
     * component type and the component id of the new component.
     */
    void register_component_discovered_id_callback(DiscoverIdCallback callback) const;

    /**
     * @brief Enable time synchronization using the TIMESYNC messages.
     */
    void enable_timesync();

    /**
     * @brief Copy constructor (object is not copyable).
     */
    System(const System&) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const System& operator=(const System&) = delete;

private:
    std::shared_ptr<SystemImpl> system_impl() { return _system_impl; };

    /*
     * MavsdkImpl and PluginImplBase need access to SystemImpl class.
     * This is not pretty, but it's not easy to hide the methods from library
     * users if not like that (or with an ugly reinterpret_cast).
     */
    friend MavsdkImpl;
    friend PluginImplBase;

    std::shared_ptr<SystemImpl> _system_impl;
};

} // namespace mavsdk
