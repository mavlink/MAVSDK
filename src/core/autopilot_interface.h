#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include "node.h"
#include "interface_base.h"

namespace mavsdk {

class NodeImpl;
class AutopilotInterfaceImpl;
class MavsdkImpl;
class InfoImpl;
class CalibrationImpl;
class ActionImpl;

/**
 * @brief This represents an interface to an autopilot.
 *
 */
class AutopilotInterface : public InterfaceBase {
public:
    /** @private Constructor, used internally
     *
     * This constructor is not (and should not be) directly called by application code.
     *
     * @param parent `Node` Node interface dependency.
     */
    explicit AutopilotInterface(Node& parent);

    /**
     * @brief Destructor.
     */
    ~AutopilotInterface();

    /**
     * @brief Get the UUID of the autopilot.
     *
     * @return UUID of the autopilot.
     */
    uint64_t get_uuid() const;

private:
    std::shared_ptr<AutopilotInterfaceImpl> impl() const { return _impl; };
    
    /*
     * MavsdkImpl and PluginImplBase need access to AutopilotImpl class.
     */
    friend MavsdkImpl;
    // TODO this is really horrible, is there any other way? I couldn't think of one
    friend InfoImpl;
    friend CalibrationImpl;
    friend ActionImpl;

    Node& _parent;

    std::shared_ptr<AutopilotInterfaceImpl> _impl;
};

} // namespace mavsdk
