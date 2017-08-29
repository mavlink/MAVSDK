#pragma once

#include "device_plugin_container.h"

namespace dronecore {

class DeviceImpl;

/**
 * A Device represents a unit such as a drone or robot.
 *
 * A device can consists of multiple components such as an autopilot with a gimbal and camera.
 * The device class allows to access plugins such as Action or Telemetry because it is based
 * on DevicePluginContainer.
 */
class Device : public DevicePluginContainer
{
public:
    /**
     * Constructor of a device called internally.
     *
     * This constructor does not need to be called by any consumer of the API. Instead
     * the device can be accessed using e.g. `DroneCore::device()`.
     *
     * @param impl the underlying device implementation
     */
    explicit Device(DeviceImpl *impl);

    /**
     * Destructor of a device called internally.
     *
     * The destructor of Device does not need to be called by any consumer of the API. Instead
     * the device will be cleaned up when DroneCore is destroyed.
     */
    ~Device();

private:
    // Non-copyable
    Device(const Device &) = delete;
    const Device &operator=(const Device &) = delete;
};

} // namespace dronecore
