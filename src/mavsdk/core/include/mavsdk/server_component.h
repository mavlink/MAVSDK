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

private:
    std::shared_ptr<ServerComponentImpl> _impl;

    friend MavsdkImpl;
    friend ServerPluginImplBase;
};

} // namespace mavsdk
