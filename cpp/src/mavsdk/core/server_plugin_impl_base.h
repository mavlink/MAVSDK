#pragma once

#include "server_component.h"
#include "server_component_impl.h"
#include "unused.h"

#include <memory>

namespace mavsdk {

class ServerPluginImplBase {
public:
    explicit ServerPluginImplBase(std::shared_ptr<ServerComponent> server_component);
    virtual ~ServerPluginImplBase() = default;

    /*
     * The method `init()` is called when a plugin is instantiated.
     */
    virtual void init() = 0;

    /*
     * The method `deinit()` is called before a plugin is destructed which happens
     * usually only at the very end when a Mavsdk instance is destructed.
     *
     * Plugins should do any cleanup of what has been set up during init.
     */
    virtual void deinit() = 0;

    ServerPluginImplBase(const ServerPluginImplBase&) = delete;
    const ServerPluginImplBase& operator=(const ServerPluginImplBase&) = delete;

protected:
    std::shared_ptr<ServerComponentImpl> _server_component_impl;
};

} // namespace mavsdk
