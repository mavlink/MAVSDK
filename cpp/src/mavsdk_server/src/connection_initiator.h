#pragma once

#include <future>
#include <mutex>
#include <string>

#include "connection_result.h"
#include "log.h"

namespace mavsdk {
namespace mavsdk_server {

template<typename Mavsdk> class ConnectionInitiator {
public:
    ConnectionInitiator() {}
    ~ConnectionInitiator() {}

    bool start(Mavsdk& mavsdk, const std::string& connection_url)
    {
        LogInfo() << "Waiting to discover system on " << connection_url << "...";
        _discovery_future = wrapped_subscribe_on_new_system(mavsdk);

        if (!add_any_connection(mavsdk, connection_url)) {
            return false;
        }

        return true;
    }

    bool wait() { return _discovery_future.get(); }

    void cancel()
    {
        std::lock_guard<std::mutex> guard(_mutex);
        if (!_is_discovery_finished) {
            _is_discovery_finished = true;
            _discovery_promise->set_value(false);
        }
    }

private:
    bool add_any_connection(Mavsdk& mavsdk, const std::string& connection_url)
    {
        mavsdk::ConnectionResult connection_result = mavsdk.add_any_connection(connection_url);

        if (connection_result != ConnectionResult::Success) {
            LogErr() << "Connection failed: " << connection_result;
            return false;
        }

        return true;
    }

    std::future<bool> wrapped_subscribe_on_new_system(Mavsdk& mavsdk)
    {
        auto future = _discovery_promise->get_future();

        mavsdk.subscribe_on_new_system([this, &mavsdk]() {
            std::lock_guard<std::mutex> guard(_mutex);
            for (auto system : mavsdk.systems()) {
                if (!_is_discovery_finished && system->has_autopilot() && system->is_connected()) {
                    LogInfo() << "System discovered";

                    _is_discovery_finished = true;
                    _discovery_promise->set_value(true);
                    break;
                }
            }
        });

        return future;
    }

    std::mutex _mutex;
    std::atomic<bool> _is_discovery_finished = false;
    std::shared_ptr<std::promise<bool>> _discovery_promise = std::make_shared<std::promise<bool>>();
    std::future<bool> _discovery_future{};
};

} // namespace mavsdk_server
} // namespace mavsdk
