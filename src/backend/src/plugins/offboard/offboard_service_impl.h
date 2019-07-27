#include "offboard/offboard.grpc.pb.h"
#include "plugins/offboard/offboard.h"

namespace mavsdk {
namespace backend {

template<typename Offboard = Offboard>
class OffboardServiceImpl final : public rpc::offboard::OffboardService::Service {
public:
    OffboardServiceImpl(Offboard& offboard) : _offboard(offboard) {}

    template<typename ResponseType>
    void
    fillResponseWithResult(ResponseType* response, mavsdk::Offboard::Result& offboard_result) const
    {
        auto rpc_result = static_cast<rpc::offboard::OffboardResult::Result>(offboard_result);

        auto* rpc_offboard_result = new rpc::offboard::OffboardResult();
        rpc_offboard_result->set_result(rpc_result);
        rpc_offboard_result->set_result_str(mavsdk::Offboard::result_str(offboard_result));

        response->set_allocated_offboard_result(rpc_offboard_result);
    }

    grpc::Status Start(
        grpc::ServerContext* /* context */,
        const rpc::offboard::StartRequest* /* request */,
        rpc::offboard::StartResponse* response) override
    {
        auto offboard_result = _offboard.start();

        if (response != nullptr) {
            fillResponseWithResult(response, offboard_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status Stop(
        grpc::ServerContext* /* context */,
        const rpc::offboard::StopRequest* /* request */,
        rpc::offboard::StopResponse* response) override
    {
        auto offboard_result = _offboard.stop();

        if (response != nullptr) {
            fillResponseWithResult(response, offboard_result);
        }

        return grpc::Status::OK;
    }

    grpc::Status IsActive(
        grpc::ServerContext* /* context */,
        const rpc::offboard::IsActiveRequest* /* request */,
        rpc::offboard::IsActiveResponse* response) override
    {
        if (response != nullptr) {
            auto is_active = _offboard.is_active();
            response->set_is_active(is_active);
        }

        return grpc::Status::OK;
    }

    grpc::Status SetActuatorControl(
        grpc::ServerContext* /* context */,
        const rpc::offboard::SetActuatorControlRequest* request,
        rpc::offboard::SetActuatorControlResponse* /* response */) override
    {
        if (request != nullptr) {
            auto requested_actuator_control =
                translateRPCActuatorControl(request->actuator_control());
            _offboard.set_actuator_control(requested_actuator_control);
        }

        return grpc::Status::OK;
    }

    static mavsdk::Offboard::ActuatorControl
    translateRPCActuatorControl(const rpc::offboard::ActuatorControl& rpc_actuator_control)
    {
        mavsdk::Offboard::ActuatorControl actuator_control{std::numeric_limits<float>::quiet_NaN()};

        int num_groups = std::min(2, rpc_actuator_control.groups_size());

        for (int i = 0; i < num_groups; i++) {
            int num_controls = std::min(8, rpc_actuator_control.groups(i).controls_size());
            for (int j = 0; j < num_controls; j++) {
                // https://developers.google.com/protocol-buffers/docs/reference/cpp/google.protobuf.repeated_field#
                actuator_control.groups[i].controls[j] = rpc_actuator_control.groups(i).controls(j);
            }
        }

        return actuator_control;
    }

    grpc::Status SetAttitude(
        grpc::ServerContext* /* context */,
        const rpc::offboard::SetAttitudeRequest* request,
        rpc::offboard::SetAttitudeResponse* /* response */) override
    {
        if (request != nullptr) {
            auto requested_attitude = translateRPCAttitude(request->attitude());
            _offboard.set_attitude(requested_attitude);
        }

        return grpc::Status::OK;
    }

    static mavsdk::Offboard::Attitude
    translateRPCAttitude(const rpc::offboard::Attitude& rpc_attitude)
    {
        mavsdk::Offboard::Attitude attitude;

        attitude.roll_deg = rpc_attitude.roll_deg();
        attitude.pitch_deg = rpc_attitude.pitch_deg();
        attitude.yaw_deg = rpc_attitude.yaw_deg();
        attitude.thrust_value = rpc_attitude.thrust_value();

        return attitude;
    }

    grpc::Status SetAttitudeRate(
        grpc::ServerContext* /* context */,
        const rpc::offboard::SetAttitudeRateRequest* request,
        rpc::offboard::SetAttitudeRateResponse* /* response */) override
    {
        if (request != nullptr) {
            auto requested_attitude_rate = translateRPCAttitudeRate(request->attitude_rate());
            _offboard.set_attitude_rate(requested_attitude_rate);
        }

        return grpc::Status::OK;
    }

    static mavsdk::Offboard::AttitudeRate
    translateRPCAttitudeRate(const rpc::offboard::AttitudeRate& rpc_attitude_rate)
    {
        mavsdk::Offboard::AttitudeRate attitude_rate;

        attitude_rate.roll_deg_s = rpc_attitude_rate.roll_deg_s();
        attitude_rate.pitch_deg_s = rpc_attitude_rate.pitch_deg_s();
        attitude_rate.yaw_deg_s = rpc_attitude_rate.yaw_deg_s();
        attitude_rate.thrust_value = rpc_attitude_rate.thrust_value();

        return attitude_rate;
    }

    grpc::Status SetPositionNed(
        grpc::ServerContext* /* context */,
        const rpc::offboard::SetPositionNedRequest* request,
        rpc::offboard::SetPositionNedResponse* /* response */) override
    {
        if (request != nullptr) {
            auto requested_position_ned_yaw =
                translateRPCPositionNedYaw(request->position_ned_yaw());
            _offboard.set_position_ned(requested_position_ned_yaw);
        }

        return grpc::Status::OK;
    }

    static mavsdk::Offboard::PositionNEDYaw
    translateRPCPositionNedYaw(const rpc::offboard::PositionNedYaw& rpc_position_ned_yaw)
    {
        mavsdk::Offboard::PositionNEDYaw position_ned_yaw;

        position_ned_yaw.north_m = rpc_position_ned_yaw.north_m();
        position_ned_yaw.east_m = rpc_position_ned_yaw.east_m();
        position_ned_yaw.down_m = rpc_position_ned_yaw.down_m();
        position_ned_yaw.yaw_deg = rpc_position_ned_yaw.yaw_deg();

        return position_ned_yaw;
    }

    grpc::Status SetVelocityBody(
        grpc::ServerContext* /* context */,
        const rpc::offboard::SetVelocityBodyRequest* request,
        rpc::offboard::SetVelocityBodyResponse* /* response */) override
    {
        if (request != nullptr) {
            auto requested_velocity_body_yawspeed =
                translateRPCVelocityBodyYawspeed(request->velocity_body_yawspeed());
            _offboard.set_velocity_body(requested_velocity_body_yawspeed);
        }

        return grpc::Status::OK;
    }

    static mavsdk::Offboard::VelocityBodyYawspeed translateRPCVelocityBodyYawspeed(
        const rpc::offboard::VelocityBodyYawspeed& rpc_velocity_body_yawspeed)
    {
        mavsdk::Offboard::VelocityBodyYawspeed velocity_body_yawspeed;

        velocity_body_yawspeed.forward_m_s = rpc_velocity_body_yawspeed.forward_m_s();
        velocity_body_yawspeed.right_m_s = rpc_velocity_body_yawspeed.right_m_s();
        velocity_body_yawspeed.down_m_s = rpc_velocity_body_yawspeed.down_m_s();
        velocity_body_yawspeed.yawspeed_deg_s = rpc_velocity_body_yawspeed.yawspeed_deg_s();

        return velocity_body_yawspeed;
    }

    grpc::Status SetVelocityNed(
        grpc::ServerContext* /* context */,
        const rpc::offboard::SetVelocityNedRequest* request,
        rpc::offboard::SetVelocityNedResponse* /* response */) override
    {
        if (request != nullptr) {
            auto requested_velocity_ned_yaw =
                translateRPCVelocityNedYaw(request->velocity_ned_yaw());
            _offboard.set_velocity_ned(requested_velocity_ned_yaw);
        }

        return grpc::Status::OK;
    }

    static mavsdk::Offboard::VelocityNEDYaw
    translateRPCVelocityNedYaw(const rpc::offboard::VelocityNedYaw& rpc_velocity_ned_yaw)
    {
        mavsdk::Offboard::VelocityNEDYaw velocity_ned_yaw;

        velocity_ned_yaw.north_m_s = rpc_velocity_ned_yaw.north_m_s();
        velocity_ned_yaw.east_m_s = rpc_velocity_ned_yaw.east_m_s();
        velocity_ned_yaw.down_m_s = rpc_velocity_ned_yaw.down_m_s();
        velocity_ned_yaw.yaw_deg = rpc_velocity_ned_yaw.yaw_deg();

        return velocity_ned_yaw;
    }

private:
    Offboard& _offboard;
};

} // namespace backend
} // namespace mavsdk
