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

    bool start(Mavsdk& dc, const std::string& connection_url)
    {
        init_mutex();
        init_timeout_logging(dc);

        LogInfo() << "Waiting to discover system on " << connection_url << "...";
        _discovery_future = wrapped_register_on_discover(dc);

        if (!add_any_connection(dc, connection_url)) {
            return false;
        }

        return true;
    }

    void wait() { _discovery_future.wait(); }

private:
    void init_mutex() { _discovery_promise = std::make_shared<std::promise<uint64_t>>(); }

    void init_timeout_logging(Mavsdk& dc) const
    {
        dc.register_on_timeout(
            [](uint64_t uuid) { LogInfo() << "System timed out [UUID: " << uuid << "]"; });
    }

    bool add_any_connection(Mavsdk& dc, const std::string& connection_url)
    {
        mavsdk::ConnectionResult connection_result = dc.add_any_connection(connection_url);

        if (connection_result != ConnectionResult::SUCCESS) {
            LogErr() << "Connection failed: " << connection_result_str(connection_result);
            return false;
        }

        return true;
    }

    std::future<uint64_t> wrapped_register_on_discover(Mavsdk& dc)
    {
        auto future = _discovery_promise->get_future();

        dc.register_on_discover([this](uint64_t uuid) {
            std::call_once(_discovery_flag, [this, uuid]() {
                LogInfo() << "System discovered [UUID: " << uuid << "]";
                _discovery_promise->set_value(uuid);
            });
        });

        return future;
    }

    std::once_flag _discovery_flag{};
    std::shared_ptr<std::promise<uint64_t>> _discovery_promise{};
    std::future<uint64_t> _discovery_future{};
};

} // namespace backend
} // namespace mavsdk
