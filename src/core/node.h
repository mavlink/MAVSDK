#pragma once

#include <memory>
#include <functional>

namespace mavsdk {

class NodeImpl;
class MavsdkImpl;
class PluginImplBase;
class AutopilotNodeImpl;

/**
 * @brief This class represents a MAVLink node, which is EXACTLY 1 component on one system.
 * In other words, this connects to anything which emits its own MAVLink heartbeat.
 *
 * Node objects are used to interact with different MAVLink enabled devices/software in
 * a MAVLink network. They are not created directly by application code, but are returned
 * by the Mavsdk class.
 */
class Node {
public:
    /** @private Constructor, used internally
     *
     * This constructor is not (and should not be) directly called by application code.
     *
     * @param parent `MavsdkImpl` dependency.
     * @param system_id System id.
     * @param comp_id Component id.
     */
    explicit Node(MavsdkImpl& parent, uint8_t system_id, uint8_t comp_id);
    /**
     * @brief Destructor.
     */
    virtual ~Node();

    /**
     * @brief Checks if the node is connected.
     *
     * A node is connected if MAVSDK is actively receiving heartbeats from that node.
     * @return `true` if the node is connected.
     */
    bool is_connected() const;

    /**
     * @brief Get the System id of the node.
     *
     * @return System id of node.
     */
    uint8_t get_system_id() const;

    /**
     * @brief Get the Component id of the node.
     *
     * @return Component id of node.
     */
    uint8_t get_component_id() const;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Node& operator=(const Node&) = delete;

private:
    std::shared_ptr<NodeImpl> node_impl() { return _node_impl; };

    /**
     * MavsdkImpl and PluginImplBase need access to NodeImpl class.
     * This is not pretty but it's not easy to hide the methods from library
     * users if not like that (or with an ugly reinterpret_cast).
     */
    friend MavsdkImpl;
    friend PluginImplBase;
    friend AutopilotNodeImpl;

    std::shared_ptr<NodeImpl> _node_impl;
};

} // namespace mavsdk
