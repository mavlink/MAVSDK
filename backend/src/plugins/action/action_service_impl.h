#include "action/action.h"
#include "action/action.grpc.pb.h"

namespace dronecore {
namespace backend {

template <typename Action = Action>
class ActionServiceImpl final : public dronecore::rpc::action::ActionService::Service
{
public:
    ActionServiceImpl(const Action &action)
        : _action(action) {}

    grpc::Status Arm(grpc::ServerContext * /* context */,
                     const dronecore::rpc::action::ArmRequest * /* request */,
                     dronecore::rpc::action::ArmResponse *response) override
    {
        auto action_result = _action.arm();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    template <typename ResponseType>
    void fillResponseWithResult(ResponseType *response, ActionResult &action_result) const
    {
        auto rpc_result = static_cast<dronecore::rpc::action::ActionResult::Result>(action_result);

        auto *rpc_action_result = new dronecore::rpc::action::ActionResult();
        rpc_action_result->set_result(rpc_result);
        rpc_action_result->set_result_str(dronecore::action_result_str(action_result));

        response->set_allocated_action_result(rpc_action_result);
    }

    grpc::Status Disarm(grpc::ServerContext * /* context */,
                        const dronecore::rpc::action::DisarmRequest * /* request */,
                        dronecore::rpc::action::DisarmResponse *response) override
    {
        auto action_result = _action.disarm();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status Takeoff(grpc::ServerContext * /* context */,
                         const dronecore::rpc::action::TakeoffRequest * /* request */,
                         dronecore::rpc::action::TakeoffResponse *response) override
    {
        auto action_result = _action.takeoff();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status Land(grpc::ServerContext * /* context */,
                      const dronecore::rpc::action::LandRequest * /* request */,
                      dronecore::rpc::action::LandResponse *response) override
    {
        auto action_result = _action.land();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status Kill(grpc::ServerContext * /* context */,
                      const dronecore::rpc::action::KillRequest * /* request */,
                      dronecore::rpc::action::KillResponse *response) override
    {
        auto action_result = _action.kill();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status ReturnToLaunch(grpc::ServerContext * /* context */,
                                const dronecore::rpc::action::ReturnToLaunchRequest * /* request */,
                                dronecore::rpc::action::ReturnToLaunchResponse *response) override
    {
        auto action_result = _action.return_to_launch();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status TransitionToFixedWings(grpc::ServerContext * /* context */,
                                        const dronecore::rpc::action::TransitionToFixedWingsRequest * /* request */,
                                        dronecore::rpc::action::TransitionToFixedWingsResponse *response) override
    {
        auto action_result = _action.transition_to_fixedwing();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status TransitionToMulticopter(grpc::ServerContext * /* context */,
                                         const dronecore::rpc::action::TransitionToMulticopterRequest * /* request */,
                                         dronecore::rpc::action::TransitionToMulticopterResponse *response) override
    {
        auto action_result = _action.transition_to_multicopter();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

private:
    const Action &_action;
};

} // namespace backend
} // namespace dronecore
