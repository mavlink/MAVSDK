#include <cmath>
#include <array>
#include <limits>
#include "mocap/mocap.grpc.pb.h"
#include "plugins/mocap/mocap.h"
#include "log.h"

namespace mavsdk {
namespace backend {

template<typename Mocap = Mocap>
class MocapServiceImpl final : public mavsdk::rpc::mocap::MocapService::Service {
public:
    MocapServiceImpl(Mocap& mocap) : _mocap(mocap) {}

    template<typename ResponseType>
    void fillResponseWithResult(ResponseType* response, mavsdk::Mocap::Result mocap_result) const
    {
        auto rpc_result = static_cast<rpc::mocap::MocapResult::Result>(mocap_result);

        auto* rpc_mocap_result = new rpc::mocap::MocapResult();
        rpc_mocap_result->set_result(rpc_result);
        rpc_mocap_result->set_result_str(mavsdk::Mocap::result_str(mocap_result));

        response->set_allocated_mocap_result(rpc_mocap_result);
    }

    typedef std::function<float(int)> get_val_t;
    template<size_t N = 21>
    bool fill_covariance(
        const mavsdk::rpc::mocap::Covariance& rpc_covariance, std::array<float, N>& covariance)
    {
        const int len_covariance = rpc_covariance.covariance_matrix_size();
        const bool first_cov_is_nan =
            (!len_covariance ? false : std::isnan(rpc_covariance.covariance_matrix(0)));
        if (!first_cov_is_nan && len_covariance != N) {
            return false;
        }
        if (!first_cov_is_nan) {
            for (int i = 0; i < N; i++) {
                covariance[i] = rpc_covariance.covariance_matrix(i);
            }
        } else {
            covariance[0] = std::numeric_limits<float>::quiet_NaN();
        }
        return true;
    }

    grpc::Status SetVisionPositionEstimate(
        grpc::ServerContext* /* context */,
        const rpc::mocap::SetVisionPositionEstimateRequest* rpc_vision_position_request,
        rpc::mocap::SetVisionPositionEstimateResponse* response) override
    {
        mavsdk::Mocap::VisionPositionEstimate position{};

        position.time_usec = rpc_vision_position_request->vision_position_estimate().time_usec();
        position.position_body.x_m =
            rpc_vision_position_request->vision_position_estimate().position_body().x_m();
        position.position_body.y_m =
            rpc_vision_position_request->vision_position_estimate().position_body().y_m();
        position.position_body.z_m =
            rpc_vision_position_request->vision_position_estimate().position_body().z_m();

        position.angle_body.roll_rad =
            rpc_vision_position_request->vision_position_estimate().angle_body().roll_rad();
        position.angle_body.pitch_rad =
            rpc_vision_position_request->vision_position_estimate().angle_body().pitch_rad();
        position.angle_body.yaw_rad =
            rpc_vision_position_request->vision_position_estimate().angle_body().yaw_rad();

        if (!fill_covariance(
                rpc_vision_position_request->vision_position_estimate().pose_covariance(),
                position.pose_covariance)) {
            fillResponseWithResult(response, mavsdk::Mocap::Result::INVALID_REQUEST_DATA);
            return grpc::Status::OK;
        }

        mavsdk::Mocap::Result mocap_result = _mocap.set_vision_position_estimate(position);
        fillResponseWithResult(response, mocap_result);

        return grpc::Status::OK;
    }

