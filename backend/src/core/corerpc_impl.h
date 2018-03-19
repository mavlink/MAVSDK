#include "core/core.grpc.pb.h"
#include "dronecore.h"

using grpc::Status;
using grpc::ServerContext;
using grpc::ServerWriter;

using namespace dronecore;

class CoreServiceImpl final: public rpc::core::CoreService::Service
{
public:
    CoreServiceImpl(DroneCore &dc)
        : dc(dc) {}

    Status SubscribeSystems(ServerContext *context,
                            const rpc::core::SubscribeSystemsRequest *request,
                            ServerWriter<rpc::core::System> *writer) override
    {
        std::vector<uint64_t> list = dc.system_uuids();

        for (auto uuid : dc.system_uuids()) {
            auto *rpc_uuid = new rpc::core::UUID();
            rpc_uuid->set_value(uuid);

            rpc::core::System system;
            system.set_allocated_uuid(rpc_uuid);

            writer->Write(system);
        }

        return Status::OK;
    }

private:
    DroneCore &dc;
};
