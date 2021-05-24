#pragma once

#include <memory>

#include <mavsdk.h>

namespace mavsdk {
namespace mavsdk_server {

template<typename Plugin> class LazyPlugin {
public:
    explicit LazyPlugin(Mavsdk& mavsdk) : _mavsdk(mavsdk) {}

    Plugin* maybe_plugin()
    {
        if (_action == nullptr) {
            if (_mavsdk.systems().size() == 0) {
                return nullptr;
            }
            _action = std::make_unique<Plugin>(_mavsdk.systems()[0]);
        }
        return _action.get();
    }

private:
    Mavsdk& _mavsdk;
    std::unique_ptr<Plugin> _action{};
};

} // namespace mavsdk_server
} // namespace mavsdk
