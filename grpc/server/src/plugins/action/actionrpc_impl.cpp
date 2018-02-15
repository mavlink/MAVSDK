#include "actionrpc_impl.h"

namespace rpc_action = dronecore::rpc::action;

grpc::Status ActionServiceImpl::Arm(grpc::ServerContext *context,
                                    const rpc_action::ArmRequest *request,
                                    rpc_action::ArmResponse *response)
{
    const auto action_result = action.arm();

    auto *rpc_action_result = new rpc_action::ActionResult();
    rpc_action_result->set_result(static_cast<rpc_action::ActionResult::Result>(action_result));
    rpc_action_result->set_result_str(dronecore::Action::result_str(action_result));

    response->set_allocated_action_result(rpc_action_result);

    return grpc::Status::OK;
}

grpc::Status ActionServiceImpl::Takeoff(grpc::ServerContext *context,
                                        const rpc_action::TakeoffRequest *request,
                                        rpc_action::TakeoffResponse *response)
{
    const auto action_result = action.takeoff();

    auto *rpc_action_result = new rpc_action::ActionResult();
    rpc_action_result->set_result(static_cast<rpc_action::ActionResult::Result>(action_result));
    rpc_action_result->set_result_str(dronecore::Action::result_str(action_result));

    response->set_allocated_action_result(rpc_action_result);

    return grpc::Status::OK;
}

grpc::Status ActionServiceImpl::Land(grpc::ServerContext *context,
                                     const rpc_action::LandRequest *request,
                                     rpc_action::LandResponse *response)
{
    const auto action_result = action.land();

    auto *rpc_action_result = new rpc_action::ActionResult();
    rpc_action_result->set_result(static_cast<rpc_action::ActionResult::Result>(action_result));
    rpc_action_result->set_result_str(dronecore::Action::result_str(action_result));

    response->set_allocated_action_result(rpc_action_result);

    return grpc::Status::OK;
}
