#include <future>

#include "telemetry/telemetry.h"
#include "telemetry/telemetry.grpc.pb.h"

namespace dronecore {
namespace backend {

template <typename Telemetry = Telemetry>
class TelemetryServiceImpl final : public dronecore::rpc::telemetry::TelemetryService::Service
{
public:
    TelemetryServiceImpl(Telemetry &telemetry)
        : _telemetry(telemetry),
          _stop_promise(std::promise<void>()),
          _stop_future(_stop_promise.get_future()) {}

    grpc::Status SubscribePosition(grpc::ServerContext * /* context */,
                                   const dronecore::rpc::telemetry::SubscribePositionRequest * /* request */,
                                   grpc::ServerWriter<rpc::telemetry::PositionResponse> *writer) override
    {
        _telemetry.position_async([&writer](dronecore::Telemetry::Position position) {
            auto rpc_position = new dronecore::rpc::telemetry::Position();
            rpc_position->set_latitude_deg(position.latitude_deg);
            rpc_position->set_longitude_deg(position.longitude_deg);
            rpc_position->set_relative_altitude_m(position.relative_altitude_m);
            rpc_position->set_absolute_altitude_m(position.absolute_altitude_m);

            dronecore::rpc::telemetry::PositionResponse rpc_position_response;
            rpc_position_response.set_allocated_position(rpc_position);
            writer->Write(rpc_position_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeHealth(grpc::ServerContext * /* context */,
                                 const dronecore::rpc::telemetry::SubscribeHealthRequest * /* request */,
                                 grpc::ServerWriter<rpc::telemetry::HealthResponse> *writer) override
    {
        _telemetry.health_async([&writer](dronecore::Telemetry::Health health) {
            auto rpc_health = new dronecore::rpc::telemetry::Health();
            rpc_health->set_is_gyrometer_calibration_ok(health.gyrometer_calibration_ok);
            rpc_health->set_is_accelerometer_calibration_ok(health.accelerometer_calibration_ok);
            rpc_health->set_is_magnetometer_calibration_ok(health.magnetometer_calibration_ok);
            rpc_health->set_is_level_calibration_ok(health.level_calibration_ok);
            rpc_health->set_is_local_position_ok(health.local_position_ok);
            rpc_health->set_is_global_position_ok(health.global_position_ok);
            rpc_health->set_is_home_position_ok(health.home_position_ok);

            dronecore::rpc::telemetry::HealthResponse rpc_health_response;
            rpc_health_response.set_allocated_health(rpc_health);
            writer->Write(rpc_health_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeHome(grpc::ServerContext * /* context */,
                               const dronecore::rpc::telemetry::SubscribeHomeRequest * /* request */,
                               grpc::ServerWriter<rpc::telemetry::HomeResponse> *writer) override
    {
        _telemetry.home_position_async([&writer](dronecore::Telemetry::Position position) {
            auto rpc_position = new dronecore::rpc::telemetry::Position();
            rpc_position->set_latitude_deg(position.latitude_deg);
            rpc_position->set_longitude_deg(position.longitude_deg);
            rpc_position->set_relative_altitude_m(position.relative_altitude_m);
            rpc_position->set_absolute_altitude_m(position.absolute_altitude_m);

            dronecore::rpc::telemetry::HomeResponse rpc_home_response;
            rpc_home_response.set_allocated_home(rpc_position);
            writer->Write(rpc_home_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeInAir(grpc::ServerContext * /* context */,
                                const dronecore::rpc::telemetry::SubscribeInAirRequest * /* request */,
                                grpc::ServerWriter<rpc::telemetry::InAirResponse> *writer) override
    {
        _telemetry.in_air_async([&writer](bool is_in_air) {
            dronecore::rpc::telemetry::InAirResponse rpc_in_air_response;
            rpc_in_air_response.set_is_in_air(is_in_air);
            writer->Write(rpc_in_air_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeArmed(grpc::ServerContext * /* context */,
                                const dronecore::rpc::telemetry::SubscribeArmedRequest * /* request */,
                                grpc::ServerWriter<rpc::telemetry::ArmedResponse> *writer) override
    {
        _telemetry.armed_async([&writer](bool is_armed) {
            dronecore::rpc::telemetry::ArmedResponse rpc_armed_response;
            rpc_armed_response.set_is_armed(is_armed);
            writer->Write(rpc_armed_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeGPSInfo(grpc::ServerContext * /* context */,
                                  const dronecore::rpc::telemetry::SubscribeGPSInfoRequest * /* request */,
                                  grpc::ServerWriter<rpc::telemetry::GPSInfoResponse> *writer) override
    {
        _telemetry.gps_info_async([this, &writer](dronecore::Telemetry::GPSInfo gps_info) {
            auto rpc_gps_info = new dronecore::rpc::telemetry::GPSInfo();
            rpc_gps_info->set_num_satellites(gps_info.num_satellites);
            rpc_gps_info->set_fix_type(translateGPSFixType(gps_info.fix_type));

            dronecore::rpc::telemetry::GPSInfoResponse rpc_gps_info_response;
            rpc_gps_info_response.set_allocated_gps_info(rpc_gps_info);
            writer->Write(rpc_gps_info_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    dronecore::rpc::telemetry::FixType translateGPSFixType(const int fix_type) const
    {
        switch (fix_type) {
            default:
            case 0:
                return dronecore::rpc::telemetry::FixType::NO_GPS;
            case 1:
                return dronecore::rpc::telemetry::FixType::NO_FIX;
            case 2:
                return dronecore::rpc::telemetry::FixType::FIX_2D;
            case 3:
                return dronecore::rpc::telemetry::FixType::FIX_3D;
            case 4:
                return dronecore::rpc::telemetry::FixType::FIX_DGPS;
            case 5:
                return dronecore::rpc::telemetry::FixType::RTK_FLOAT;
            case 6:
                return dronecore::rpc::telemetry::FixType::RTK_FIXED;
        }
    }

    void stop()
    {
        _stop_promise.set_value();
    }

private:
    Telemetry &_telemetry;
    std::promise<void> _stop_promise;
    std::future<void> _stop_future;
};

} // namespace backend
} // namespace dronecore
