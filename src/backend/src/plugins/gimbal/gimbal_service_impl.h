#include "gimbal/gimbal.grpc.pb.h"
#include "plugins/gimbal/gimbal.h"

namespace mavsdk {
namespace backend {

template<typename Gimbal = Gimbal>
class GimbalServiceImpl final : public rpc::gimbal::GimbalService::Service {
public:
    GimbalServiceImpl(Gimbal& gimbal) : _gimbal(gimbal) {}

    grpc::Status SetPitchAndYaw(
        grpc::ServerContext* /*  context */,
        const rpc::gimbal::SetPitchAndYawRequest* request,
        rpc::gimbal::SetPitchAndYawResponse* response) override
    {
        if (request != nullptr) {
            const auto requested_gimbal_pitch = request->pitch_deg();
            const auto requested_gimbal_yaw = request->yaw_deg();

            const auto gimbal_result =
                _gimbal.set_pitch_and_yaw(requested_gimbal_pitch, requested_gimbal_yaw);

            if (response != nullptr) {
                auto* rpc_gimbal_result = new rpc::gimbal::GimbalResult();
                rpc_gimbal_result->set_result(
                    static_cast<rpc::gimbal::GimbalResult::Result>(gimbal_result));
                rpc_gimbal_result->set_result_str(mavsdk::Gimbal::result_str(gimbal_result));

                response->set_allocated_gimbal_result(rpc_gimbal_result);
            }
        }

        return grpc::Status::OK;
    }

    grpc::Status SetMode(
        grpc::ServerContext* /* context */,
        const rpc::gimbal::SetModeRequest* request,
        rpc::gimbal::SetModeResponse* response) override
    {
        if (request != nullptr) {
            const auto requested_gimbal_mode = request->gimbal_mode();

            const auto gimbal_result =
                _gimbal.set_gimbal_mode(translateRPCGimbalMode(requested_gimbal_mode));

            if (response != nullptr) {
                auto* rpc_gimbal_result = new rpc::gimbal::GimbalResult();
                rpc_gimbal_result->set_result(
                    static_cast<rpc::gimbal::GimbalResult::Result>(gimbal_result));
                rpc_gimbal_result->set_result_str(mavsdk::Gimbal::result_str(gimbal_result));

                response->set_allocated_gimbal_result(rpc_gimbal_result);
            }
        }

        return grpc::Status::OK;
    }

    static mavsdk::Gimbal::GimbalMode translateRPCGimbalMode(const rpc::gimbal::GimbalMode mode)
    {
        switch (mode) {
            case rpc::gimbal::GimbalMode::YAW_FOLLOW:
                return mavsdk::Gimbal::GimbalMode::YAW_FOLLOW;
            case rpc::gimbal::GimbalMode::YAW_LOCK:
                return mavsdk::Gimbal::GimbalMode::YAW_LOCK;
            default:
                return mavsdk::Gimbal::GimbalMode::YAW_FOLLOW;
        }
    }

private:
    Gimbal& _gimbal;
};

} // namespace backend
} // namespace mavsdk
