#pragma once

#include <memory>
#include <mutex>

#include <mavsdk.h>
#include <log.h>

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
            _plugin = std::make_unique<Plugin>(first_autopilot());
        }
        return _plugin.get();
    }

private:
    Mavsdk& _mavsdk;
    std::unique_ptr<Plugin> _plugin{};
    std::mutex _mutex{};

    std::shared_ptr<System> first_autopilot()
    {
        for (auto system : _mavsdk.systems()) {
            if (system->has_autopilot()) {
                return system;
            }
        }

        LogErr() << "No autopilot found!";
        return nullptr;
    }
};

} // namespace mavsdk::mavsdk_server
