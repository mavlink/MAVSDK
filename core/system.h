#pragma once

#include "global_include.h"
#include "mavlink_include.h"
#include "mavlink_parameters.h"
#include "mavlink_commands.h"
#include "timeout_handler.h"
#include "call_every_handler.h"
#include <cstdint>
#include <functional>
#include <atomic>
#include <vector>
#include <unordered_set>
#include <map>
#include <thread>
#include <mutex>

namespace dronecore {

class MAVLinkSystem;
class DroneCoreImpl;
class PluginImplBase;

/**
 * @brief This class represents the MAVLink based Vehicle/Robot
 * or a stand-alone system whom DroneCore application want to interact with.
 */
class System
{
public:
    explicit System(DroneCoreImpl &parent, uint8_t system_id, uint8_t comp_id);
    ~System();

    /**
     * @brief Checks whether the system is a standalone (Non-autopilot).
     * @return true if stand alone, false otherwise.
     */
    bool is_standalone() const;

    /**
     * @brief Checks whether the system is an autopilot.
     * @return true if its an autopilot, false otherwise.
     */
    bool is_autopilot() const;

    /**
     * @brief Checks whether the system has camera with the given camera ID.
     *
     * A System may have atmost 6 cameras with ID ranging (1-6).
     * When called without passing camera ID, it checks for the camera ID 1.
     * @param camera_id ID of the camera in the range (1-6).
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
