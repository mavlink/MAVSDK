#include "action/action.h"
#include "action/action.grpc.pb.h"

namespace dronecore {
namespace backend {

template <typename Action>
class ActionServiceImpl final : public dronecore::rpc::action::ActionService::Service
{
public:
    ActionServiceImpl(const Action &action)
        : _action(action) {}

    grpc::Status Arm(grpc::ServerContext * /* context */,
                     const dronecore::rpc::action::ArmRequest * /* request */,
                     dronecore::rpc::action::ArmResponse *response)
    {
        auto action_result = _action.arm();
        auto rpc_result = static_cast<dronecore::rpc::action::ActionResult::Result>(action_result);

        auto *rpc_action_result = new dronecore::rpc::action::ActionResult();
        rpc_action_result->set_result(rpc_result);
        rpc_action_result->set_result_str(dronecore::Action::result_str(action_result));

        response->set_allocated_action_result(rpc_action_result);

        return grpc::Status::OK;
    }

    grpc::Status Takeoff(grpc::ServerContext * /* context */,
                         const dronecore::rpc::action::TakeoffRequest * /* request */,
                         dronecore::rpc::action::TakeoffResponse *response)
    {
        auto action_result = _action.takeoff();
        auto rpc_result = static_cast<dronecore::rpc::action::ActionResult::Result>(action_result);

        auto *rpc_action_result = new dronecore::rpc::action::ActionResult();
        rpc_action_result->set_result(rpc_result);
        rpc_action_result->set_result_str(dronecore::Action::result_str(action_result));

        response->set_allocated_action_result(rpc_action_result);

        return grpc::Status::OK;
    }

    grpc::Status Land(grpc::ServerContext * /* context */,
                      const dronecore::rpc::action::LandRequest * /* request */,
                      dronecore::rpc::action::LandResponse *response)
    {
        auto action_result = _action.land();
        auto rpc_result = static_cast<dronecore::rpc::action::ActionResult::Result>(action_result);

        auto *rpc_action_result = new dronecore::rpc::action::ActionResult();
        rpc_action_result->set_result(rpc_result);
        rpc_action_result->set_result_str(dronecore::Action::result_str(action_result));

        response->set_allocated_action_result(rpc_action_result);

        return grpc::Status::OK;
    }
private:
    const Action &_action;
};

} // namespace backend
} // namespace dronecore
