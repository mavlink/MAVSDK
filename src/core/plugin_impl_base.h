#pragma once
#include "node_impl.h"
#include <memory>

namespace mavsdk {

class Node;
class NodeImpl;

class PluginImplBase {
public:
    explicit PluginImplBase(Node& node);
    virtual ~PluginImplBase() = default;

    /*
     * The method `init()` is called when a plugin is instantiated which happens
     * when a node is constructed. This does not mean that the node actually
     * exists and is connected, it might just be an empty dummy node.
     *
     * Plugins should do initialization steps with other parts of the Dronecode SDK
     * at this state, e.g. set up callbacks with _parent (Node).
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
     * The method `enable()` is called when a node is discovered (is connected).
     *
     * Plugins should do all initialization/configuration steps here that require a
     * node to be connected such as setting/getting parameters.
     *
     * If any threads, call_every or timeouts are needed, they can be started now.
     */
    virtual void enable() = 0;

    /*
     * The method `disable()` is called when a node has timed out. The method is also
     * called before `deinit()` is called in case we destruct with a node still
     * connected.
     *
     * Plugins should stop whatever they were doing in order to prevent warnings and
     * errors because communication to the node no longer work, e.g. stop setting
     * parameters or commands.
     *
     * If any threads, call_every, or timeouts are running, they should be stopped now.
     */
    virtual void disable() = 0;

    // Non-copyable
    PluginImplBase(const PluginImplBase&) = delete;
    const PluginImplBase& operator=(const PluginImplBase&) = delete;

protected:
    std::shared_ptr<NodeImpl> _parent;
};

} // namespace mavsdk
