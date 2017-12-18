#include "dronecore.grpc.pb.h"
#include <google/protobuf/empty.pb.h>

using grpc::Status;
using grpc::ServerContext;
using grpc::ServerWriter;
using dronecorerpc::DroneCoreRPC;
using dronecorerpc::Active_UUID_List;

using namespace dronecore;

class DroneCoreRPCImpl final: public DroneCoreRPC::Service
{
public:
    DroneCoreRPCImpl(DroneCore *dc_obj)
    {
        dc = dc_obj;
    }

    Status Get_UUID_List(ServerContext *context,
                         const ::google::protobuf::Empty *request,
                         Active_UUID_List *response) override
    {
        std::vector<uint64_t> list;
        list = dc->device_uuids();
        for (auto it = list.begin(); it != list.end(); ++it) {
            UUID *item = response->add_uuid_list();
            item->set_uuid(*it);
        }
        return Status::OK;
    }

private:
    DroneCore *dc;
};
