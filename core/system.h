#pragma once

#include "global_include.h"
#include "mavlink_include.h"

namespace dronecore {

class MAVLinkSystem;
class DroneCoreImpl;
class PluginImplBase;

/**
 * @brief This class represents a system, made up of one or more components
 * (e.g. autopilot, cameras, servos, gimbals, etc). Commonly System objects
 * are used to interact with UAVs (including their components) and standalone cameras.
 */
class System
{
public:
    explicit System(DroneCoreImpl &parent, uint8_t system_id, uint8_t comp_id);
    ~System();

    /**
     * @brief Checks whether the system is an autopilot.
     * @return true if its an autopilot, false otherwise.
     */
    bool is_autopilot() const;

    /**
     * @brief Checks whether the system is a standalone (Non-autopilot).
     * @return true if stand alone, false otherwise.
     */
    bool is_standalone() const;

    /**
     * @brief Checks whether the system has camera with the given camera ID.
     *
     * A System may have several cameras with ID starting from 1.
     * When called without passing camera ID, it checks for the camera ID 1.
     * @param camera_id ID of the camera starting from 1 onwards.
     * @return true if camera with the given camera ID is found, false otherwise.
     */
    bool has_camera(uint8_t camera_id = 1) const;

    /**
     * @brief Checks whether the system has gimbal.
     * @return true if the system has gimbal, false otherwise.
     */
    bool has_gimbal() const;

    friend class DroneCoreImpl;
    friend class PluginImplBase;

    // Non-copyable
    System(const System &) = delete;
    const System &operator=(const System &) = delete;

private:

    void add_new_component(uint8_t component_id);
    void process_mavlink_message(const mavlink_message_t &message);
    void set_system_id(uint8_t system_id);
    bool is_connected() const;
    uint64_t get_uuid() const;
    uint8_t get_system_id() const;

    std::shared_ptr<MAVLinkSystem> mavlink_system() { return _mavlink_system; };

    std::shared_ptr<MAVLinkSystem> _mavlink_system;
};


} // namespace dronecore
