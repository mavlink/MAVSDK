#include <future>

#include "telemetry/telemetry.h" // TODO: remove this include now that it is templated
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
