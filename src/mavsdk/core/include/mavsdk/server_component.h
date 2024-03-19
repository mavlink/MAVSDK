#pragma once

#include <cstdint>
#include <memory>

namespace mavsdk {

class MavsdkImpl;
class ServerComponentImpl;
class ServerPluginImplBase;

/**
 * @brief This class represents a component, used to initialize a server plugin.
 */
class ServerComponent {
public:
    /**
     * @private Constructor, used internally
     *
     * This constructor is not (and should not be) directly called by application code.
     */
    ServerComponent(MavsdkImpl& mavsdk_impl, uint8_t component_id);

    /**
     * @brief Destructor.
     */
    ~ServerComponent() = default;

    /**
     * @brief MAVLink component ID of this component
     */
    uint8_t component_id() const;

    /**
     * @brief Set system status of this MAVLink entity.
     *
     * The default system status is MAV_STATE_UNINIT.
     *
     * @param system_status system status.
     */
    void set_system_status(uint8_t system_status);

private:
    std::shared_ptr<ServerComponentImpl> _impl;

    friend MavsdkImpl;
    friend ServerPluginImplBase;
};

} // namespace mavsdk
