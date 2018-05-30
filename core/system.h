#pragma once

#include <memory>

namespace dronecore {

class SystemImpl;
class DroneCoreImpl;
class PluginImplBase;

/**
 * @brief This class represents a system, made up of one or more components
 * (e.g. autopilot, cameras, servos, gimbals, etc).
 *
 * System objects are used to interact with UAVs (including their components) and standalone
 * cameras. They are not created directly by application code, but are returned by the DroneCore
 * class.
 */
class System {
public:
    /**
     * @brief Constructor.
     *
     * This constructor is not (and should not be) directly called by application code.
     *
     * @param system_id %System id.
     * @param comp_id Component id.
     */
    explicit System(DroneCoreImpl &parent, uint8_t system_id, uint8_t comp_id);
    /**
     * @brief Destructor.
     */
    ~System();

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
     * @brief Get the UUID of the system.
     *
     * @return UUID of system.
     */
    uint64_t get_uuid() const;

    // Non-copyable
    /**
     * @brief Copy constructor (object is not copyable).
     */
    System(const System &) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const System &operator=(const System &) = delete;

private:
    std::shared_ptr<SystemImpl> system_impl() { return _system_impl; };

    /*
     * DroneCoreImpl and PluginImplBase need access to SystemImpl class.
     * This is not pretty but it's not easy to hide the methods from library
     * users if not like that (or with an ugly reinterpret_cast).
     */
    friend DroneCoreImpl;
    friend PluginImplBase;

    std::shared_ptr<SystemImpl> _system_impl;
};

} // namespace dronecore
