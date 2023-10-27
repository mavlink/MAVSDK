#pragma once

#include <memory>
#include <mutex>

#include <mavsdk.h>

namespace mavsdk::mavsdk_server {

template<typename ServerPlugin> class LazyServerPlugin {
public:
    explicit LazyServerPlugin(Mavsdk& mavsdk) : _mavsdk(mavsdk) {}

    ServerPlugin* maybe_plugin()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_server_plugin == nullptr) {
            _server_plugin = std::make_unique<ServerPlugin>(
                _mavsdk.server_component_by_type(Mavsdk::ComponentType::CompanionComputer));
        }
        return _server_plugin.get();
    }

private:
    Mavsdk& _mavsdk;
    std::unique_ptr<ServerPlugin> _server_plugin{};
    std::mutex _mutex{};
};

} // namespace mavsdk::mavsdk_server
