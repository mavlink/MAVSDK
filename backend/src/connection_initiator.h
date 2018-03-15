#include <future>

#include "connection_result.h"
#include "log.h"

namespace dronecore {
namespace backend {

template <typename DroneCore>
class ConnectionInitiator
{
public:
    ConnectionInitiator() {}
    ~ConnectionInitiator() {}

    bool start(DroneCore &dc, const int port)
    {
        init_mutex();
        init_timeout_logging(dc);

        _discovery_future = wrapped_register_on_discover(dc);

        if (!add_udp_connection(dc, port)) {
            return false;
        }

        return true;
    }

    void wait()
    {
        _discovery_future.wait();
    }

private:
    void init_mutex()
    {
        _discovery_promise = std::make_shared<std::promise<uint64_t>>();
    }

    void init_timeout_logging(DroneCore &dc) const
    {
        dc.register_on_timeout([](uint64_t uuid) {
            LogInfo() << "System timed out [UUID: " << uuid << "]";
        });
    }

    bool add_udp_connection(DroneCore &dc, const int port)
    {
        dronecore::ConnectionResult connection_result = dc.add_udp_connection(port);

        if (connection_result != ConnectionResult::SUCCESS) {
            LogErr() << "Connection failed: " << connection_result_str(connection_result);
            return false;
        }

        return true;
    }

    std::future<uint64_t> wrapped_register_on_discover(DroneCore &dc)
    {
        auto future = _discovery_promise->get_future();

        LogInfo() << "Waiting to discover system...";

        dc.register_on_discover([this](uint64_t uuid) {
            std::call_once(_discovery_flag, [this, uuid]() {
                LogInfo() << "System discovered [UUID: " << uuid << "]";
                _discovery_promise->set_value(uuid);
            });
        });

        return future;
    }

    std::once_flag _discovery_flag;
    std::shared_ptr<std::promise<uint64_t>> _discovery_promise;
    std::future<uint64_t> _discovery_future;
};

} // backend
} // dronecore
