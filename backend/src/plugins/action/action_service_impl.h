#include "action/action.h"
#include "action/action.grpc.pb.h"

namespace dronecore {
namespace backend {

template <typename Action = Action>
class ActionServiceImpl final : public rpc::action::ActionService::Service
{
public:
    ActionServiceImpl(Action &action)
        : _action(action) {}

    grpc::Status Arm(grpc::ServerContext * /* context */,
                     const rpc::action::ArmRequest * /* request */,
                     rpc::action::ArmResponse *response) override
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
        auto rpc_result = static_cast<rpc::action::ActionResult::Result>(action_result);

        auto *rpc_action_result = new rpc::action::ActionResult();
        rpc_action_result->set_result(rpc_result);
        rpc_action_result->set_result_str(action_result_str(action_result));

        response->set_allocated_action_result(rpc_action_result);
    }

    grpc::Status Disarm(grpc::ServerContext * /* context */,
                        const rpc::action::DisarmRequest * /* request */,
                        rpc::action::DisarmResponse *response) override
    {
        auto action_result = _action.disarm();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status Takeoff(grpc::ServerContext * /* context */,
                         const rpc::action::TakeoffRequest * /* request */,
                         rpc::action::TakeoffResponse *response) override
    {
        auto action_result = _action.takeoff();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status Land(grpc::ServerContext * /* context */,
                      const rpc::action::LandRequest * /* request */,
                      rpc::action::LandResponse *response) override
    {
        auto action_result = _action.land();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status Kill(grpc::ServerContext * /* context */,
                      const rpc::action::KillRequest * /* request */,
                      rpc::action::KillResponse *response) override
    {
        auto action_result = _action.kill();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status ReturnToLaunch(grpc::ServerContext * /* context */,
                                const rpc::action::ReturnToLaunchRequest * /* request */,
                                rpc::action::ReturnToLaunchResponse *response) override
    {
        auto action_result = _action.return_to_launch();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status TransitionToFixedWing(grpc::ServerContext * /* context */,
                                       const rpc::action::TransitionToFixedWingRequest * /* request */,
                                       rpc::action::TransitionToFixedWingResponse *response) override
    {
        auto action_result = _action.transition_to_fixedwing();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status TransitionToMulticopter(grpc::ServerContext * /* context */,
                                         const rpc::action::TransitionToMulticopterRequest * /* request */,
                                         rpc::action::TransitionToMulticopterResponse *response) override
    {
        auto action_result = _action.transition_to_multicopter();

        if (response != nullptr) {
            fillResponseWithResult(response, action_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status GetTakeoffAltitude(grpc::ServerContext * /* context */,
                                    const rpc::action::GetTakeoffAltitudeRequest * /* request */,
                                    rpc::action::GetTakeoffAltitudeResponse *response) override
    {
        if (response != nullptr) {
            auto takeoff_altitude = _action.get_takeoff_altitude_m();
            response->set_altitude_m(takeoff_altitude);
        }

        return grpc::Status::OK;
    }

    grpc::Status SetTakeoffAltitude(grpc::ServerContext * /* context */,
                                    const rpc::action::SetTakeoffAltitudeRequest *request,
                                    rpc::action::SetTakeoffAltitudeResponse * /* response */) override
    {
        if (request != nullptr) {
            const auto requested_altitude = request->altitude_m();
            _action.set_takeoff_altitude(requested_altitude);
        }

        return grpc::Status::OK;
    }

    grpc::Status GetMaximumSpeed(grpc::ServerContext * /* context */,
                                 const rpc::action::GetMaximumSpeedRequest * /* request */,
                                 rpc::action::GetMaximumSpeedResponse *response) override
    {
        if (response != nullptr) {
            auto max_speed = _action.get_max_speed_m_s();
            response->set_speed_m_s(max_speed);
        }

        return grpc::Status::OK;
    }

    grpc::Status SetMaximumSpeed(grpc::ServerContext * /* context */,
                                 const rpc::action::SetMaximumSpeedRequest *request,
                                 rpc::action::SetMaximumSpeedResponse * /* response */) override
    {
        if (request != nullptr) {
            const auto requested_speed = request->speed_m_s();
            _action.set_max_speed(requested_speed);
        }

        return grpc::Status::OK;
    }

private:
    Action &_action;
};

} // namespace backend
} // namespace dronecore
