#pragma once

#include <future>
#include <string>

#include "connection_result.h"
#include "log.h"

namespace mavsdk {
namespace backend {

template<typename Mavsdk> class ConnectionInitiator {
public:
    ConnectionInitiator() {}
    ~ConnectionInitiator() {}

    bool start(Mavsdk& mavsdk, const std::string& connection_url)
    {
        init_mutex();

        LogInfo() << "Waiting to discover system on " << connection_url << "...";
        _discovery_future = wrapped_subscribe_on_change(mavsdk);

        if (!add_any_connection(mavsdk, connection_url)) {
            return false;
        }

        return true;
    }

    void wait() { _discovery_future.wait(); }

private:
    void init_mutex() { _discovery_promise = std::make_shared<std::promise<void>>(); }

    bool add_any_connection(Mavsdk& mavsdk, const std::string& connection_url)
    {
        mavsdk::ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);

        if (connection_result != ConnectionResult::Success) {
            LogErr() << "Connection failed: " << connection_result;
            return false;
        }

        return true;
    }

    std::future<void> wrapped_subscribe_on_change(Mavsdk& mavsdk)
    {
        auto future = _discovery_promise->get_future();

        mavsdk.subscribe_on_change([this, &mavsdk]() {
            const auto system = mavsdk.systems().at(0);

            if (system->is_connected()) {
                std::call_once(_discovery_flag, [this]() {
                    LogInfo() << "System discovered";
                    _discovery_promise->set_value();
                });
            } else {
                LogInfo() << "System timed out";
            }
        });

        return future;
    }

    std::once_flag _discovery_flag{};
    std::shared_ptr<std::promise<void>> _discovery_promise{};
    std::future<void> _discovery_future{};
};

} // namespace backend
} // namespace mavsdk
