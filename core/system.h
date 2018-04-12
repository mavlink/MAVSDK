#pragma once

#include "global_include.h"
#include "mavlink_include.h"

namespace dronecore {

class MAVLinkSystem;
class DroneCoreImpl;
class PluginImplBase;

/**
 * @brief This class represents a system, made up of one or more components
 * (e.g. autopilot, cameras, servos, gimbals, etc).
 *
 * System objects are used to interact with UAVs (including their components) and standalone cameras.
 * They are not created directly by application code, but are returned by the DroneCore class.
 */
class System
{
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

    void add_new_component(uint8_t component_id);
    void process_mavlink_message(const mavlink_message_t &message);
    void set_system_id(uint8_t system_id);
    bool is_connected() const;
    uint64_t get_uuid() const;
    uint8_t get_system_id() const;

    std::shared_ptr<MAVLinkSystem> mavlink_system() { return _mavlink_system; };

    /* TODO: Optimize this later.
     * For now,
     * - DroneCoreImpl wants to access private methods of System.
     * - PluginImplBase requests System class to get instance of MAVLinkSystem class.
    */
    friend DroneCoreImpl;
    friend PluginImplBase;

    std::shared_ptr<MAVLinkSystem> _mavlink_system;
};


} // namespace dronecore
