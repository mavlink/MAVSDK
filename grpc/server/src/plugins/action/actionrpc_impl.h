#include "action.h"
#include "action/action.grpc.pb.h"

using grpc::Status;
using grpc::ServerContext;

using namespace dronecore;

class ActionServiceImpl final : public rpc::action::ActionService::Service
{
public:
    ActionServiceImpl(DroneCore *dc_obj)
    {
        dc = dc_obj;
    }

    Status Arm(ServerContext *context, const rpc::action::ArmRequest *request,
               rpc::action::ActionResult *response) override
    {
        const Action::Result action_result = dc->device(request->uuid().value()).action().arm();
        response->set_result(static_cast<rpc::action::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status TakeOff(ServerContext *context, const rpc::action::TakeOffRequest *request,
                   rpc::action::ActionResult *response) override
    {
        const Action::Result action_result = dc->device(request->uuid().value()).action().takeoff();
        response->set_result(static_cast<rpc::action::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

    Status Land(ServerContext *context, const rpc::action::LandRequest *request,
                rpc::action::ActionResult *response) override
    {
        const Action::Result action_result = dc->device(request->uuid().value()).action().land();
        response->set_result(static_cast<rpc::action::ActionResult::Result>(action_result));
        response->set_result_str(Action::result_str(action_result));
        return Status::OK;
    }

private:
    DroneCore *dc;
};
