#include "core/core.grpc.pb.h"

using grpc::Status;
using grpc::ServerContext;
using grpc::ServerWriter;

using namespace dronecore;

class CoreServiceImpl final: public rpc::core::CoreService::Service
{
public:
    CoreServiceImpl(DroneCore &dc)
        : dc(dc) {}

    Status SubscribeDevices(ServerContext *context,
                            const rpc::core::SubscribeDevicesRequest *request,
                            ServerWriter<rpc::core::Device> *writer) override
    {
        std::vector<uint64_t> list = dc.device_uuids();

        for (auto uuid : dc.device_uuids()) {
            auto *rpc_uuid = new rpc::core::UUID();
            rpc_uuid->set_value(uuid);

            rpc::core::Device device;
            device.set_allocated_uuid(rpc_uuid);

            writer->Write(device);
        }

        return Status::OK;
    }

private:
    DroneCore &dc;
};