    grpc::Status SetAttitudePositionMocap(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::mocap::SetAttitudePositionMocapRequest* rpc_attitude_position_request,
        mavsdk::rpc::mocap::SetAttitudePositionMocapResponse* response) override
    {
        mavsdk::Mocap::AttitudePositionMocap attitude_position_mocap{};

        attitude_position_mocap.time_usec =
            rpc_attitude_position_request->attitude_position_mocap().time_usec();

        attitude_position_mocap.q.w =
            rpc_attitude_position_request->attitude_position_mocap().q().w();
        attitude_position_mocap.q.x =
            rpc_attitude_position_request->attitude_position_mocap().q().x();
        attitude_position_mocap.q.y =
            rpc_attitude_position_request->attitude_position_mocap().q().y();
        attitude_position_mocap.q.z =
            rpc_attitude_position_request->attitude_position_mocap().q().z();

        attitude_position_mocap.position_body.x_m =
            rpc_attitude_position_request->attitude_position_mocap().position_body().x_m();
        attitude_position_mocap.position_body.y_m =
            rpc_attitude_position_request->attitude_position_mocap().position_body().y_m();
        attitude_position_mocap.position_body.z_m =
            rpc_attitude_position_request->attitude_position_mocap().position_body().z_m();

        if (!fill_covariance(
                rpc_attitude_position_request->attitude_position_mocap().pose_covariance(),
                attitude_position_mocap.pose_covariance)) {
            fillResponseWithResult(response, mavsdk::Mocap::Result::INVALID_REQUEST_DATA);
            return grpc::Status::OK;
        }

        mavsdk::Mocap::Result mocap_result =
            _mocap.set_attitude_position_mocap(attitude_position_mocap);
        fillResponseWithResult(response, mocap_result);

        return grpc::Status::OK;
    }

    mavsdk::Mocap::Odometry::MavFrame
    translateRPCOdometryMavFrame(const mavsdk::rpc::mocap::Odometry::MavFrame mav_frame)
    {
        switch (mav_frame) {
            case mavsdk::rpc::mocap::Odometry::MavFrame::Odometry_MavFrame_MOCAP_NED:
                return mavsdk::Mocap::Odometry::MavFrame::MOCAP_NED;
            case mavsdk::rpc::mocap::Odometry::MavFrame::Odometry_MavFrame_LOCAL_FRD:
                return mavsdk::Mocap::Odometry::MavFrame::LOCAL_FRD;
        }
    }

    grpc::Status SetOdometry(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::mocap::SetOdometryRequest* rpc_odometry_request,
        mavsdk::rpc::mocap::SetOdometryResponse* response) override
    {
        mavsdk::Mocap::Odometry odometry{};

        odometry.time_usec = rpc_odometry_request->odometry().time_usec();

        odometry.frame_id =
            translateRPCOdometryMavFrame(rpc_odometry_request->odometry().frame_id());

        odometry.position_body.x_m = rpc_odometry_request->odometry().position_body().x_m();
        odometry.position_body.y_m = rpc_odometry_request->odometry().position_body().y_m();
        odometry.position_body.z_m = rpc_odometry_request->odometry().position_body().z_m();

        odometry.q.w = rpc_odometry_request->odometry().q().w();
        odometry.q.x = rpc_odometry_request->odometry().q().x();
        odometry.q.y = rpc_odometry_request->odometry().q().y();
        odometry.q.z = rpc_odometry_request->odometry().q().z();

        odometry.speed_body.x_m_s = rpc_odometry_request->odometry().speed_body().x_m_s();
        odometry.speed_body.y_m_s = rpc_odometry_request->odometry().speed_body().y_m_s();
        odometry.speed_body.z_m_s = rpc_odometry_request->odometry().speed_body().z_m_s();

        odometry.angular_velocity_body.roll_rad_s =
            rpc_odometry_request->odometry().angular_velocity_body().roll_rad_s();
        odometry.angular_velocity_body.pitch_rad_s =
            rpc_odometry_request->odometry().angular_velocity_body().pitch_rad_s();
        odometry.angular_velocity_body.yaw_rad_s =
            rpc_odometry_request->odometry().angular_velocity_body().yaw_rad_s();

        if (!fill_covariance(
                rpc_odometry_request->odometry().pose_covariance(), odometry.pose_covariance)) {
            fillResponseWithResult(response, mavsdk::Mocap::Result::INVALID_REQUEST_DATA);
            return grpc::Status::OK;
        }

        if (!fill_covariance(
                rpc_odometry_request->odometry().velocity_covariance(),
                odometry.velocity_covariance)) {
            fillResponseWithResult(response, mavsdk::Mocap::Result::INVALID_REQUEST_DATA);
            return grpc::Status::OK;
        }

        mavsdk::Mocap::Result mocap_result = _mocap.set_odometry(odometry);
        fillResponseWithResult(response, mocap_result);

        return grpc::Status::OK;
    }

    void stop() {}

private:
    Mocap& _mocap;
};

} // namespace backend
} // namespace mavsdk
