#include "action/action.h"
#include "action/action.grpc.pb.h"

class ActionServiceImpl final : public dronecore::rpc::action::ActionService::Service
{
public:
    ActionServiceImpl(const dronecore::Action &action)
        : action(action) {}

    grpc::Status Arm(grpc::ServerContext *context,
                     const dronecore::rpc::action::ArmRequest *request,
                     dronecore::rpc::action::ArmResponse *response) override;

    grpc::Status Takeoff(grpc::ServerContext *context,
                         const dronecore::rpc::action::TakeoffRequest *request,
                         dronecore::rpc::action::TakeoffResponse *response) override;

    grpc::Status Land(grpc::ServerContext *context,
                      const dronecore::rpc::action::LandRequest *request,
                      dronecore::rpc::action::LandResponse *response) override;

private:
    const dronecore::Action &action;
};
