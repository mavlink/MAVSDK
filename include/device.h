#pragma once

#include "device_plugin_container.h"

namespace dronecore {

class DeviceImpl;

/**
 * @brief A Device represents a vehicle such as a drone or robot.
 *
 * A device can consist of multiple components such as an autopilot with a gimbal and camera.
 *
 * This class is derived from DevicePluginContainer, which provides methods
 * to access plugin classes like Action, Telemetry, Info, Logging, Offboard and Mission
 * (for example, using `DroneCore::device().action()...`).
 *
 * **NOTE** The content of DevicePluginContainer, and hence the available accessors,
 * are auto-generated at compile time.
 *
 * Device objects are not created or destroyed directly by API consumers. They are accessed using, for example, DroneCore::device() and cleaned up when DroneCore is destroyed.
 */
class Device : public DevicePluginContainer
{
public:
    /**
     * @brief Constructor (internal use only).
     *
     * This constructor does not need to be called by any consumer of the API.
     *
     * @param impl The underlying device implementation.
     */
    explicit Device(DeviceImpl *impl);

    /**
     * @brief Destructor (internal use only).
     *
     * The destructor of Device does not need to be called by any consumer of the API.
     */
    ~Device();

private:
    // Non-copyable
    Device(const Device &) = delete;
    const Device &operator=(const Device &) = delete;
};

} // namespace dronecore
