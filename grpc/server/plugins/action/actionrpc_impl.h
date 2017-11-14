#include "action.h"
#include "action.grpc.pb.h"
#include <google/protobuf/empty.pb.h>

using grpc::Status;
using grpc::ServerContext;
using dronecorerpc::ActionRPC;
using dronecorerpc::Empty;

using namespace dronecore;

class ActionRPCImpl final : public ActionRPC::Service
{
public:
    ActionRPCImpl(DroneCore *dc_obj)
    {
        dc = dc_obj;
    }

    Status Arm(ServerContext *context, const ::google::protobuf::Empty *request,
               dronecorerpc::ActionResult *response) override
    {
        const Action::Result action_result = dc->device().action().arm();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status TakeOff(ServerContext *context, const ::google::protobuf::Empty *request,
                   dronecorerpc::ActionResult *response) override
    {
        const Action::Result action_result = dc->device().action().takeoff();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status Land(ServerContext *context, const ::google::protobuf::Empty *request,
                dronecorerpc::ActionResult *response) override
    {
        const Action::Result action_result = dc->device().action().land();
        response->set_result(static_cast<dronecorerpc::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

private:
    DroneCore *dc;
};
