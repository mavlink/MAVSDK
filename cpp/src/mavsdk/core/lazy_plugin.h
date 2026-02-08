#pragma once

#include <memory>
#include <mutex>

#include <mavsdk.h>

namespace mavsdk::mavsdk_server {

template<typename Plugin> class LazyPlugin {
public:
    explicit LazyPlugin(Mavsdk& mavsdk) : _mavsdk(mavsdk) {}

    Plugin* maybe_plugin()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (_plugin == nullptr) {
            if (_mavsdk.systems().empty()) {
                return nullptr;
            }
            _plugin = std::make_unique<Plugin>(_mavsdk.first_autopilot(0).value());
        }
        return _plugin.get();
    }

private:
    Mavsdk& _mavsdk;
    std::unique_ptr<Plugin> _plugin{};
    std::mutex _mutex{};
};

} // namespace mavsdk::mavsdk_server
