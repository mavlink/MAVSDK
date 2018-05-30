#include <future>
#include <string>

#include "core/core.grpc.pb.h"
#include "dronecore.h"

namespace dronecore {
namespace backend {

template<typename DroneCore = DroneCore>
class CoreServiceImpl final : public dronecore::rpc::core::CoreService::Service {
public:
    CoreServiceImpl(DroneCore &dc) :
        _dc(dc),
        _stop_promise(std::promise<void>()),
        _stop_future(_stop_promise.get_future())
    {}

    grpc::Status SubscribeDiscover(grpc::ServerContext * /* context */,
                                   const rpc::core::SubscribeDiscoverRequest * /* request */,
                                   grpc::ServerWriter<rpc::core::DiscoverResponse> *writer) override
    {
        _dc.register_on_discover([&writer](const uint64_t uuid) {
            dronecore::rpc::core::DiscoverResponse rpc_discover_response;
            rpc_discover_response.set_uuid(uuid);
            writer->Write(rpc_discover_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeTimeout(grpc::ServerContext * /* context */,
                                  const rpc::core::SubscribeTimeoutRequest * /* request */,
                                  grpc::ServerWriter<rpc::core::TimeoutResponse> *writer) override
    {
        _dc.register_on_timeout([&writer](const uint64_t /* uuid */) {
            dronecore::rpc::core::TimeoutResponse rpc_timeout_response;
            writer->Write(rpc_timeout_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    // For now, the running plugins are hardcoded and we assume they are always started by the
    // backend.
    grpc::Status
    ListRunningPlugins(grpc::ServerContext * /* context */,
                       const rpc::core::ListRunningPluginsRequest * /* request */,
                       dronecore::rpc::core::ListRunningPluginsResponse *response) override
    {
        std::string plugin_names[3] = {"action", "mission", "telemetry"};

        for (const auto plugin_name : plugin_names) {
            auto plugin_info = response->add_plugin_info();
            plugin_info->set_name(plugin_name);
            plugin_info->set_address("localhost");
            plugin_info->set_port(50051);
        }

        return grpc::Status::OK;
    }

    void stop() { _stop_promise.set_value(); }

private:
    DroneCore &_dc;
    std::promise<void> _stop_promise;
    std::future<void> _stop_future;
};

} // namespace backend
} // namespace dronecore
