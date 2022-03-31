#pragma once

#include <memory>

#include <mavsdk.h>

namespace mavsdk::mavsdk_server {

template<typename Plugin> class LazyPlugin {
public:
    explicit LazyPlugin(Mavsdk& mavsdk) : _mavsdk(mavsdk) {}

    Plugin* maybe_plugin()
    {
        if (_plugin == nullptr) {
            if (_mavsdk.systems().empty()) {
                return nullptr;
            }
            _plugin = std::make_unique<Plugin>(_mavsdk.systems()[0]);
        }
        return _plugin.get();
    }

private:
    Mavsdk& _mavsdk;
    std::unique_ptr<Plugin> _plugin{};
};

} // namespace mavsdk::mavsdk_server
