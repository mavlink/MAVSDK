#pragma once

#include <memory>
#include <typeinfo>

#include <mavsdk.h>
#include "log.h"

namespace mavsdk {
namespace mavsdk_server {

template<typename Plugin> class LazyPlugin {
public:
    explicit LazyPlugin(Mavsdk& mavsdk) : _mavsdk(mavsdk) {}

    Plugin* maybe_plugin()
    {
        bool created = false;
        if (_action == nullptr) {
            if (_mavsdk.systems().size() == 0) {
                LogWarn() << "No system yet (" << typeid(Plugin).name() << ")";
                return nullptr;
            }
            LogDebug() << "Create plugin (" << typeid(Plugin).name() << ")";
            _action = std::make_unique<Plugin>(_mavsdk.systems()[0]);
            LogDebug() << "Created plugin (" << typeid(Plugin).name() << ")";
            created = true;
        }
        if (created) {
            LogDebug() << "Use created plugin (" << typeid(Plugin).name() << ")";
        } else {
            LogDebug() << "Use existing plugin (" << typeid(Plugin).name() << ")";
        }
        return _action.get();
    }

private:
    Mavsdk& _mavsdk;
    std::unique_ptr<Plugin> _action{};
};

} // namespace mavsdk_server
} // namespace mavsdk
