#pragma once

#include <cstdint>
#include <memory>

namespace mavsdk {

class MavsdkImpl;
class ServerComponentImpl;
class ServerPluginImplBase;

class ServerComponent {
public:
    ServerComponent(MavsdkImpl& mavsdk_impl, uint8_t component_id);
    ~ServerComponent() = default;

private:
    std::shared_ptr<ServerComponentImpl> _impl;

    friend MavsdkImpl;
    friend ServerPluginImplBase;
};

} // namespace mavsdk
