#pragma once

namespace dronecore {

class DeviceImpl;

class PluginImplBase
{
public:
    PluginImplBase();
    virtual ~PluginImplBase() = default;

    void set_parent(DeviceImpl *parent);

    /*
     * The method `init()` is called when a plugin is instantiated which happens
     * when a device is constructed. This does not mean that the device actually
     * exists and is connected, it might just be an empty dummy device.
     *
     * Plugins should do initialization steps with other parts of DroneCore
     * at this state, e.g. set up callbacks with _parent (DeviceImpl).
     */
    virtual void init() = 0;

    /*
     * The method `deinit()` is called before a plugin is destructed which happens
     * usually only at the very end when a DroneCore instance is destructed.
     *
     * Plugins should do any cleanup of what has been set up during init.
     */
    virtual void deinit() = 0;

    /*
     * The method `enable()` is called when a device is discovered (is connected).
     *
     * Plugins should do all initialization/configuration steps here that require a
     * device to be connected such as setting/getting parameters.
     *
     * If any threads, call_every or timeouts are needed, they can be started now.
     */
    virtual void enable() = 0;

    /*
     * The method `disable()` is called when a device has timed out.
     *
     * Plugins should stop whatever they were doing in order to prevent warnings and
     * errors because communication to the device no longer work, e.g. stop setting
     * parameters or commands.
     *
     * If any threads, call_every, or timeouts are running, they should be stopped now.
     */
    virtual void disable() = 0;

    // Non-copyable
    PluginImplBase(const PluginImplBase &) = delete;
    const PluginImplBase &operator=(const PluginImplBase &) = delete;

protected:
    DeviceImpl *_parent;
};

} // namespace dronecore
