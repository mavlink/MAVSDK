#pragma once
#include "system_impl.h"
#include <memory>

namespace mavsdk {

class System;
class SystemImpl;

class PluginImplBase {
public:
    explicit PluginImplBase(System& system); // to be deprecated
    explicit PluginImplBase(std::shared_ptr<System> system); // new
    virtual ~PluginImplBase() = default;

    /*
     * The method `init()` is called when a plugin is instantiated which happens
     * when a system is constructed. This does not mean that the system actually
     * exists and is connected, it might just be an empty dummy system.
     *
     * Plugins should do initialization steps with other parts of MAVSDK
     * at this state, e.g. set up callbacks with _system_impl (System).
     */
    virtual void init() = 0;

    /*
     * The method `deinit()` is called before a plugin is destructed which happens
     * usually only at the very end when a Mavsdk instance is destructed.
     *
     * Plugins should do any cleanup of what has been set up during init.
     */
    virtual void deinit() = 0;

    /*
     * The method `enable()` is called when a system is discovered (is connected).
     *
     * Plugins should do all initialization/configuration steps here that require a
     * system to be connected such as setting/getting parameters.
     *
     * If any threads, call_every or timeouts are needed, they can be started now.
     */
    virtual void enable() = 0;

    /*
     * The method `disable()` is called when a system has timed out. The method is also
     * called before `deinit()` is called in case we destruct with a system still
     * connected.
     *
     * Plugins should stop whatever they were doing in order to prevent warnings and
     * errors because communication to the system no longer work, e.g. stop setting
     * parameters or commands.
     *
     * If any threads, call_every, or timeouts are running, they should be stopped now.
     */
    virtual void disable() = 0;

    // Non-copyable
    PluginImplBase(const PluginImplBase&) = delete;
    const PluginImplBase& operator=(const PluginImplBase&) = delete;

protected:
    std::shared_ptr<SystemImpl> _system_impl;
};

} // namespace mavsdk
