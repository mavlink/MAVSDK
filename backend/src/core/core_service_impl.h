#include <string>

#include "core/core.grpc.pb.h"
#include "dronecore.h"

namespace dronecore {
namespace backend {

template <typename DroneCore = DroneCore>
class CoreServiceImpl final: public dronecore::rpc::core::CoreService::Service
{
public:
    CoreServiceImpl(DroneCore &dc)
        : _dc(dc) {}

    // For now, the running plugins are hardcoded and we assume they are always started by the backend.
    grpc::Status ListRunningPlugins(grpc::ServerContext* /* context */,
                                    const rpc::core::ListRunningPluginsRequest* /* request */,
                                    dronecore::rpc::core::ListRunningPluginsResponse *response) override
    {
        std::string plugin_names[3] = { "action", "mission", "telemetry" };

        for (const auto plugin_name : plugin_names) {
            auto plugin_info = response->add_plugin_info();
            plugin_info->set_name(plugin_name);
            plugin_info->set_address("localhost");
            plugin_info->set_port(50051);
        }

        return grpc::Status::OK;
    }

private:
    DroneCore &_dc;
};

} // namespace backend
} // namespace dronecore
