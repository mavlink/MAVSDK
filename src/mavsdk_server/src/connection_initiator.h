#pragma once

#include <chrono>
#include <future>
#include <mutex>
#include <thread>
#include <string>

#include "connection_result.h"
#include "log.h"

namespace mavsdk {
namespace mavsdk_server {

template<typename Mavsdk> class ConnectionInitiator {
public:
    ConnectionInitiator() {}
    ~ConnectionInitiator() {}

    bool connect(Mavsdk& mavsdk, const std::string& connection_url)
    {
        LogInfo() << "Waiting to discover system on " << connection_url << "...";

        if (!add_any_connection(mavsdk, connection_url)) {
            return false;
        }

        // Instead of using the subscribe_on_new_system callback, we just use a
        // while loop here. That's because the subscribe_on_new_system callback
        // won't tell us if an autopilot is discovered when another component
        // of the same system is discovered first, thus triggering the callback
        // early when not autopilot is around.
        // With a stupid old while loop we can just keep checking this until
        // we have an autopilot and then exit.
        while (!_should_exit) {
            for (const auto& system : mavsdk.systems()) {
                if (system->has_autopilot()) {
                    LogInfo() << "System discovered";
                    return true;
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        return false;
    }

    void cancel() { _should_exit = true; }

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

    std::atomic<bool> _should_exit{false};
};

} // namespace mavsdk_server
} // namespace mavsdk
