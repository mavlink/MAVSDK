#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include "node.h"

namespace mavsdk {

class NodeImpl;
class AutopilotNodeImpl;
class MavsdkImpl;
class PluginImplBase;

/**
 * @brief This represents a connection to an Autopilot Node.
 *
 * It allows usage of all Node methods, as well as functionality
 * specific to controlling autopilots.
 */
class AutopilotNode : public Node {
public:
    /**
     * @brief Get the UUID of the autopilot.
     *
     * @return UUID of the autopilot.
     */
    virtual uint64_t get_uuid() const;

private:
    std::shared_ptr<AutopilotNodeImpl> autopilot_node_impl() const { return _autopilot_node_impl; };
    
    /*
     * MavsdkImpl and PluginImplBase need access to AutopilotImpl class.
     */
    friend MavsdkImpl;
    friend PluginImplBase;

    std::shared_ptr<AutopilotNodeImpl> _autopilot_node_impl;
};

} // namespace mavsdk
